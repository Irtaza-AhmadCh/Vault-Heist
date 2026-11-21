# Vault Heist: The Living Vault --- Endless Mode Gameplay Specification

## 1. Purpose

Endless Mode is a single-player survival mode based on the AVL Tree
mechanics from Act 3.

The player must continuously process a stream of AVL operations while
keeping the tree valid. The stream contains both insertions and
deletions. The score is based on how long the player survives with a
valid AVL tree, and difficulty increases by increasing the rate at which
operations arrive.

Unlike campaign levels, Endless Mode has no fixed puzzle, fixed
operation count, or final level. The session continues until the player
fails.

**Important gameplay rule:** When an insertion or deletion makes the AVL
tree unbalanced, the player must repair it by performing the required
rotation through dragging. Rotations are not automatically completed for
the player in Endless Mode.

## 2. Source requirements and implementation decisions

The following requirements are explicitly stated in Project Deliverable
1:

-   Endless Mode is a survival mode.
-   It uses a stream of mixed insert and delete operations.
-   The score is survival time while the tree stays valid.
-   Difficulty ramps up by increasing the operation rate.
-   Endless Mode is accessible from the Main Menu.
-   The local leaderboard is associated with Endless Mode.
-   The save file stores the current Endless Mode tree and random seed.
-   AVL supports insertion, deletion, LL, RR, LR and RL rotations,
    height and size updates, and balance validation.
-   The general interaction specification supports dragging a subtree to
    a rotation pivot for manual AVL rotation.

The following details make those requirements implementable. They are
recommended gameplay decisions, not numerical or procedural rules
explicitly fixed by Deliverable 1:

-   The exact initial tree, operation interval, stage thresholds, and
    operation deadline.
-   The exact game-over handling for an expired operation.
-   The precise operation-card layout and interaction sequence.
-   Applying manual drag-based rotation repair to Endless Mode, using
    the same drag interaction established for Act 3 manual rotation
    levels.

## 3. Core gameplay loop

The complete loop is:

1.  Start Endless Mode.
2.  Generate or restore the current AVL tree.
3.  Start the survival timer.
4.  Generate one valid operation: INSERT or DELETE.
5.  Display the operation to the player.
6.  The player performs the requested operation.
7.  The AVL model updates the tree and identifies any imbalance.
8.  If the tree is unbalanced, the player repairs it using drag-based
    rotation.
9.  Validate BST ordering, heights, subtree sizes, and AVL balance.
10. If valid, count the operation as completed and continue.
11. Increase operation frequency as survival progresses.
12. End the session when the player fails a required operation or the
    tree is left invalid.

Only one operation should be active at a time. The next operation is
generated after the current operation has been completed and validated.
This keeps the task clear and prevents multiple unresolved tree changes.

## 4. Start screen

The Endless Mode entry screen should explain the objective:

> Keep the AVL tree balanced. Complete incoming insertions and
> deletions. Drag the correct subtree to the rotation pivot whenever the
> tree becomes unbalanced. Survive for as long as possible.

The screen should contain:

-   **Start:** begins a new session.
-   **Resume:** appears if a saved Endless Mode session is available.
-   **Leaderboard:** displays saved best survival times.
-   **Back:** returns to the Main Menu.

Starting a new session creates a new initial tree and random seed.
Resuming restores the saved tree and seed, subject to validation.

## 5. Initial AVL tree

At the beginning of a new session, generate a small, valid AVL tree. A
recommended starting size is 5--7 unique integer keys.

The initial tree must satisfy:

-   BST ordering: every key in the left subtree is smaller than its
    parent, and every key in the right subtree is larger.
-   Every node has a correct stored height.
-   Every node has a correct stored subtree size.
-   Every node has a balance factor in the valid AVL range of -1, 0, or
    +1.
-   No duplicate keys exist.

The exact starting values can be generated from a stored random seed so
that a session can be reproduced.

## 6. Main gameplay screen

The screen should be organized into the following areas.

### 6.1 Header / HUD

Show:

-   Survival time.
-   Current score, based on survival time.
-   Number of successfully completed operations.
-   Current difficulty stage or rate.
-   Current operation deadline, if the recommended deadline system is
    implemented.

The survival timer should be the most prominent score-related value.

### 6.2 AVL tree canvas

The main area displays the current AVL tree.

Each node should show:

-   Its key/value.
-   Its balance factor.
-   Optionally its height and subtree size, preferably in a tooltip or
    compact secondary badge.

Use the project's existing AVL colour convention:

-   Green: balanced node.
-   Yellow: balance factor of +1 or -1, if retaining the warning
    convention.
-   Red: node currently violating AVL balance.
-   Blue: currently selected or active node.
-   Gold: target node or rotation pivot.

Do not rely on colour alone. Use labels, outlines, or icons as
additional signals.

The tree canvas should support selecting nodes and dragging a subtree
during rotation repair. Background dragging and mouse-wheel zoom/pan
should remain available without accidentally moving tree nodes.

### 6.3 Incoming operation panel

Display one operation prominently:

-   `INSERT <key>` or
-   `DELETE <key>`

The panel should clearly state what the player must do. Do not ask the
player to invent a value: the operation generator supplies the key.

### 6.4 Operation status / event log

Show concise feedback such as:

-   Operation received.
-   Key inserted or deleted.
-   Imbalance detected at a node.
-   Rotation type required.
-   Rotation completed.
-   AVL validation passed or failed.

The event log should use the existing event-driven architecture.

### 6.5 Controls

Provide the controls needed for the current operation. Avoid showing
irrelevant actions. For example, while the player is repairing an
imbalance, the interface should focus on the highlighted rotation rather
than allowing another insertion or deletion.

## 7. Processing an INSERT operation

Example incoming request:

**INSERT 45**

1.  The operation panel displays `INSERT 45`.
2.  The player selects the Insert action and confirms the supplied key.
3.  The AVL model performs the normal BST insertion.
4.  Duplicate keys are rejected consistently with the project's
    data-structure rules. The tree must not be modified by a rejected
    duplicate.
5.  The model updates heights and subtree sizes along the affected path.
6.  The model checks balance factors.
7.  If no imbalance exists, the operation is complete after validation.
8.  If an imbalance exists, the game enters rotation-repair mode. The
    player must perform the required rotation by dragging.

The model must identify the required rotation case. The player should
not be expected to guess blindly: the affected node(s) and the required
case should be visibly indicated, while the player performs the drag
interaction.

## 8. Processing a DELETE operation

Example incoming request:

**DELETE 20**

1.  The operation panel displays `DELETE 20`.
2.  The player selects Delete and confirms the supplied key.
3.  The AVL model searches for the key and performs standard BST
    deletion.
4.  If the node has:
    -   **No children:** remove the leaf.
    -   **One child:** replace the node with its child.
    -   **Two children:** use the inorder successor (the minimum key in
        the right subtree), replace the deleted node's value as
        appropriate, and remove the successor node.
5.  Update heights and subtree sizes along the affected path.
6.  Check balance factors while moving back toward the root.
7.  If rebalancing is required, identify the appropriate rotation case.
8.  The player repairs the imbalance using drag-based rotation.
9.  Validate the complete AVL tree before accepting the operation as
    completed.

A deletion can require rebalancing at more than one ancestor. The game
must not assume that only one rotation is ever needed. It should present
the next required repair after each successful repair until the tree is
valid.

If the generated key is not present, the generator should avoid
producing that request. If an unexpected missing-key request occurs,
show a clear error and do not change the tree.

## 9. Rotation interaction --- drag to perform the rotation

**Rotations in Endless Mode are performed by the player through
dragging.** The AVL model detects the imbalance and identifies the
required rotation, but it does not silently complete the rotation.

This reuses the manual rotation interaction described for Act 3: the
player drags a subtree to the appropriate rotation pivot.

### 9.1 Rotation-repair state

When an operation creates an imbalance:

1.  Pause generation of new operations.
2.  Highlight the unbalanced node.
3.  Highlight the child and grandchild that determine the rotation case.
4.  Display the required case: LL, RR, LR, or RL.
5.  Show a short instruction indicating the drag target.
6.  Enable the relevant drag interaction.
7.  Keep the tree unchanged until a valid drag is completed.

### 9.2 Single rotations: LL and RR

For an LL case, the player performs the required right rotation by
dragging the relevant left subtree to the highlighted pivot.

For an RR case, the player performs the required left rotation by
dragging the relevant right subtree to the highlighted pivot.

The interface should visually communicate:

-   Which node/subtree is draggable.
-   Which node is the rotation pivot/target.
-   Where the dragged subtree will be placed.
-   Whether the proposed drop is valid.

On a valid drop, apply the rotation to the AVL model, update links,
heights and subtree sizes, and animate the resulting positions.

### 9.3 Double rotations: LR and RL

A double rotation requires two sequential drag actions.

-   **LR:** first perform the left rotation on the left child subtree,
    then perform the right rotation at the unbalanced node.
-   **RL:** first perform the right rotation on the right child subtree,
    then perform the left rotation at the unbalanced node.

The game should guide the player through the first drag and then update
the highlights for the second drag. Do not ask the player to complete
both movements in one ambiguous gesture.

After the second rotation, update heights and subtree sizes and validate
the affected tree.

### 9.4 Correct and incorrect drags

-   **Correct drag:** apply the required rotation, animate it, update
    metadata, and continue validation.
-   **Incorrect drop:** reject the drop and return the dragged subtree
    to its original position. Do not corrupt the model.
-   An incorrect drag may incur a time cost or a small mistake counter
    if the team wants a penalty system. This is an optional design
    decision; Deliverable 1 does not specify an Endless Mode rotation
    penalty.

The model must remain the source of truth. The GUI drag should request a
rotation; it must not independently rearrange visual nodes without
updating the AVL model.

## 10. Difficulty progression

Deliverable 1 specifies that difficulty increases by increasing the
operation rate. It does not prescribe exact stage durations or operation
intervals.

Recommended implementation:

-   Begin with a generous interval between operations.
-   Reduce the interval as survival time increases.
-   Keep the operation itself understandable and the required drag
    target visually clear.
-   Do not increase difficulty by changing AVL rules or introducing new
    data structures.

An example tuning table (to be play-tested):

  Stage              Suggested time to complete an operation
  ---------------- -----------------------------------------
  1 --- Beginner                                  10 seconds
  2 --- Normal                                     8 seconds
  3 --- Advanced                                   6 seconds
  4 --- Expert                                     4 seconds
  5 --- Extreme                                    3 seconds

These are example values, not requirements from the project brief.

A fair timing rule is to pause the operation deadline while the game is
playing a required tree animation. The deadline should run while the
player is expected to make a decision or perform a drag. The team should
decide whether the deadline applies to the full operation or only to the
player's interaction time.

The generator should create a valid sequence:

-   INSERT keys must not already exist.
-   DELETE keys must exist.
-   The generator should avoid allowing the tree to become empty.
-   The generator should maintain a reasonable minimum and maximum tree
    size.
-   The generator should use a reproducible random seed.

## 11. Scoring and leaderboard

The explicit Endless Mode scoring rule is:

**Score = survival time while the AVL tree stays valid.**

Recommended interpretation:

-   Start the survival timer when the session begins.
-   Keep it running during active gameplay.
-   Stop it when the session ends.
-   Use the final survival time as the primary leaderboard score.
-   Display completed operation count and highest difficulty reached as
    additional statistics, not replacements for survival time.

The project brief does not specify applying the campaign score formula,
alarm capacity, star system, or campaign penalties to Endless Mode. Do
not automatically apply those systems unless the team explicitly decides
to extend them.

Leaderboard entries should be stored locally and sorted by survival
time, with the longest survival ranked first.

## 12. Game-over conditions

Deliverable 1 says the player survives while the tree stays valid, but
does not fully define the failure screen or exact failure triggers.

Recommended game-over conditions:

1.  **Deadline expired:** the player did not complete the active
    operation/required rotation within the allowed time.
2.  **Invalid final tree:** a completed player action leaves the AVL
    tree invalid. In normal operation, validation should catch this
    before the state is accepted.
3.  **Explicit quit:** leaving the session should offer a choice to
    save/exit or abandon, rather than recording it as a normal survival
    failure.

An incorrect drag should normally be rejected without changing the
model. It should not instantly end the run unless the team deliberately
chooses a stricter rule.

## 13. End-of-session result screen

When the session ends, display:

-   Final survival time.
-   Final score (survival time).
-   Number of completed insertions.
-   Number of completed deletions.
-   Total operations completed.
-   Highest difficulty reached.
-   Reason the session ended.
-   Whether the result entered the local leaderboard.

Provide:

-   Play Again.
-   Leaderboard.
-   Main Menu.
-   Resume/Save option only if the project supports saving an
    in-progress session.


## 13. Scoreboard and local leaderboard

### 13.1 In-game scoreboard

During an active run, show:

- **Current score / survival time:** elapsed survival time while the AVL tree remains valid. This is the primary score.
- **Operations completed:** an optional supporting statistic showing how many INSERT and DELETE operations the player has completed.
- **Current difficulty / operation rate:** shows the current pace of incoming operations.
- **Current operation:** the active INSERT or DELETE request.
- **Time remaining:** show this only if the team implements a per-operation deadline.

The score itself is based on survival time. Operation count and difficulty are informational statistics; they do not add to or replace the score.

### 13.2 End-of-run scoreboard

When the run ends, show:

- **Final score:** the player's total survival time. This is the score recorded for the leaderboard.
- **Operations completed:** optional run statistic.
- **Highest difficulty reached:** optional run statistic.
- **End reason:** for example, the operation deadline expired or the player chose to exit.

Do not request or record a player name. Endless Mode's scoreboard and leaderboard are score-only.

### 13.3 Local top-10 leaderboard

The Project Deliverable 1 game flow specifies:

**Main Menu → Endless Mode → Leaderboard (local file).**

For Endless Mode, implement a local leaderboard containing the **top 10 scores only**:

- Each entry consists of a rank and a score.
- The score is the player's survival time.
- Sort scores from longest survival time to shortest.
- Keep only the best 10 scores.
- Do not display or store player names.
- Do not apply campaign stars, alarm-step scoring, or campaign penalties to Endless Mode.

The leaderboard screen should be accessible from the Main Menu and from the Endless Mode result screen.

### 13.4 Example top-10 leaderboard

| Rank | Score (survival time) |
|---:|---:|
| 1 | 08:42 |
| 2 | 07:15 |
| 3 | 06:38 |
| 4 | 05:52 |
| 5 | 05:21 |
| 6 | 04:48 |
| 7 | 04:12 |
| 8 | 03:56 |
| 9 | 03:24 |
| 10 | 02:51 |

These are example values only. The actual leaderboard must display the ten highest scores achieved in completed runs.

### 13.5 Suggested stored record

The local leaderboard only needs to store the score for each entry. A simple representation is:

```text
EndlessLeaderboard
    topScores[10]    // survival times, sorted highest to lowest
```

When a run ends:

1. Take the final survival time as the score.
2. Compare it with the current top 10.
3. If it qualifies, insert it in the correct position.
4. Remove the lowest score if the list now contains more than 10 entries.
5. Save the updated top 10 to the local leaderboard file.

If fewer than 10 runs have been recorded, show the available scores and leave the remaining positions empty.

## 14. Save and resume

Project Deliverable 1 explicitly says the save file stores the current
Endless Mode tree and random seed.

For a resumable session, store enough additional state to continue
consistently:

-   Current AVL tree serialization.
-   Random seed and generator state (or enough information to reproduce
    the next operation).
-   Survival time elapsed.
-   Current difficulty/rate stage.
-   Operation counter.
-   Any currently pending operation, if saving is allowed mid-operation.

On load:

1.  Reconstruct the tree.
2.  Recompute AVL heights and subtree sizes rather than trusting stored
    values.
3.  Run the validator.
4.  Restore the random generator state and session information.
5.  Resume only if the tree passes validation.

The save/load implementation should follow the project's general
SaveManager design.

## 15. Architecture and implementation responsibilities

Follow the project's event-driven architecture:

**Input → Level Logic → AVL Model → Event Log → Animation Controller →
Renderer**

The AVL model must remain independent of the GUI.

Suggested responsibilities:

### Endless Mode controller

-   Start, pause, resume, and end the session.
-   Track survival time and difficulty.
-   Request the next operation.
-   Track the current operation and its deadline.
-   Update the local leaderboard.

### Operation generator

-   Generate valid insertions and deletions.
-   Avoid duplicate insertions and missing-key deletions.
-   Use a reproducible random seed.
-   Respect configured tree-size limits.

### AVL model

-   Execute insertion and deletion.
-   Identify imbalance and required rotation cases.
-   Apply a requested rotation after the player completes the drag.
-   Update height and subtree size.
-   Validate BST ordering and AVL balance.

### Drag interaction / rotation controller

-   Identify the draggable subtree and valid pivot.
-   Show valid drop targets.
-   Reject invalid drops without changing the model.
-   Send a rotation request to the AVL model after a valid drop.
-   Play the resulting rotation events.

### Event and animation system

Reuse the existing event architecture. Relevant events include:

-   `INSERT`
-   `UNLINK`
-   `SWAP_VALUE`
-   `ROTATE`
-   `UPDATE_HEIGHT`
-   `UPDATE_SIZE`
-   `BALANCE_ALERT`
-   `DONE`

The exact event payloads should follow the common project event
contract.

## 16. Example session walkthrough

This is an illustrative sequence, not a fixed scripted level.

### Start

-   Generate a valid AVL tree.
-   Start the timer.
-   Display the first request.

### Request 1: INSERT 45

-   Player submits the insertion.
-   The AVL model inserts 45.
-   The model detects an RL imbalance.
-   The game highlights the affected nodes and displays `RL`.
-   The player performs the first drag for the right rotation on the
    left child subtree.
-   The player performs the second drag for the left rotation at the
    unbalanced node.
-   The model updates metadata and validates the tree.
-   The operation is counted as complete.

### Request 2: DELETE 20

-   Player submits the deletion.
-   The model performs the appropriate BST deletion case.
-   The model updates heights and sizes.
-   If an imbalance appears, the game highlights the required rotation.
-   The player performs the required drag-based rotation(s).
-   The tree is validated.

### Later requests

-   Operations continue to arrive.
-   The time between requests becomes shorter as difficulty rises.
-   The player continues inserting, deleting, and manually repairing
    imbalances through dragging.
-   The session ends when the player fails the agreed survival
    condition.

## 17. Acceptance criteria

Endless Mode is ready when all of the following work:

-   [ ] Endless Mode can be opened from the Main Menu.
-   [ ] A new session creates a valid AVL tree.
-   [ ] The game generates a stream containing both INSERT and DELETE
    operations.
-   [ ] Generated insertions never use an existing key.
-   [ ] Generated deletions always refer to an existing key.
-   [ ] Insertions and deletions update the AVL model correctly.
-   [ ] The model detects LL, RR, LR and RL cases.
-   [ ] The player performs rotations through dragging.
-   [ ] LL and RR can be completed with the appropriate single drag
    interaction.
-   [ ] LR and RL guide the player through two sequential drag actions.
-   [ ] Invalid drops do not corrupt the tree.
-   [ ] Heights and subtree sizes are correct after every operation and
    rotation.
-   [ ] The validator confirms BST ordering and AVL balance.
-   [ ] Operation rate increases as survival progresses.
-   [ ] Survival time is recorded as the primary score.
-   [ ] Results can be added to and displayed on the local leaderboard.
-   [ ] The current Endless Mode tree and random seed can be saved.
-   [ ] A loaded tree is recalculated and validated before play resumes.

## 18. Decisions to confirm before implementation

The core gameplay is defined above. The team should still confirm these
tuning and policy details:

1.  Exact initial tree size and key range.
2.  Exact operation-rate schedule and whether a per-operation deadline
    is used.
3.  Maximum and minimum allowed tree size.
4.  Whether incorrect drag attempts have a penalty, and what that
    penalty is.
5.  Whether an in-progress operation can be saved.
6.  Whether the survival timer pauses during rotation animations.
7.  Whether the leaderboard stores only the best time per player or
    multiple runs.

These decisions should be finalized without changing the core
requirements: mixed insertions and deletions, manual drag-based
rotations, survival-time scoring, and increasing operation rate.

## 19. Source alignment

This specification is based on **Project Deliverable 1**, particularly:

-   **Page 2:** Main Menu → Endless Mode → local leaderboard; general
    scoring context.
-   **Page 4:** Act 3 Endless specification --- survival mode, mixed
    insertions/deletions, survival-time score, increasing operation
    rate.
-   **Page 5:** AVL model and event-driven architecture.
-   **Page 6:** Rotation events and undo/replay architecture.
-   **Page 8:** AVL operations, rotation primitives, height/size
    updates, and validator.
-   **Page 9:** General GUI interaction --- dragging a subtree to a
    rotation pivot for manual AVL rotation.
-   **Page 10:** Save file includes the current Endless Mode tree and
    random seed; AVL height and size are recomputed on load and the
    validator runs before play.

Where the brief does not specify exact Endless Mode values or behaviour,
this document labels the proposed details as implementation
recommendations rather than source requirements.
