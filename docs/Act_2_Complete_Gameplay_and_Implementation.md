# Vault Heist: The Living Vault
# Act 2 – The Sorted Wing (BST)
## Gameplay Workflow and Implementation Specification

**Document purpose:** This document consolidates the Act 2 requirements from *Project Deliverable 1*, the agreed gameplay decisions, and the implementation recommendations accepted during discussion. It is intended to guide implementation without changing the project's existing architecture or coding guardrails.

---

## 1. Act 2 Overview

Act 2 is titled **The Sorted Wing** and introduces the Binary Search Tree (BST).

A BST stores values according to this ordering rule:

- Values smaller than a node's key belong in its left subtree.
- Values larger than a node's key belong in its right subtree.
- Duplicate keys are not allowed. An insertion of a duplicate must be rejected and the GUI must display a clear message.

Act 2 contains five levels:

| Level | Name | Main concept |
|---|---|---|
| 2.1 | Search Heist | Searching in a BST |
| 2.2 | Stash Loot | Inserting into a BST |
| 2.3 | Steal and Collapse | Deleting a BST node |
| 2.4 | Range Raid | Range query with subtree pruning |
| 2.5 | Boss: The Lazy Architect | BST degeneration caused by sorted insertion |

The player should learn the BST rules by interacting with the tree rather than simply watching an explanation. The GUI must make comparisons, visited nodes, insertion positions, deletion cases, and skipped subtrees understandable.

### 1.1 Act 2 learning progression

The levels build on one another:

1. **2.1:** Learn how a BST search follows comparisons from the root.
2. **2.2:** Learn how the same comparison rule determines where a new value is inserted.
3. **2.3:** Learn the three structural cases involved in deleting a value.
4. **2.4:** Learn how BST ordering allows entire subtrees to be skipped during a range query.
5. **2.5:** Experience how inserting sorted values can destroy the expected efficiency of a BST, and understand why balanced trees such as AVL trees are needed.

---

## 2. Shared BST Rules and Interaction Conventions

These rules apply throughout Act 2.

### 2.1 BST ordering

For every node:

- Every key in its left subtree is smaller than the node's key.
- Every key in its right subtree is larger than the node's key.
- No duplicate key is stored.

The BST implementation, not the GUI, is responsible for enforcing the ordering rule.

### 2.2 Node selection

- The player interacts with visible tree nodes and, where appropriate, visible empty child positions.
- The GUI sends the player's choice to level logic.
- Level logic validates the choice against the BST model.
- The model performs the actual operation.
- The model/operation produces events.
- The animation controller and renderer display those events.

The GUI must not directly change node pointers or implement BST algorithms.

### 2.3 Alarm convention

The project deliverable defines the core alarm rule:

- Entering/visiting a node costs one alarm step.
- Comparing the target or inserted key with a node's key costs one alarm step.
- A visit and its comparison must not accidentally be counted twice if they represent the same logical step in the implementation. The team must use one consistent accounting convention and document it.
- When the relevant alarm meter reaches its capacity, the stage/level fails according to its rules.

The core project capacity rule is:

\[
\text{Alarm Capacity} = \lceil 1.5 \times \text{optimalSteps} \rceil + 2
\]

The existing general scoring rules are described in Section 8. Level 2.5 has an additional agreed two-stage arrangement described in Section 7.

### 2.4 Common feedback

The GUI should clearly distinguish:

- Correct selection.
- Incorrect direction or choice.
- Current node being examined.
- Current comparison result.
- Empty child position / `NULL`.
- Successful operation.
- Failed or invalid operation.
- Alarm steps used and remaining capacity.

Colour should not be the only indicator. Use text, outlines, icons, or other visual cues alongside colour.

---

# 3. Level 2.1 – Search Heist

## 3.1 Objective

The player must find a given target value in a BST by navigating through the correct sequence of nodes.

This level teaches that BST search uses comparisons to decide whether to continue left or right.

## 3.2 Initial screen

The level screen displays:

- A valid BST structure.
- The edges connecting its nodes.
- The target value the player must find.
- An alarm meter.
- The level objective and instructions.

**The node keys are initially hidden.** Each node displays a placeholder such as `?`. This prevents the player from simply reading the entire tree and encourages them to reveal nodes through the search process.

The BST itself is generated internally with real integer keys. Hiding a key is only a GUI presentation choice; it does not mean that the model stores an unknown value.

## 3.3 Player workflow

1. The player is given a target value.
2. Initially, only the root node is selectable.
3. The player clicks the root.
4. The root's actual key is revealed.
5. The game compares the target with the revealed key.
6. The player must choose the correct next direction:
   - If target is smaller, choose the left child.
   - If target is larger, choose the right child.
   - If target equals the current key, the target has been found.
7. The selected child is revealed.
8. The same process continues until the target is found or the required child position is `NULL`.

The game must validate each selected direction against the BST comparison. The player should not be allowed to jump to an arbitrary node.

## 3.4 Target found

If the player reaches a node whose key equals the target:

- Mark the node as found.
- Play the successful search animation.
- Complete the level.
- Show the result screen with steps, penalties, score, and stars.

## 3.5 Target not found

The level must also support targets that are not present in the BST.

When the correct search path reaches a missing child:

- Show that the required left or right child is `NULL`.
- Display a **Target Not Found** button beneath the tree.
- The player clicks this button to submit the conclusion that the target does not exist.

The game validates the button press:

- If the player has followed the correct search path and the required child is actually `NULL`, the answer is correct and the level completes.
- If the player presses the button before reaching the correct `NULL` position, it is an incorrect answer. Apply the project's wrong-traversal penalty of `+1` alarm step.
- The game should provide feedback explaining why the answer was premature or incorrect.

### Example

Suppose the target is `30`, and the search path is:

`50 → 40 → 35`

At node `35`, the target is smaller, so the player must go left. If the left child is `NULL`, the player can click **Target Not Found**. The target is not present in the tree.

## 3.6 Alarm and penalties

- Correct search progress consumes alarm according to the project's node-visit/comparison convention.
- An incorrect traversal direction uses the documented penalty: `+1` alarm step.
- Do not count a correct comparison twice merely because both a visit event and comparison event are displayed.
- A hint, if used, applies the project's general hint rule: the maximum achievable star rating is reduced by one.

## 3.7 Relevant model operations and events

Model operation:

- `search(key)`

Useful events:

- `VISIT`
- `COMPARE`
- A result event such as `FOUND` or `NOT_FOUND`, if the existing event vocabulary supports it.

The data structure should return enough information for the level logic and GUI to show the path. Do not make the BST depend on SFML.

---

# 4. Level 2.2 – Stash Loot

## 4.1 Objective

The player must insert a supplied value into an existing BST by choosing the correct search path and the correct empty child position.

This level teaches that BST insertion follows the same comparison rule as searching.

## 4.2 Initial screen

The screen displays:

- An existing BST with all real node values visible.
- The new value that must be inserted.
- The alarm meter.
- Empty child positions for the current leaves.

### Empty child positions

For each leaf node, show both possible empty child positions:

- A left empty-child circle/slot.
- A right empty-child circle/slot.

These circles are **visual placeholders for `NULL` child pointers**. They are not actual BST nodes and must not be stored as nodes in the BST model.

The renderer derives these placeholders from the real tree:

- If `node->left == NULL`, it may draw a left empty slot.
- If `node->right == NULL`, it may draw a right empty slot.

The actual BST stores only real nodes.

## 4.3 Player workflow

1. Display the value to insert.
2. The player starts at the root.
3. The player clicks the current node to perform/reveal the comparison.
4. If the new value is smaller, the player must move left.
5. If the new value is larger, the player must move right.
6. Continue until the correct child pointer is `NULL`.
7. The player clicks the corresponding empty-child slot.
8. The game validates that this is the correct insertion position.
9. The BST performs its normal `insert()` operation.
10. The new node appears at the selected position through an insertion animation.

The player is choosing the path and destination. The actual insertion and pointer updates are performed by the BST implementation.

## 4.4 Incorrect direction and recovery

- A wrong direction incurs the project's documented wrong-traversal penalty: `+1` alarm step.
- The game displays feedback explaining the comparison.
- The player may be allowed to backtrack and continue from the correct path. If recovery is allowed, the incorrect-action penalty remains recorded.
- The game must not silently accept a wrong path.

This recovery behavior is a gameplay recommendation accepted for the interaction: it allows the player to correct their route while preserving the cost of the mistake.

## 4.5 Duplicate values

If the supplied value already exists:

- The BST must reject the insertion.
- The GUI must display a visible duplicate-key message.
- No duplicate node may be created.

This is a project-wide BST requirement, not merely a Level 2.2 rule.

## 4.6 Completion

When the player selects the correct empty child slot and the insertion succeeds:

- Animate the new node appearing.
- Update the tree layout.
- Confirm successful insertion.
- Complete the level when its insertion objective is satisfied.

## 4.7 Relevant model operations and events

Model operation:

- `insert(key)`

Useful events:

- `COMPARE`
- `INSERT`

The renderer draws `NULL` placeholders from the model's current child pointers; no placeholder-node objects should be inserted into the BST.

---

# 5. Level 2.3 – Steal and Collapse

## 5.1 Objective

The player must delete a specified value from a BST and identify which of the three BST deletion cases applies.

The three cases are:

1. Leaf node — no children.
2. One-child node — exactly one child.
3. Two-child node — both children exist.

The player first finds the target, identifies the deletion case, and then watches/participates in the corresponding deletion process.

## 5.2 Initial workflow: find the target

1. Give the player a target value to delete.
2. Use the same general search interaction introduced in Level 2.1.
3. The player follows comparisons from the root.
4. When the target is found, highlight the target node.
5. Present the three deletion-case choices.

The search portion should reuse the established Level 2.1 interaction rather than inventing a completely different search system.

## 5.3 Case selection

Present these choices:

- `leaf`
- `one_child`
- `two_children`

The player must select the case they believe applies to the highlighted target.

### Correct choice

- Confirm the selected case.
- Start the corresponding deletion animation.
- Execute the actual deletion through the BST model.

### Incorrect choice

- Display feedback explaining the actual child configuration.
- Allow the player to try selecting the correct case again.
- Do not invent an alarm penalty for this mistake: the project deliverable does not specify a penalty for choosing the wrong deletion case.

A separate penalty could be introduced only if the project team explicitly agrees to change the scoring specification.

## 5.4 Case 1: deleting a leaf

A leaf has no left or right child.

Workflow:

1. Highlight the target leaf.
2. Emit an `UNLINK` event.
3. Remove the parent's link to the leaf.
4. Animate/fade the leaf out.
5. Update the tree layout.

## 5.5 Case 2: deleting a node with one child

A one-child node has exactly one non-`NULL` child.

Workflow:

1. Highlight the target node and its only child.
2. Link the target's parent directly to the target's child.
3. Remove the target node.
4. Animate the child taking the target node's former position.
5. Update the tree layout.

The model performs the pointer changes. The GUI only visualizes the emitted operation events.

## 5.6 Case 3: deleting a node with two children

For two children, use the inorder successor.

The inorder successor is the **smallest value in the target's right subtree**. It is found by moving to the target's right child and then repeatedly moving left until no further left child exists.

Agreed player interaction:

1. Highlight the target node.
2. Highlight the target's right subtree.
3. Ask the player to identify/select the inorder successor manually.
4. Validate the selected node:
   - Correct selection: continue.
   - Incorrect selection: show feedback and allow another selection.
5. Once the correct successor is selected, emit `SWAP_VALUE`.
6. Animate the target and successor values swapping.
7. Delete the original successor node, which now contains the target value.
8. Update the tree layout.

The model must perform a correct BST deletion. The animation must not merely exchange displayed labels while leaving the model in an invalid state.

### Worked example

Initial BST:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

Delete `50`.

- The target has two children.
- Its right subtree is rooted at `70`.
- The smallest value in that right subtree is `60`.
- Therefore, `60` is the inorder successor.

After swapping values and deleting the original successor node:

```text
             60
           /    \
         30      70
        /  \       \
      20   40      80
```

The resulting tree remains a valid BST.

## 5.7 Relevant model operations and events

Model operation:

- `delete(key)`

Useful events:

- `VISIT`
- `COMPARE`
- `HIGHLIGHT_SUBTREE`
- `UNLINK`
- `SWAP_VALUE`

The exact event payloads should identify the affected nodes and contain enough information for the animation and, where supported, undo/replay.

---

# 6. Level 2.4 – Range Raid

## 6.1 Objective from the project specification

The player performs a range query for a supplied interval:

`[lo, hi]`

The GUI greys out subtrees that can be skipped. The scoring rewards pruning.

This level teaches that BST ordering allows the algorithm to avoid examining subtrees that cannot contain values inside the requested range.

## 6.2 Required behavior

- Give the player a lower bound `lo` and upper bound `hi`.
- Perform a range query over the BST.
- Identify values that lie within the inclusive interval `[lo, hi]`.
- Visually distinguish included values from excluded values.
- Grey out subtrees that the range-query algorithm can safely prune.
- Reward effective pruning according to the level's scoring configuration.

The BST API should expose:

- `rangeQuery(lo, hi)`

The project specification states that the range query uses inorder traversal with pruning and emits `HIGHLIGHT_SUBTREE` events for skipped subtrees.

## 6.3 Important implementation boundary

The project deliverable specifies the range-query objective and the greyed-out skipped subtrees, but it does not fully prescribe the exact mouse interaction sequence for this level.

Therefore, implement the required range-query behavior and pruning visualization first. Any additional player-driven node-selection interaction should be treated as a gameplay extension and agreed upon before it is made mandatory.

## 6.4 Relevant events

- `VISIT`
- `COMPARE`
- `HIGHLIGHT_SUBTREE`

The event payload should identify the root of the skipped subtree so that the renderer can grey out the entire corresponding region.

---

# 7. Level 2.5 – Boss: The Lazy Architect

## 7.1 Objective and learning purpose

This is the final boss level of Act 2.

The project deliverable describes the core idea:

- Deliveries arrive in sorted order.
- Inserting them into a normal BST makes the vault become a corridor.
- The tree height reaches `n` for `n` nodes.
- The player must attempt a search under a shrinking alarm capacity.
- The level ends with a cutscene explaining why balancing is necessary.

The agreed detailed design divides this boss level into **two mandatory stages**:

1. BST insertion stage.
2. BST search demonstration stage.

The two stages have separate alarm meters. The insertion stage determines the official score and stars. The search stage is compulsory but complementary and educational; it does not contribute to the final score or star ranking.

## 7.2 Stage A – Sorted insertion challenge

### Purpose

The player constructs a BST by inserting a supplied sorted array. This deliberately creates a skewed BST.

### Initial screen

Display:

- A sorted array of integer values.
- An initially empty BST/tree canvas.
- Insertion instructions.
- The insertion-stage alarm meter.
- The insertion-stage score/penalty information.

Example array:

```text
[10, 20, 30, 40, 50, 60, 70]
```

### Player interaction

The agreed interaction is similar to the tree-building interaction used in Act 1 Level 1.2:

1. The player selects a value from the displayed array.
2. The player places the first value as the root.
3. Once a node is placed, the GUI can show its left and right child positions.
4. The player selects the next value from the array.
5. The player chooses the left or right direction according to the BST ordering rule.
6. The player continues following the correct BST path until the correct empty child position is reached.
7. The player selects that empty child position to insert the value.
8. The game validates the player's path and destination.
9. The actual BST `insert()` operation creates the node.
10. The GUI animates the insertion and displays the updated tree.
11. Repeat until every value in the supplied array has been inserted.

The game must validate the actual BST insertion position. The GUI must not independently construct or mutate the tree.

### Why sorted insertion creates a corridor

For ascending values:

- `10` becomes the root.
- `20` is greater than `10`, so it goes right.
- `30` is greater than `10` and `20`, so it goes right again.
- `40` follows the same pattern.
- This continues for every value.

The resulting tree is:

```text
10
  \
   20
     \
      30
        \
         40
           \
            50
              \
               60
                 \
                  70
```

For `n` ascending, distinct values, this produces a right-skewed chain with `n` nodes and height `n` when height is counted in nodes (or `n - 1` edges when counted in edges). Follow the project's single documented height convention consistently.

The BST remains valid: it is the shape that becomes inefficient, not the ordering.

### Insertion-stage alarm and scoring

The insertion stage has its own alarm tracking and is the **only scoring stage** of Level 2.5.

Record the following during insertion:

- Insertion alarm steps used.
- Insertion penalties.
- Insertion bonuses, if any are supported by the level.
- Insertion score.
- Insertion star result.
- Whether insertion was completed successfully.

Use the project's normal alarm-capacity rule for this stage:

\[
\text{Insertion Alarm Capacity}
=
\lceil 1.5 \times \text{insertionOptimalSteps} \rceil + 2
\]

Use the project's standard scoring formula:

\[
\text{Insertion Score}
=
\max(0,\ 1000 - 10 \times \text{insertionAlarmSteps}
- \text{insertionPenalties} + \text{insertionBonuses})
\]

The insertion stage must follow the normal project completion/failure rules. If the insertion alarm reaches capacity before the insertion objective is completed, the player fails/retries according to the normal rules and must not bypass the insertion stage to reach the search explanation.

### When insertion completes

Once all values have been inserted successfully:

1. Stop insertion-stage input.
2. Finalize the insertion-stage result.
3. Save the insertion alarm steps, penalties, score, and stars.
4. Mark the insertion objective as completed.
5. Preserve this result so that the search stage cannot overwrite it.
6. Transition to Stage B.

**The official Level 2.5 score and stars are determined at this point from insertion performance.**

## 7.3 Stage B – Search challenge

### Purpose

The search stage demonstrates the consequence of the skewed BST. The player must experience that searching this tree can take linear time, even though a balanced search tree can search in logarithmic time.

This stage is mandatory, but it is not a scoring stage.

### Separate search alarm

The search stage has its own independent alarm meter and budget.

- Reset the meter when Stage B begins.
- Do not carry over the insertion-stage alarm usage.
- Do not change the saved insertion-stage alarm record.
- Do not add search-stage penalties to the insertion-stage penalties.
- Do not calculate or award a separate search score or stars.
- Search-stage results must not change the official Level 2.5 score or star ranking.

### Search budget

For the educational challenge, use a logarithmic comparison budget based on the number of nodes. The agreed example uses:

\[
\text{Search Budget} = \lceil \log_2(n+1) \rceil
\]

This represents an approximate worst-case comparison allowance for a balanced BST with `n` nodes. It is a deliberately limited challenge budget for this boss level; it is not the actual worst-case complexity of the skewed BST.

For seven nodes:

\[
\lceil \log_2(7+1) \rceil
=
\lceil \log_2(8) \rceil
=
3
\]

The search challenge therefore allows three comparisons in this example.

**Important distinction:** The budget is based on balanced-tree search performance, while the player is searching the skewed BST they just constructed. That contrast is intentional.

### Target selection

A completely random target could be near the root and might be found within the small budget. To ensure that the boss demonstrates the problem:

- Choose the deepest node as the target, or choose from the deepest available nodes.
- For an ascending sorted array that created a right-skewed chain, the largest value is the deepest node.
- This makes the required search path predictable and ensures the player experiences the long search.

### Example search

For the seven-node tree:

```text
10
  \
   20
     \
      30
        \
         40
           \
            50
              \
               60
                 \
                  70
```

Choose `70` as the target.

The required search path is:

```text
10 → 20 → 30 → 40 → 50 → 60 → 70
```

That is seven comparisons if each visited node is counted once. The search budget is three comparisons.

The search meter should update after each comparison. The player reaches the budget before reaching `70`, so the search challenge ends as an educational failure.

### What “shrinking alarm capacity” means in this design

Keep these concepts separate:

- **Total search budget:** The fixed logarithmic limit calculated when the search stage begins.
- **Remaining search budget:** The amount left after each comparison.
- **Alarm used:** The number of comparisons already consumed.

The total budget does not randomly decrease after every insertion. The search stage starts with a separate, deliberately small logarithmic budget, and the remaining portion decreases as the player searches.

For example, with a total search budget of 3:

| Search action | Comparisons used | Comparisons remaining |
|---|---:|---:|
| Search starts | 0 | 3 |
| Compare at 10 | 1 | 2 |
| Compare at 20 | 2 | 1 |
| Compare at 30 | 3 | 0 |

At the limit, the challenge ends and the explanation screen is shown. The player cannot continue searching indefinitely.

The insertion alarm and search alarm must never share the same counter or capacity field.

## 7.4 Stage C – Educational explanation and AVL reveal

After the search budget is exhausted:

1. Stop search input.
2. Show an explanation that the BST became skewed because the values were inserted in sorted order.
3. Explain that a skewed BST can require `O(n)` comparisons in the worst case.
4. Generate an AVL tree containing the exact same values.
5. Display the balanced AVL tree.
6. Explain that AVL balancing maintains logarithmic height, so search remains `O(log n)` in the worst case.
7. Complete the educational sequence and mark Level 2.5 complete.

### AVL tree generation

Use the project's existing AVL insertion implementation to construct the balanced tree from the same values.

- Do not manually hardcode the final AVL arrangement.
- Insert the same values into an AVL tree using its normal `insert()` operation.
- Allow the AVL implementation to perform the required rotations.
- Render the resulting AVL tree.
- The values must be identical to those in the player's BST; only their structure differs.

An illustrative balanced arrangement for the example values is:

```text
             40
           /    \
         20      60
        /  \    /  \
      10   30  50   70
```

The actual shape must be the result of the project's AVL insertion algorithm. The diagram above illustrates the intended balanced result for the example.

### Suggested explanation text

> **Why did the search fail?**
>
> You inserted the values in sorted order. Each new value became the right child of the previous value, creating a skewed BST. A skewed BST can require `O(n)` comparisons to find a value near the end of the chain.
>
> A balanced tree keeps its height small. AVL trees automatically maintain balance through rotations after insertion and deletion. This keeps search, insertion, and deletion at `O(log n)` time in the worst case.
>
> The values in the two trees are the same. The difference is their structure.

## 7.5 Official scoring and stars for Level 2.5

**Only Stage A (insertion) determines the final score and star ranking.**

The search stage is compulsory, but it is complementary and educational. It does not contribute to the final result.

At the end of insertion, preserve:

- `insertion_alarm_steps`
- `insertion_penalties`
- `insertion_bonuses`
- `insertion_score`
- `insertion_stars`
- `insertion_completed`

The search stage may separately track:

- `search_alarm_steps`
- `search_budget`
- `search_completed`
- `search_challenge_failed` (the expected outcome when the deliberately limited budget is exhausted)

These search values are for gameplay flow, demonstration, and debugging only. They must not affect the official score or stars.

### Final result rule

```cpp
final_score = insertion_result.score;
final_stars = insertion_result.stars;
```

The search result must not overwrite either value.

### Suggested result data model

A simple data model can keep the two stage records independent:

```cpp
struct StageResult
{
    int alarm_steps;
    int penalties;
    int bonuses;
    int score;
    int stars;
    bool completed;
};

StageResult insertion_result;
StageResult search_result;
```

The search stage does not need to calculate a meaningful score or star value. If the shared structure is reused for convenience, those fields must be ignored for the search stage and must never be copied into the final level result.

A more explicit design can use a separate search record:

```cpp
struct SearchStageResult
{
    int alarm_steps;
    int search_budget;
    bool completed;
    bool challenge_failed;
};
```

Choose the simpler form that fits the existing project code. Do not introduce unnecessary abstraction.

### Star ranking

Use the project's existing star rules against the insertion-stage measurements:

- **3 stars:** Steps are at or below optimal and there are no penalties.
- **2 stars:** Steps are at or below `1.25 × optimal` and there is at most one penalty.
- **1 star:** The insertion objective is completed within its alarm capacity.
- **0 stars:** The insertion alarm becomes full; retry is required.

The project's hint rule also applies if hints are available: using a hint reduces the maximum achievable star cap by one.

The search challenge does not reduce stars, add penalties, or alter the saved insertion score.

## 7.6 Level 2.5 complete workflow

```text
LEVEL 2.5 START
      |
      v
Show sorted array
      |
      v
Initialize insertion alarm
      |
      v
Player inserts all values into BST
      |
      +---- Insertion alarm full? ---- Yes ---> Insertion failure / retry
      |
      No
      |
      v
All values inserted correctly?
      |
      v
Save insertion steps, penalties, bonuses,
score and stars as the official result
      |
      v
Start separate search stage
      |
      v
Calculate logarithmic search budget
      |
      v
Select deepest BST value as target
      |
      v
Player searches the skewed BST
      |
      v
Search budget exhausted before target?
      |
      v
Explain skewed BST and O(n) worst-case search
      |
      v
Build AVL from the same values
      |
      v
Show balanced AVL and explain O(log n)
      |
      v
Complete Level 2.5
      |
      v
Show final result using insertion score and stars only
```

---

# 8. Act 2 Scoring, Alarm and Penalty Rules

## 8.1 General scoring formula

The project deliverable defines:

\[
\text{Score}
=
\max(0,\ 1000 - 10 \times \text{alarmSteps}
- \text{penalties} + \text{bonuses})
\]

The score cannot fall below zero.

## 8.2 Star rules

| Result | Stars |
|---|---:|
| Steps ≤ optimal and no penalties | 3 |
| Steps ≤ 1.25 × optimal and at most one penalty | 2 |
| Completed within alarm capacity | 1 |
| Alarm full | 0; retry |

The conditions should be evaluated in the intended priority order so that the best qualifying result is awarded.

## 8.3 Documented penalties relevant to Act 2

The project deliverable specifies:

- Wrong traversal click: `+1` alarm step.
- Using a hint: maximum star rating reduced by one.

The project deliverable also specifies penalties for rotation choices and Act 4 heap operations; those are not Act 2 penalties.

Do not invent additional Act 2 penalties and treat them as existing project requirements. In particular:

- Level 2.3 wrong deletion-case selection has no specified penalty.
- Level 2.3 incorrect successor selection has no specified penalty.
- Any additional penalty must be approved as a design change before implementation.

## 8.4 Level 2.5 scoring isolation

Level 2.5 is the special case with two alarm meters:

| Metric | Insertion stage | Search stage |
|---|---|---|
| Alarm counter | Independent | Independent |
| Alarm capacity | Normal project capacity rule | Logarithmic challenge budget |
| Penalties | Counted for final result | Not included in final result |
| Score | Calculated | No score |
| Stars | Calculated | No stars |
| Mandatory | Yes | Yes |
| Educational purpose | Tests BST insertion | Demonstrates BST degeneration |

Do not reset or overwrite the insertion result when starting the search stage.

---

# 9. GUI and Screen Requirements for Act 2

Act 2 uses the shared game screen components described in the project deliverable.

## 9.1 Main level screen

The screen should include:

- Act and level identification.
- Mission/objective text.
- Tree canvas.
- Alarm meter.
- Score display where appropriate.
- Steps used / maximum steps.
- Operation or interaction instructions.
- Event log.
- Undo, replay, reset, hint, and speed controls where supported by the shared game interface.

## 9.2 Tree canvas

The tree canvas must support:

- Node and edge rendering.
- Node selection.
- Current-node highlighting.
- Search-path highlighting.
- Comparison feedback.
- Empty-child placeholders for insertion levels.
- Greyed-out skipped subtrees for Range Raid.
- Deletion highlighting and animations.
- Layout recalculation after structural changes.

## 9.3 Level-specific GUI behavior

| Level | Required visual behavior |
|---|---|
| 2.1 | Hide keys initially; reveal visited nodes; show search path and `Target Not Found` control when the required child is `NULL`. |
| 2.2 | Show existing keys and empty child slots for leaves; animate the inserted node. |
| 2.3 | Highlight target, show deletion-case choices, highlight right subtree and successor for the two-child case, animate unlink/swap. |
| 2.4 | Show `[lo, hi]`; grey out safely skipped subtrees; distinguish values in the range. |
| 2.5 insertion | Show sorted array and player-built BST; display insertion alarm and insertion scoring data. |
| 2.5 search | Show the same skewed BST, target, and separate search alarm budget. |
| 2.5 explanation | Show why the skewed BST search failed and reveal an AVL tree containing the same values. |

## 9.4 Level 2.5 result screen

The official result screen must display:

- Insertion-stage score.
- Insertion-stage stars.
- Insertion-stage alarm steps.
- Insertion-stage penalties.
- Level completion status.

Search-stage alarm usage may be shown in an educational breakdown if useful, but it must be clearly labelled as **not included in score or stars**.

---

# 10. Data Structure and Event Implementation

## 10.1 BST model responsibilities

The BST layer should implement the operations needed by Act 2:

- `insert(key)`
- `search(key)`
- `delete(key)`
- `findMin()`
- `findMax()`
- `successor(key)` or equivalent successor lookup
- `predecessor(key)`
- `rangeQuery(lo, hi)`
- `isValidBST()`

The project deliverable describes the BST API and deletion behavior. Reuse the existing implementation wherever it already exists.

## 10.2 Level logic responsibilities

The level logic is responsible for:

- Loading or generating the level's BST and target values.
- Tracking the current player interaction.
- Validating player-selected directions and positions.
- Calling the appropriate BST operation.
- Tracking level objectives and completion.
- Applying alarm and penalty rules.
- Calculating score and stars.
- For Level 2.5, keeping insertion and search records separate.
- Triggering the explanatory AVL reveal after the search challenge.

The level logic should not duplicate the BST algorithms.

## 10.3 Event-driven communication

The architecture is event-driven:

```text
Player Input
    |
    v
GUI Input Handler
    |
    v
Level Logic / GameEngine
    |
    v
BST or AVL Model
    |
    v
Event Log
    |
    v
Animation Controller
    |
    v
Renderer
```

The model does not know about the GUI. It reports what happened through events; the GUI animates those events.

## 10.4 Act 2 event vocabulary

Use the existing event format and naming conventions. Relevant event types include:

- `VISIT`
- `COMPARE`
- `INSERT`
- `UNLINK`
- `SWAP_VALUE`
- `HIGHLIGHT_SUBTREE`

The architecture document also gives general examples such as `DELETE` and `ROTATE`. Do not create duplicate event names for the same operation. Agree on the exact event type strings centrally.

Each event should include enough information to identify the affected node(s) and support animation, undo, or replay where the existing event system requires it.

## 10.5 Empty child placeholders

For Level 2.2 and Level 2.5 insertion:

- Do not allocate actual BST nodes to represent empty positions.
- Use the tree's `NULL` child pointers as the source of truth.
- The layout/renderer may draw an empty circle at a `NULL` child position.
- When a real insertion occurs, the model creates a real node and the renderer replaces the placeholder with that node.

## 10.6 Level 2.5 two-stage result separation

Keep two distinct stage records. For example:

```cpp
struct InsertionStageResult
{
    int alarm_steps;
    int penalties;
    int bonuses;
    int score;
    int stars;
    bool completed;
};

struct SearchStageResult
{
    int alarm_steps;
    int search_budget;
    bool completed;
    bool challenge_failed;
};
```

The final level result must read only from `InsertionStageResult`.

This separation is important because the search challenge is intentionally expected to exhaust its limited budget. That expected educational failure must not turn the entire level into a failed scored attempt.

---

# 11. Suggested Implementation Breakdown

Implement Act 2 in small, testable pieces. Do not rewrite working BST code to fit the GUI.

## 11.1 Shared foundations

1. Confirm the existing BST node and class interfaces.
2. Confirm the event type strings and event payload format.
3. Confirm how the game records alarm steps and penalties.
4. Confirm how the score system calculates stars.
5. Confirm how the renderer identifies nodes and computes their positions.
6. Confirm the existing undo/replay behavior before adding new event types.

## 11.2 Level 2.1 implementation

1. Prepare or generate a valid BST and target.
2. Add a GUI display mode that hides keys.
3. Make only the root selectable at the beginning.
4. Reveal a node after it is selected.
5. Validate the player's left/right decision.
6. Record the search path and alarm steps.
7. Add the `Target Not Found` action when the correct next child is `NULL`.
8. Test both found and not-found targets.

## 11.3 Level 2.2 implementation

1. Render the existing BST with visible keys.
2. Derive empty child slots from `NULL` pointers.
3. Let the player select the value to insert.
4. Validate the route using BST comparisons.
5. Validate the selected empty child position.
6. Call the existing BST `insert()` operation.
7. Animate the new node.
8. Test duplicate rejection and wrong-direction recovery.

## 11.4 Level 2.3 implementation

1. Provide the target key.
2. Reuse Level 2.1 search interaction.
3. Highlight the target node.
4. Determine its child configuration.
5. Present the three deletion cases.
6. Validate the player's case choice.
7. Implement and animate leaf deletion.
8. Implement and animate one-child deletion.
9. Implement successor selection and two-child deletion.
10. Test the resulting BST ordering after every case.

## 11.5 Level 2.4 implementation

1. Provide `lo` and `hi`.
2. Implement or reuse `rangeQuery(lo, hi)`.
3. Emit events for visited nodes and pruned subtrees.
4. Grey out skipped subtrees.
5. Highlight returned values.
6. Connect pruning performance to the level's scoring configuration.
7. Confirm the precise player-selection mechanic with the team if one is to be added.

## 11.6 Level 2.5 implementation

1. Prepare a sorted array of distinct integer values.
2. Display the array and an empty BST canvas.
3. Implement the player-driven root placement.
4. Show left/right child positions after each insertion.
5. Let the player select each next value and its correct BST insertion path.
6. Validate each selected path and empty child position.
7. Call the existing BST `insert()` operation for every value.
8. Record insertion alarm steps, penalties, and bonuses.
9. On insertion completion, calculate and save the official score and stars.
10. Freeze the insertion result record.
11. Initialize a separate search alarm counter and logarithmic budget.
12. Choose a deepest-node target.
13. Run the search challenge and consume one search step per comparison.
14. Stop the search when the budget is exhausted.
15. Display the skewed-BST explanation.
16. Insert the same values into the existing AVL implementation.
17. Render the resulting AVL tree.
18. Complete the level and show the saved insertion score and stars only.

---

# 12. Testing Checklist

## 12.1 BST model tests

- [ ] Smaller values are inserted to the left.
- [ ] Larger values are inserted to the right.
- [ ] Duplicate values are rejected.
- [ ] Search finds an existing key.
- [ ] Search correctly reports a missing key.
- [ ] Leaf deletion works.
- [ ] One-child deletion works for both left-child and right-child cases.
- [ ] Two-child deletion uses the inorder successor correctly.
- [ ] Range query returns only values inside `[lo, hi]`.
- [ ] Range query pruning does not skip a valid result.
- [ ] BST validity is preserved after insertion and deletion.

## 12.2 Level 2.1 tests

- [ ] Only the root is selectable initially.
- [ ] Node keys are hidden before being visited.
- [ ] A visited node reveals its actual key.
- [ ] Smaller target directs the player left.
- [ ] Larger target directs the player right.
- [ ] Equal target completes the search.
- [ ] Correct `NULL` path enables `Target Not Found`.
- [ ] Premature `Target Not Found` is rejected and penalized.
- [ ] Alarm steps are not double-counted.

## 12.3 Level 2.2 tests

- [ ] Existing node values are visible.
- [ ] Empty child slots are rendered from `NULL` pointers.
- [ ] Empty placeholders are not actual BST nodes.
- [ ] Correct path leads to the correct insertion position.
- [ ] Wrong direction is penalized.
- [ ] Recovery does not erase the penalty.
- [ ] Correct insertion creates one real node.
- [ ] Duplicate insertion is rejected visibly.

## 12.4 Level 2.3 tests

- [ ] Search reaches the target before case selection.
- [ ] Leaf case is identified correctly.
- [ ] One-child case is identified correctly.
- [ ] Two-child case is identified correctly.
- [ ] Incorrect case choice allows retry.
- [ ] Leaf node is unlinked correctly.
- [ ] One-child node is replaced by its child.
- [ ] Inorder successor is the minimum of the right subtree.
- [ ] Incorrect successor selection is rejected.
- [ ] Correct successor selection performs the value swap and successor removal.
- [ ] BST validity remains true after each deletion case.

## 12.5 Level 2.4 tests

- [ ] Inclusive lower and upper bounds work.
- [ ] Values outside the range are excluded.
- [ ] Valid values are never hidden by pruning.
- [ ] Pruned subtrees are visibly greyed out.
- [ ] Pruning events identify the correct subtree.

## 12.6 Level 2.5 tests

- [ ] The supplied array is sorted and contains distinct values.
- [ ] Player must insert all values.
- [ ] Sorted insertion creates a skewed chain.
- [ ] For `n` nodes, height is `n` under node-count convention.
- [ ] Insertion alarm is independent from search alarm.
- [ ] Insertion penalties are recorded.
- [ ] Insertion score uses the standard score formula.
- [ ] Insertion stars use the standard star rules.
- [ ] Insertion result is saved before the search stage begins.
- [ ] Search budget is calculated from the number of nodes.
- [ ] Search starts with a reset alarm meter.
- [ ] Search target is selected from the deepest node(s).
- [ ] Search alarm is consumed per comparison.
- [ ] Search stops when the separate search budget is exhausted.
- [ ] Search-stage alarm does not alter insertion-stage alarm.
- [ ] Search-stage penalties do not alter the official score.
- [ ] Search stage does not award or remove stars.
- [ ] AVL is built from the same values using the actual AVL implementation.
- [ ] Final screen uses insertion-stage score and stars only.

---

# 13. Coding and Architecture Guardrails

All Act 2 implementation must follow the uploaded project guardrails and architecture.

## 13.1 Keep the architecture layered

The project uses layered architecture with event-driven communication.

Responsibilities:

- **Data structures:** BST and AVL algorithms and node relationships.
- **Game/level logic:** Objectives, player choices, validation, alarm, scoring, and level transitions.
- **Events:** Records model operations for animation, undo, and replay.
- **GUI:** SFML rendering, input handling, and animation.
- **Utilities:** Validation, saving, loading, and supporting functions.

Do not put BST algorithms in GUI code.

## 13.2 C++ coding constraints

- Use beginner-friendly C++.
- Do not use STL containers such as `std::vector`.
- Do not use built-in tree or heap implementations.
- Implement data structures manually using structs, arrays, pointers, and dynamic memory where appropriate.
- Enums are prohibited.
- Use centrally defined lowercase `snake_case` string literals for string-based identifiers.
- Use `.h` files for shared declarations/interfaces and `.cpp` files for implementations.
- Avoid templates, unnecessary globals, operator overloading, and try/catch unless explicitly agreed.
- Avoid over-engineering and unnecessary abstractions.
- Preserve existing architecture, folder structure, naming conventions, and working code.
- Keep SFML-specific code out of the core BST/AVL implementation wherever possible.

## 13.3 Event-driven model

The model should describe what happened. The GUI should animate it.

Do not:

- Make BST methods draw directly to the screen.
- Make the renderer change tree pointers.
- Store GUI-only `NULL` placeholders as real BST nodes.
- Duplicate the BST insertion/search/deletion algorithm inside level logic.
- Let search-stage metrics overwrite insertion-stage metrics in Level 2.5.

## 13.4 Reuse existing components

Reuse:

- BST insertion, search, deletion, and range-query operations.
- AVL insertion and rotation implementation.
- Tree layout and rendering components.
- Alarm/score infrastructure, with separate stage records for Level 2.5.
- Event log and animation controller.
- Shared result-screen components.

Extend existing components only where necessary. Do not replace working code merely to match the examples in this document.

---

# 14. Decisions, Requirements, and Scope Boundaries

This section distinguishes what comes directly from the project deliverable from the detailed gameplay decisions agreed during discussion.

## 14.1 Direct project requirements

- Act 2 is The Sorted Wing and uses a BST.
- Smaller keys go left and larger keys go right.
- Duplicates are rejected with a visible message.
- Level 2.1 is BST search; the player navigates left/right and alarm counts comparisons.
- Level 2.2 is BST insertion; the player chooses the path and the new node appears at the correct leaf.
- Level 2.3 is BST deletion with leaf, one-child, and two-child cases; the player selects the case; the two-child case uses the inorder successor and a value swap.
- Level 2.4 is a range query `[lo, hi]`; skipped subtrees are greyed out and scoring rewards pruning.
- Level 2.5 uses sorted deliveries to create a corridor-shaped BST of height `n`; the player attempts a search under a shrinking alarm capacity; the level explains why balancing is needed.
- The core alarm capacity and scoring/star rules are defined by the project deliverable.

## 14.2 Detailed gameplay decisions agreed during discussion

- Level 2.1 hides node keys initially and reveals them as the player visits nodes.
- Level 2.1 uses a `Target Not Found` button when the correct next child is `NULL`.
- Level 2.2 shows empty child circles for leaf nodes; these are GUI placeholders, not actual BST nodes.
- Level 2.2 uses click-based path and empty-slot selection rather than requiring drag-and-drop.
- Level 2.2 allows recovery from a wrong direction while retaining the penalty.
- Level 2.3 reuses the Level 2.1 search interaction.
- Level 2.3 asks the player to identify the deletion case before the operation.
- Level 2.3 allows retry after a wrong case choice; no extra penalty is assumed.
- Level 2.3 lets the player manually select the inorder successor.
- Level 2.5 has two mandatory stages: insertion and search.
- Level 2.5 insertion uses a sorted array and player-driven insertion interaction similar to Act 1 Level 1.2.
- Level 2.5 insertion intentionally creates a skewed BST.
- Level 2.5 uses separate insertion and search alarm meters/counters.
- Level 2.5 final score and stars are based only on insertion.
- Level 2.5 search is compulsory and educational but does not affect score, penalties, or stars.
- Level 2.5 search uses a logarithmic challenge budget based on node count to demonstrate the contrast with the skewed BST's linear worst case.
- Level 2.5 chooses a deepest target so the search challenge reliably demonstrates the long path.
- After the search challenge, the game explains the skewed BST and shows an AVL tree built from the same values.

## 14.3 Items that remain implementation choices or need team confirmation

- Exact BST generation rules and difficulty values for Levels 2.1–2.4.
- Exact optimal-step calculation for each insertion level.
- Whether the game records one alarm step per visit, per comparison, or one combined step when a visit and comparison are the same action. The project wording says both visiting and comparing cost one step; the implementation must choose a consistent interpretation that does not accidentally double-count a single logical action.
- Exact scoring bonus configuration, if bonuses are used.
- Exact player-driven interaction for Level 2.4 beyond the required range-query and pruning visualization.
- Exact animation timings and event payload details.
- Exact search target selection among deepest nodes if there are multiple equally deep nodes.
- Whether the explanatory AVL reveal is displayed side-by-side with the skewed BST or as a replacement screen.
- Exact level result-screen layout.

These choices should be finalized consistently with the existing project architecture and team interfaces. Do not silently treat an unconfirmed option as a project requirement.

---

# 15. Final Expected Outcome

When Act 2 is implemented, the player should understand and be able to demonstrate:

- How a BST orders values.
- How searching follows comparisons.
- How a missing target is identified through a `NULL` child.
- How insertion finds the correct empty position.
- Why duplicate keys are rejected.
- How the three BST deletion cases work.
- Why the inorder successor is used for two-child deletion.
- How range-query pruning skips irrelevant subtrees.
- Why sorted insertion can create a skewed BST.
- Why a skewed BST can require `O(n)` search time.
- Why AVL balancing maintains logarithmic height and `O(log n)` worst-case search.

**Most important Level 2.5 rule:** The insertion stage is the scored test of the player's skill. The search stage is a mandatory demonstration of the consequence of degeneration. The final score and star ranking must always come from the saved insertion-stage result alone.
