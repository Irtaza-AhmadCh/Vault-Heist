# Vault Heist --- Act 4, Level 4.1 Gameplay and Implementation Specification

**Level:** 4.1 --- Activate the Vault\
**Act:** 4 --- The Heap Fortress\
**Data structure:** Max Heap\
**Primary learning goals:** Complete binary tree, Max Heap property,
bottom-up heap construction\
**Audience:** Gameplay, GUI, data-structure, and AI implementation
agents

------------------------------------------------------------------------

## 1. Purpose and scope

This document defines the proposed player-facing workflow and
implementation behaviour for Level 4.1. It turns the level requirement
into a concrete, beginner-friendly interaction that can be implemented
with the existing C++/SFML architecture.

The player receives an unsorted array of treasure values. The same data
is displayed in two synchronised ways:

1.  **Heap Tree View:** the binary-tree representation of the array.
2.  **Array View:** the underlying zero-based array.

The player helps build a Max Heap using **bottom-up construction**. The
game chooses which parent to process and highlights it. The player
inspects the highlighted parent and its children, chooses the larger
child when a swap is needed, and confirms the swap. The game performs
the actual model operation and updates both views through events.

### Design principles

-   Use mouse clicks and confirmation buttons, not drag-and-drop.
-   Do not ask the player to choose the next parent or calculate array
    indices.
-   Let the game guide the player through bottom-up heap construction.
-   Keep the data structure implementation separate from SFML and
    rendering.
-   Use one MaxHeap model as the source of truth; the tree view is
    derived from the array.
-   Keep incorrect actions understandable and recoverable.
-   Do not add extra mechanics that are not needed to teach this level's
    concepts.

------------------------------------------------------------------------

## 2. Source requirements versus proposed interaction details

The project brief specifies that Level 4.1 teaches complete binary
trees, the Max Heap property, and bottom-up heap construction. It says
the player selects or performs valid swaps to build a Max Heap, and
success occurs when every parent is greater than or equal to its
existing children.

The brief also requires synchronised heap-tree and array views. A swap
must be reflected in both views.

The detailed click sequence, confirmation button, highlight behaviour,
and exact error messages in this document are **implementation
proposals** to make those requirements easy to play and implement. They
are not additional verbatim requirements from the project brief.

### Important penalty distinction

The project-wide scoring table specifies: - Wrong traversal click: +1
alarm. - Wrong rotation choice: +2 alarm and -50 points. - Hint use:
maximum star rating is capped at 1 star. - Incorrect parent-child
comparison or invalid swap in Act 4: +1 alarm.

Therefore, **the -50-point wrong-rotation penalty belongs to the Act 3
rotation mechanic. It must not automatically be applied to Level 4.1.**
For Level 4.1, the specified consequence for an incorrect parent-child
comparison or invalid swap is +1 alarm. Do not add a -50-point Act 4
penalty unless the project team explicitly changes the brief.

------------------------------------------------------------------------

## 3. Level entry and briefing workflow

### Screen 1 --- Level selection

1.  The player opens Act 4 --- The Heap Fortress.
2.  The player selects **Level 4.1 --- Activate the Vault**.
3.  The level briefing screen appears.

### Screen 2 --- Briefing

Display:

-   **Mission:** Organise the treasures.
-   **Objective:** Build a valid Max Heap.
-   **Instructions:** Follow the highlighted parent. Compare it with its
    children. If a child is larger, select the largest child and confirm
    the swap.
-   **Alarm meter:** Show current alarm and maximum capacity.
-   **Controls:** Mouse click, Confirm, Continue, Undo, Reset, Hint.

The player clicks **Start Level**.

### Screen 3 --- Gameplay

The level screen contains:

1.  **Level header:** Act 4, Level 4.1, mission name.
2.  **Objective/instruction panel:** tells the player what to do next.
3.  **Heap Tree View:** nodes and edges showing the complete binary
    tree.
4.  **Array View:** cells labelled with their indices and values.
5.  **Alarm meter and score panel:** current alarm, alarm capacity,
    score, penalties, and steps.
6.  **Action controls:** Confirm Swap, No Swap Needed, Continue, Undo,
    Reset, Hint.
7.  **Event/message panel:** explains the most recent operation or
    error.

The array is generated or loaded from the level data. For the worked
example in this document, use:

``` text
[4, 10, 3, 5, 1]
```

Both views must display the same values in the same array-index
positions.

------------------------------------------------------------------------

## 4. Player interaction model

### 4.1 Highlighted parent

The game calculates the last non-leaf index and starts there. For an
array of `n` elements using zero-based indexing:

``` text
last_non_leaf = floor(n / 2) - 1
```

For `n = 5`, the last non-leaf index is `1`.

The game highlights: - The current parent. - Its existing left child, if
any. - Its existing right child, if any. - The corresponding array
cells.

The game also displays a short instruction, for example:

> Check whether 10 is greater than or equal to its children.

The player does not select the next parent. The game controls the
processing order.

### 4.2 Selecting a child

If one or both children are larger than the parent:

1.  The player clicks the child they believe has the greatest value.
2.  The selected node and corresponding array cell receive a selection
    highlight.
3.  The interface displays the proposed swap, for example `Swap 4 ↔ 10`.
4.  The player clicks **Confirm Swap**.
5.  The game validates the choice.
6.  If correct, the MaxHeap model swaps the values and emits the
    relevant events.
7.  The tree and array animate the same swap.

Only a child of the currently highlighted parent may be selected for
this swap. Do not allow arbitrary array-cell swaps.

### 4.3 No swap needed

If the parent is already greater than or equal to both existing
children:

1.  The player clicks **No Swap Needed**.
2.  The game checks the current parent and its existing children.
3.  If the parent satisfies the Max Heap condition, the game accepts the
    action and moves on.
4.  If a child is larger, the action is incorrect and the alarm
    increases by one.

A leaf has no children. The game should automatically mark a leaf as
complete rather than asking the player to make a no-op decision.

### 4.4 Continue after a subtree is repaired

After a correct swap, the value that moved down may still be smaller
than one of its new children. The game must continue sift-down from the
value's new index.

The player repeats the child-selection and confirmation process until
the value is in a valid position.

Once the current sift-down path is complete, the game moves to the next
parent index, decreasing the index by one.

------------------------------------------------------------------------

## 5. Complete worked gameplay example

Use this example to understand the expected player experience and to
test the implementation.

### Initial array

``` text
[4, 10, 3, 5, 1]
```

The corresponding complete binary tree is:

``` text
          4
        /   \
      10     3
     /  \
    5    1
```

### Step 1 --- Process parent at index 1

The last non-leaf index is `1`, so the game highlights value `10`.

Its children are: - Left child: `5` - Right child: `1`

The player sees that `10` is greater than both children.

**Player action:** Click **No Swap Needed**.

**Game response:** Accept the action. Mark this subtree as valid. Move
to parent index `0`.

No swap is performed.

### Step 2 --- Process parent at index 0

The game highlights root value `4` and its children: - Left child:
`10` - Right child: `3`

The player must select the largest child, which is `10`.

**Player action:** Click node `10`, then click **Confirm Swap**.

**Game response:** Validate that: - The selected node is a child of the
current parent. - It is the largest existing child. - It is greater than
the parent.

The swap is valid. The array becomes:

``` text
[10, 4, 3, 5, 1]
```

The tree becomes:

``` text
          10
        /    \
       4      3
      / \
     5   1
```

The game animates the same value exchange in the tree and array.

### Step 3 --- Continue sift-down

The value `4` moved to index `1`. The game now checks its children: -
Left child: `5` - Right child: `1`

The player selects `5` and clicks **Confirm Swap**.

The array becomes:

``` text
[10, 5, 3, 4, 1]
```

The tree becomes:

``` text
          10
        /    \
       5      3
      / \
     4   1
```

Now `5` is greater than `4` and `1`. The sift-down path is complete.

### Step 4 --- Finish construction

There are no more parent indices to process.

The game validates the complete array: - Index `0`: parent `10`,
children `5` and `3`. - Index `1`: parent `5`, children `4` and `1`.

Every parent is greater than or equal to its existing children.

**Final Max Heap:**

``` text
[10, 5, 3, 4, 1]
```

The level is completed.

------------------------------------------------------------------------

## 6. Alarm meter, steps, penalties, and scoring

This section is important because the implementation must not confuse
**alarm steps**, **penalty points**, and **the -50-point Act 3 rotation
penalty**.

### 6.1 Alarm capacity

The project brief defines alarm capacity as:

``` text
alarm_capacity = ceil(1.5 * optimalSteps) + 2
```

When the alarm meter reaches its capacity, the level is failed.

For example, if this level's calculated `optimalSteps` is `6`:

``` text
alarm_capacity = ceil(1.5 * 6) + 2
               = 9 + 2
               = 11
```

The displayed meter could show:

``` text
Alarm: 4 / 11
```

The number `6` in this example is illustrative. The level configuration
or a deterministic calculation must supply the actual `optimalSteps`; it
must not be guessed independently by the GUI.

### 6.2 What counts as an alarm step in Level 4.1?

The project-wide core mechanic says that comparing against a room's key
costs one alarm step. The Act 4 scoring rule additionally says that an
incorrect parent-child comparison or invalid swap adds one alarm.

Use the following consistent interpretation for Level 4.1:

  -----------------------------------------------------------------------
  Player/game action                                         Alarm effect
  ------------------------------ ----------------------------------------
  Correct parent-child                                                 +1
  comparison                     

  Incorrect parent-child                        +1 additional error alarm
  comparison / wrong child       
  choice                         

  Correctly confirmed required      No extra alarm beyond the comparisons
  swap                           

  Invalid swap attempt                                           +1 alarm

  Correct No Swap Needed action     No extra alarm beyond the comparisons
                                                             already made

  Incorrect No Swap Needed                                       +1 alarm
  action                         

  Automatic highlighting or                                             0
  animation                      

  Moving to the next parent                                             0
  automatically                  

  Opening the briefing or result                                        0
  screen                         

  Using Undo or Reset               0 by default; see implementation note
                                                                    below

  Using a hint                                  Apply the hint rule below
  -----------------------------------------------------------------------

**Important:** A comparison should be counted once. Do not charge one
alarm step for the same comparison in both the GUI and the MaxHeap
model. The game logic/level controller should be the single authority
that updates the alarm.

For an incorrect selection, the comparison that revealed the mistake is
still a comparison step; the additional incorrect-action alarm is
applied only if the level logic classifies it as an incorrect
parent-child choice. To avoid accidental double counting, implement this
as one clearly defined action result with an explicit alarm cost.

### 6.3 Defining `optimalSteps`

The project brief uses `optimalSteps` in the alarm-capacity formula but
does not give a fixed number for Level 4.1.

Recommended implementation:

1.  Run a deterministic bottom-up Max Heap construction on a copy of the
    level's initial array.
2.  Count the parent-child comparisons made by the standard sift-down
    procedure.
3.  Use that count as the level's `optimalSteps`.
4.  Store or calculate it once when the level is loaded.
5.  Use the same counting convention during gameplay.

This makes the capacity depend on the actual input array and the
algorithm's expected efficient construction. Do not count automatic
animations, highlights, or screen transitions as optimal steps.

**Team confirmation:** The exact project-wide meaning of `optimalSteps`
should be agreed and documented once for all acts. The comparison-count
approach above is a practical Level 4.1 proposal.

### 6.4 What happens when the alarm reaches capacity?

-   The level immediately enters the failed state when
    `alarmSteps >= alarmCapacity`.
-   Stop accepting gameplay actions.
-   Stop starting new heap operations.
-   Show a failure/result screen with alarm steps, capacity, score, and
    penalties.
-   Offer **Retry** and, if the campaign design allows it, **Return to
    Act Map**.
-   Retry reloads the original level array and resets the level's
    counters.

Do not allow the player to finish an operation after the alarm capacity
has already been reached.

### 6.5 Penalty points and the -50 rule

The project brief gives a specific `-50 points` penalty for a **wrong
rotation choice** in Act 3. Level 4.1 has no rotation-choice mechanic.

For Level 4.1: - Wrong child selection: alarm consequence as defined
above; no additional -50 points. - Invalid swap: +1 alarm; no additional
-50 points. - Incorrect No Swap Needed: +1 alarm; no additional -50
points. - Hint use: star cap of 1; do not invent an extra point
deduction unless the team defines one consistently.

The `-50` rotation penalty must remain in the Act 3 rotation gameplay.
Do not apply it to Level 4.1 merely because the user asked to define the
scoring rules.

If the project team later decides that an Act 4 error should also cost
points, that is a new game-rule decision and must be recorded as a
change to the project specification.

### 6.6 Score formula

The project brief defines:

``` text
score = 1000 - (10 * alarmSteps) - penalties + bonuses
score = max(0, score)
```

For Level 4.1: - `alarmSteps` is the total alarm count for this
attempt. - `penalties` is the total applicable point deductions. Under
the current brief, there is no -50 rotation penalty in this level. -
`bonuses` should be zero unless the team defines a specific bonus for
this level. - The score cannot fall below zero.

Example, assuming: - `alarmSteps = 4` - `penalties = 0` - `bonuses = 0`

``` text
score = 1000 - (10 * 4) - 0 + 0
      = 960
```

If a future, explicitly approved Act 4 point penalty of 50 were added,
it would be represented in `penalties = 50`; it must not be silently
assumed in the current implementation.

### 6.7 Stars

Use the project-wide star thresholds:

  Result                                                      Stars
  ------------------------------------------------------ ----------
  Steps `<= optimalSteps` and no penalties                        3
  Steps `<= 1.25 * optimalSteps` and at most 1 penalty            2
  Level completed within alarm capacity                           1
  Alarm reaches capacity                                   0; retry

The project brief also says using a hint caps the maximum star rating at
1.

Implementation notes: - Use the project's agreed definition of `steps`
consistently. Recommended: use the same comparison/action-step counter
used for the optimal-step comparison. - Do not treat alarm capacity as
the optimal-step target. - If the level is failed, award zero stars
regardless of the intermediate score. - If a hint was used, apply
`stars = min(calculatedStars, 1)`.

------------------------------------------------------------------------

## 7. Wrong-action cases and expected responses

### Case A --- Player selects the smaller child

Example: - Parent = `4` - Left child = `10` - Right child = `3` - Player
selects `3`

Expected response: 1. Reject the selection. 2. Do not modify the array
or tree. 3. Add the specified incorrect-action alarm. 4. Display a
concise message: **"Choose the larger child."** 5. Keep the current
parent highlighted so the player can try again.

### Case B --- Player selects a node that is not a child

Expected response: 1. Reject the selection. 2. Do not modify the array
or tree. 3. Add +1 alarm for the invalid swap attempt. 4. Display:
**"You can only swap the highlighted parent with one of its children."**

Prefer preventing this action in the GUI by making only the current
parent's children selectable. Keep the model/game-logic validation
anyway.

### Case C --- Player confirms a swap that is not needed

If the parent is already greater than or equal to both children, no swap
is required.

Expected response: 1. Reject the swap. 2. Do not modify the array or
tree. 3. Add +1 alarm. 4. Display: **"This parent already satisfies the
Max Heap property."**

### Case D --- Player clicks No Swap Needed when a swap is required

Expected response: 1. Reject the action. 2. Do not modify the array or
tree. 3. Add +1 alarm. 4. Display: **"A child is greater than the
parent. Select the largest child."**

### Case E --- Player selects the parent itself

Expected response: - Do not allow it to become a swap candidate. - Do
not modify the model. - If the click is treated as an invalid swap
attempt, add +1 alarm according to the invalid-swap rule. - Prefer
disabling the parent as a selectable swap target to prevent unnecessary
errors.

### Case F --- Player clicks a leaf or a node with only one child

-   A leaf is automatically complete.
-   If a parent has one child, compare only the existing child.
-   Never assume that a right child exists when a left child does not.
-   A one-child parent can only select its existing child.

------------------------------------------------------------------------

## 8. Undo, Reset, Hint, and Replay

### Undo

Recommended simple implementation: take an array snapshot before each
accepted player action.

When the player clicks Undo: 1. Restore the previous array snapshot. 2.
Rebuild or refresh the tree view from the restored array. 3. Restore the
current parent index and sift-down position. 4. Restore the relevant
level progress state. 5. Restore the alarm and score counters to their
pre-action values, if Undo is intended to reverse the whole action.

**Team decision to confirm:** The brief allows snapshot-based undo but
does not explicitly state whether undo restores alarm/score. The
simplest fair implementation is to restore the entire action, including
counters, so the operation is fully undone. If the team wants undo to
consume alarm instead, document that separately.

### Reset

-   Restore the original unsorted array.
-   Reset the current parent to the last non-leaf index.
-   Clear selected child and pending swap.
-   Reset alarm, score, penalties, steps, and hint-used state for the
    new attempt.
-   Rebuild both visualisations.

### Hint

A simple hint can highlight the correct larger child and display a short
explanation.

-   Mark `hintUsed = true`.
-   Apply the project-wide maximum star cap of 1.
-   Do not invent a separate point penalty.
-   The hint should not directly mutate the heap.

### Replay

Replay the recorded event sequence for the current attempt. It should
animate model events; it must not independently recompute or change the
heap.

------------------------------------------------------------------------

## 9. Recommended internal state and workflow

Keep the implementation straightforward. The exact names can follow the
existing codebase, but the following state is sufficient:

``` text
initialArray
currentArray
heapSize
currentParentIndex
currentSiftIndex
selectedChildIndex
pendingSwap
alarmSteps
alarmCapacity
optimalSteps
score
penalties
bonuses
steps
hintUsed
levelStatus
```

Suggested `levelStatus` string values (not an enum, because enums are
prohibited by the coding guardrails):

``` text
briefing
playing
selecting_child
confirming_swap
checking_subtree
level_complete
level_failed
```

Use the project's standard string-literal conventions and centralise
accepted values. Do not introduce an enum.

### Suggested gameplay loop

``` text
Load level data
Create MaxHeap from initial array
Calculate optimalSteps and alarmCapacity
Display tree and array

Set currentParentIndex to last non-leaf index

While level is playing:

    Highlight current parent and existing children

    If current parent is a leaf:
        Mark it complete
        Move to previous parent
        Continue

    Wait for player action

    If player selects a child:
        Check that it is a child of the current sift index
        Check that it is the largest existing child
        Show proposed swap

    If player confirms a valid swap:
        Record snapshot
        Ask MaxHeap model to perform sift-down swap
        Record and animate events
        Update tree and array
        Continue sift-down from the child's former index

    If player chooses No Swap Needed:
        Check parent against existing children
        If valid:
            Finish this sift-down
            Move to previous parent
        Otherwise:
            Add incorrect-action alarm
            Show message

    If player makes an invalid action:
        Add the applicable alarm
        Show message
        Keep model unchanged

    If alarmSteps reaches alarmCapacity:
        Set levelStatus to level_failed
        Stop accepting gameplay actions

If all parent indices are complete:
    Validate the complete Max Heap

    If valid:
        Set levelStatus to level_complete
        Calculate score and stars
```

This is a gameplay-level outline, not a requirement to put the whole
loop in one function. Keep UI input, game rules, MaxHeap operations, and
rendering in their existing layers.

------------------------------------------------------------------------

## 10. Data structure and GUI responsibilities

Follow the existing layered architecture and event-driven design.

### MaxHeap / data structure layer

Responsible for: - Array storage and heap size. -
Parent/left-child/right-child index calculations. - Max Heap
comparisons. - Bottom-up build and sift-down operations. - Swapping
array values. - Checking whether the array is a valid Max Heap.

It must not include SFML or draw anything.

### Game logic / level controller

Responsible for: - Loading Level 4.1 data. - Choosing the current parent
index. - Determining which player actions are allowed. - Validating the
player's child choice. - Applying alarm, score, penalty, and star
rules. - Deciding when the level succeeds or fails. - Coordinating undo,
reset, and hints.

### Event system

Use the existing agreed event system. Relevant Act 4 events from the
project brief include:

-   `HEAP_BUILD`
-   `HEAP_SWAP`
-   `HEAPIFY_START`
-   `HEAPIFY_DONE`
-   `HEAP_SIZE_CHANGE` (not normally needed for Level 4.1 unless the
    implementation changes heap size)
-   `DONE`

Use the exact agreed event payload conventions. A heap swap event should
contain the affected array indices and enough information for the
renderer/undo system.

### GUI / renderer

Responsible for: - Displaying the heap tree and array. - Highlighting
the current parent and children. - Showing selection and confirmation
controls. - Displaying instructions, feedback, alarm, score, and result
screens. - Animating events received from the event/animation system.

The GUI must not implement sift-down or directly change the MaxHeap
array.

### Synchronisation rule

The array is the source of truth. The tree view is derived from the
array using zero-based heap indexing:

``` text
parent(i) = (i - 1) / 2
left(i)   = 2 * i + 1
right(i)  = 2 * i + 2
```

For a swap of indices `i` and `j`: - The model swaps `array[i]` and
`array[j]`. - The event system records the swap. - The array renderer
animates cells `i` and `j`. - The tree renderer animates the nodes
corresponding to indices `i` and `j`.

Do not maintain a second independent tree data structure for the heap.

------------------------------------------------------------------------

## 11. Suggested UI states

### State 1 --- Waiting for child selection

-   Current parent and existing children are highlighted.
-   Child nodes are selectable.
-   Confirm Swap is disabled until a valid child is selected.
-   No Swap Needed is available.

### State 2 --- Child selected

-   Selected child is visibly distinct.
-   Proposed swap is shown.
-   Confirm Swap is enabled.
-   The player may cancel the selection.

### State 3 --- Swap animation

-   Disable further gameplay input during the animation.
-   Animate the swap in the array and tree.
-   Update the model through the event-driven flow.
-   After the animation, continue sift-down.

### State 4 --- Parent complete

-   Show a short confirmation.
-   Move the highlight to the next parent.
-   Do not require the player to click through unnecessary screens.

### State 5 --- Level result

On success, show: - Max Heap constructed. - Final array. - Alarm steps
and capacity. - Score and penalties. - Stars. - Continue button to Level
4.2.

On failure, show: - Alarm capacity reached. - Alarm steps and
capacity. - Score and penalties. - Retry button.

------------------------------------------------------------------------

## 12. Implementation decisions that should remain fixed

These choices keep the level simple and consistent:

1.  Use click-based interaction; no drag-and-drop.
2.  The game chooses and highlights the next parent.
3.  Process parents from the last non-leaf index down to index zero.
4.  The player chooses the larger existing child when a swap is
    required.
5.  The player confirms the proposed swap.
6.  Continue sift-down from the position where the parent value moved.
7.  Only allow swaps between the current sift position and one of its
    existing children.
8.  The MaxHeap array is the source of truth.
9.  Tree and array visualisations update from the same model events.
10. Incorrect Act 4 parent-child choices and invalid swaps add +1 alarm.
11. Do not apply the Act 3 wrong-rotation -50-point penalty to Level
    4.1.
12. Use the project-wide alarm-capacity and score formulas.
13. Use snapshots for simple undo.
14. Keep all MaxHeap algorithms out of GUI code.

------------------------------------------------------------------------

## 13. Acceptance tests for Level 4.1

An implementation agent should verify at least the following cases.

  -----------------------------------------------------------------------
  Test                    Input / action          Expected result
  ----------------------- ----------------------- -----------------------
  Correct build           `[4, 10, 3, 5, 1]`,     Final heap
                          follow the correct      `[10, 5, 3, 4, 1]`
                          choices                 

  Already-valid subtree   Parent `10`, children   Parent accepted; no
                          `5` and `1`; choose No  swap
                          Swap Needed             

  Wrong child             Parent `4`, children    Rejected; +1
                          `10` and `3`; select    incorrect-action alarm;
                          `3`                     array unchanged

  Correct child           Parent `4`, children    Swap accepted; both
                          `10` and `3`; select    views update
                          `10` and confirm        

  Continue sift-down      After root swap, `4`    Player can swap `4`
                          has child `5`           with `5`

  No-swap error           Parent has a larger     Rejected; +1 alarm;
                          child; choose No Swap   array unchanged
                          Needed                  

  Invalid target          Select a node that is   Rejected; +1 alarm;
                          not a child of current  array unchanged
                          parent                  

  Single child            Parent has only a left  Compare only the
                          child                   existing left child

  Leaf                    Current node has no     Automatically marked
                          children                complete

  Alarm failure           Alarm reaches capacity  Level fails; input
                                                  stops

  Undo                    Undo a completed swap   Prior array and agreed
                                                  level/counter state
                                                  restored

  Reset                   Reset after several     Original array and
                          actions                 initial state restored

  Hint                    Use hint and finish     Final star count capped
                                                  at 1

  Synchronisation         Perform any valid swap  Array cells and
                                                  corresponding tree
                                                  nodes show the same
                                                  values

  Final validation        Complete all parents    MaxHeap validator
                                                  returns true
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 14. Open points to confirm with the team

These are the only areas where the brief does not fully prescribe Level
4.1's exact behaviour:

1.  **Optimal-step convention:** This document recommends counting
    comparisons made by deterministic bottom-up heap construction.
    Confirm that the same definition will be used consistently across
    the game.
2.  **Incorrect-action accounting:** The brief says an incorrect Act 4
    parent-child comparison or invalid swap adds +1 alarm. Confirm
    whether a wrong child selection is treated as one incorrect
    comparison/action, so it is not double-counted.
3.  **Undo accounting:** This document recommends restoring alarm and
    score counters along with the array snapshot. Confirm whether undo
    should reverse counters or consume alarm.
4.  **Hint point cost:** The brief specifies a one-star maximum after
    using a hint, but does not specify an additional point deduction.
    This document does not add one.
5.  **Act 4 point penalties:** The brief does not assign the -50
    wrong-rotation penalty to Act 4. Keep it out of Level 4.1 unless the
    team formally changes the rule.

------------------------------------------------------------------------

## 15. Final player experience summary

The complete player experience should feel like this:

1.  Enter Act 4 and select Level 4.1.
2.  Read the mission and click Start Level.
3.  See the unsorted treasures in the heap tree and array.
4.  Follow the game's highlighted parent.
5.  Compare it with its existing children.
6.  Click No Swap Needed if the parent is already valid.
7.  Otherwise select the largest child and confirm the swap.
8.  Watch the tree and array update together.
9.  Continue sift-down until that subtree is valid.
10. Repeat for the remaining parents, moving bottom-up.
11. The game validates the complete Max Heap.
12. The player receives the result screen and unlocks Level 4.2 if
    successful.

**Core implementation goal:** The player makes the meaningful heap
decisions, while the game handles indexing, parent selection,
validation, event generation, animation, alarm accounting, and scoring.
This keeps the level educational, playable, and practical to implement
within the existing Vault Heist architecture.
