# Vault Heist --- Act 4: The Heap Fortress

## Complete Gameplay and Implementation Specification (Levels 4.1--4.5)

**Act:** 4 --- The Heap Fortress\
**Data structure:** Array-based Max Heap\
**Algorithms:** Bottom-up heap construction, heapify/sift-down, sift-up,
extract maximum, Heap Sort\
**Target:** C++ / SFML, using the project's layered, event-driven
architecture.

------------------------------------------------------------------------

# 1. Source basis and design boundaries

This guide is based on *Project Deliverable 1*, the
architecture/guardrails, and the project roadmap.

The deliverable defines five levels:

  -----------------------------------------------------------------------
  Level                   Name                    Required player goal
  ----------------------- ----------------------- -----------------------
  4.1                     Activate the Vault      Build a Max Heap using
                                                  bottom-up construction

  4.2                     Repair the Security     Repair a disturbed heap
                          Locks                   using heapify, sift-up
                                                  and sift-down

  4.3                     Extract the Greatest    Repeatedly extract the
                          Treasure                root, reduce heap size
                                                  and restore the heap

  4.4                     Race Against the Guards Complete multiple
                                                  extraction/heapify
                                                  iterations under an
                                                  alarm/operation limit

  4.5                     Final Vault Boss        Independently complete
                                                  Heap Sort on a new
                                                  array; final array must
                                                  be ascending
  -----------------------------------------------------------------------

The player uses two synchronised representations: 1. **Heap Tree View**
--- binary-tree representation of the heap. 2. **Array View** ---
underlying array, showing the active heap and sorted/extracted region.

The implementation flow must remain:

``` text
Player Input -> Level Logic -> MaxHeap Model -> Event Log
             -> Animation Controller -> Renderer
```

The MaxHeap model must not depend on SFML. The GUI must not implement
heap algorithms or mutate the heap array directly.

### Source requirements versus proposed details

The deliverable specifies each level's concepts, broad player
interaction, and success conditions. It does not prescribe every button,
screen message, or click sequence. The detailed controls and workflows
below are practical implementation proposals, not claims that every
micro-interaction appears verbatim in the source.

### Important penalty clarification

The project-wide rules specify: - Alarm capacity:
`ceil(1.5 × optimalSteps) + 2`. - Full alarm meter means failure. -
Score: `max(0, 1000 - 10 × alarmSteps - penalties + bonuses)`. - Wrong
rotation choice in Act 3: +2 alarm and -50 points. - Hint use: maximum
star rating capped at 1 star. - Incorrect parent-child comparison or
invalid swap in Act 4: +1 alarm.

**Do not apply Act 3's -50 wrong-rotation penalty to Act 4.** Act 4 has
no AVL rotation-choice mechanic. Under the current source rules, Act 4
errors add alarm; they do not automatically incur -50 points.

------------------------------------------------------------------------

# 2. Shared Act 4 gameplay screen

Use one screen layout across all five levels. Only the objective,
instructions, and enabled actions change.

``` text
+--------------------------------------------------------------+
| ACT 4: THE HEAP FORTRESS      LEVEL 4.x      OBJECTIVE        |
+-------------------------------+------------------------------+
|                               |                              |
|        HEAP TREE VIEW         |          ARRAY VIEW          |
|          [root]               | [0] [1] [2] [3] [4] ...      |
|          /    \               |                              |
|        [ ]    [ ]             | Active Heap | Sorted Region  |
|       /  \                    |                              |
|      [ ][ ]                   |                              |
+-------------------------------+------------------------------+
| Instruction / Current operation                              |
+--------------------------------------------------------------+
| Alarm: current / capacity | Score | Steps | Penalties         |
+--------------------------------------------------------------+
| Available actions | Undo | Replay | Reset | Hint | Speed      |
+---------------------------------------------------------------+
| Event Log                                                     |
+--------------------------------------------------------------+
```

Always display: - Current act, level, and objective. - Contextual
instruction/current operation. - Alarm steps and capacity. - Score,
steps, and penalties. - Heap Tree View and Array View. - Active heap
size. - Sorted region when extraction/sorting begins. - Event Log. -
Undo, Replay, Reset, Hint, and animation-speed controls.

**Accessibility:** Never use colour as the only signal. Label
`ACTIVE HEAP`, `SORTED`, `CURRENT PARENT`, `SELECTED CHILD`, and
`EXTRACTED MAXIMUM`. Use labels, outlines, icons, or patterns in
addition to colour.

## 2.1 Array/tree synchronisation

The MaxHeap is array-based. The tree view is derived from array indices;
do not create a second independent tree model.

For zero-based index `i`:

``` text
parent(i) = floor((i - 1) / 2)
left(i)   = 2 * i + 1
right(i)  = 2 * i + 2
```

For every `HEAP_SWAP(indexA, indexB, reason)` event: - The model swaps
the two array values. - The array view animates those cells. - The tree
view animates the nodes represented by those indices. - Both views
finish with identical values.

During extraction and sorting: - Only indices below `heapSize` belong to
the active heap. - Indices at `heapSize` and above belong to the sorted
region. - Sorted elements remain visible but must be labelled as sorted
and excluded from later heap operations.

## 2.2 Shared toolbar

-   **Undo:** restore the previous action snapshot.
-   **Replay:** replay the recorded event log.
-   **Reset Level:** restore the original seeded input and counters.
-   **Hint:** reveal a contextual hint; using it caps the maximum star
    rating at 1.
-   **Speed slider:** changes animation speed, not algorithm results.
-   **Pause/step:** use the project's general keyboard controls where
    implemented.

For the first implementation, use snapshots for undo. The deliverable
explicitly permits snapshots as the simpler initial approach before
inverse-event undo is implemented.

------------------------------------------------------------------------

# 3. Shared alarm, steps, penalties, score, and stars

Use the same ScoreSystem for every Act 4 level. Do not create five
separate scoring implementations.

## 3.1 Alarm capacity

``` text
alarmCapacity = ceil(1.5 * optimalSteps) + 2
```

`optimalSteps` must be computed by the model, not typed manually into
level data. The level data supplies the seeded input and configuration;
the model calculates the optimal count using the project's agreed
operation-counting convention.

When:

``` text
alarmSteps >= alarmCapacity
```

the level immediately enters `level_failed`. Stop accepting gameplay
actions and show the failure/result screen. Retry restores the original
input, heap size, operation state, alarm, score, penalties, bonuses, and
hint-used state.

## 3.2 What adds alarm in Act 4?

The source says a comparison costs one alarm step, and Act 4 incorrect
parent-child comparisons or invalid swaps add +1 alarm. The Act 4 GUI
description also says incorrect operations and unnecessary steps
increase the alarm.

Use this consistent policy:

  -----------------------------------------------------------------------
  Action                                                  Alarm treatment
  ------------------------------ ----------------------------------------
  Required parent-child                                                +1
  comparison                     

  Unnecessary                                 +1 for the unnecessary step
  comparison/operation           

  Incorrect parent-child choice                 +1 incorrect-action alarm

  Invalid swap attempt                                                 +1

  Invalid extraction attempt                                           +1

  Correct swap                       No extra error alarm; count required
                                                              comparisons

  Correct extraction                Count its algorithmic steps using the
                                                        shared convention

  Automatic                                                             0
  highlighting/animation         

  Automatic transition to next                                          0
  parent                         

  Briefing/result screen                                                0

  Hint                           No invented alarm cost; star cap applies

  Undo/Reset                              0 by default; confirm with team
  -----------------------------------------------------------------------

**Avoid double-counting.** A single incorrect action must update
ScoreSystem once with an explicit alarm cost. Do not charge the same
comparison in both the model and GUI.

## 3.3 `optimalSteps` convention

The deliverable says optimal steps are computed by the model, but does
not completely define one universal count convention for all heap
interactions.

Recommended convention: - Count algorithmic comparisons/operations that
the player is expected to perform. - Do not count animations,
highlights, screen transitions, or rendering. - Calculate optimal steps
by running the deterministic model procedure on a copy of the level's
initial input. - Use the same categories of steps in gameplay and in the
optimal-step calculation.

The team should confirm the precise operation-counting convention once
and use it for all five levels. The GUI must never guess the optimal
count.

## 3.4 Score formula

``` text
score = 1000 - (10 * alarmSteps) - penalties + bonuses
score = max(0, score)
```

For the current Act 4 rules: - `alarmSteps` is the attempt's accumulated
alarm. - `penalties` includes only point deductions explicitly specified
by the project or later approved by the team. - Do not add the Act 3 -50
rotation penalty to Act 4. - `bonuses` remain zero unless an explicit
level bonus is approved.

Example:

``` text
alarmSteps = 7
penalties  = 0
bonuses    = 0

score = 1000 - (10 * 7) - 0 + 0
      = 930
```

## 3.5 Star calculation

  Result                                                      Stars
  ------------------------------------------------------ ----------
  Steps `<= optimalSteps` and no penalties                        3
  Steps `<= 1.25 * optimalSteps` and at most 1 penalty            2
  Level completed within alarm capacity                           1
  Alarm reaches capacity                                   0; retry

If a hint was used:

``` text
finalStars = min(calculatedStars, 1)
```

Failed attempts receive zero stars. The definition of `steps` and
`penalties` must match the shared ScoreSystem convention.

## 3.6 Undo and counters

Recommended initial implementation: take a complete snapshot before each
accepted player action. Undo restores: - Array values and active heap
size. - Current operation and indices. - Selected element/pending
action. - Alarm, score, penalties, bonuses, and steps. - Sorted-region
state. - Event-log position.

This makes Undo a true reversal. If the team wants Undo to consume alarm
or preserve penalties, define that as one shared rule; do not vary it
between levels.

------------------------------------------------------------------------

# 4. Level 4.1 --- Activate the Vault

## 4.1 Objective

**Mission:** Organise the treasures.\
**Concepts:** Complete binary tree, Max Heap property, bottom-up heap
construction.\
**Source-defined success:** Every parent is greater than or equal to its
existing children.

The player receives an unsorted array. The game guides bottom-up
construction. The player decides whether a swap is required and selects
the larger child.

## 4.2 Entry workflow

1.  Player opens Act 4 --- The Heap Fortress.
2.  Player clicks **Level 4.1 --- Activate the Vault**.
3.  Briefing displays:
    -   Mission: Organise the treasures.
    -   Objective: Build a Max Heap.
    -   Instruction: Follow the highlighted parent. Compare it with its
        children. If a child is larger, select the largest child and
        confirm the swap.
4.  Player clicks **Start Level**.
5.  The game loads a seeded array and displays it in both views.

Worked example:

``` text
[4, 10, 3, 5, 1]
```

## 4.3 Player workflow

### Step 1 --- Game highlights the first parent

For `n` elements:

``` text
lastNonLeaf = floor(n / 2) - 1
```

For five elements, the last non-leaf index is `1`. The game highlights
index 1 (value `10`) and its children (`5`, `1`). The player does not
calculate or select the index.

Instruction: **Compare the highlighted parent with its children.**

### Step 2 --- Player decides whether a swap is needed

Parent `10` is greater than both children `5` and `1`.

Player clicks **No Swap Needed**. The game validates the relationship,
accepts the action, marks the subtree complete, and automatically moves
to index 0.

### Step 3 --- Player repairs the root

The game highlights root `4` and children `10` and `3`.

Instruction: **Select the larger child. If it is greater than the
parent, confirm the swap.**

Player clicks node `10`. The interface previews:

``` text
Proposed swap: 4 <-> 10
[Confirm Swap] [Cancel]
```

Player clicks **Confirm Swap**. The game validates that the selected
node is a child of the current parent, is the largest existing child,
and is greater than the parent.

The model swaps values:

``` text
[10, 4, 3, 5, 1]
```

Both views animate the same swap.

### Step 4 --- Continue sift-down

Value `4` moved to index 1. The game highlights children `5` and `1`.
Player selects `5` and confirms.

Array becomes:

``` text
[10, 5, 3, 4, 1]
```

Value `4` is now greater than its existing child `1`, so this sift-down
path is complete.

### Step 5 --- Complete construction

No more parent indices remain. The model validates the complete heap.

``` text
          10
        /    \
       5      3
      / \
     4   1

Final array: [10, 5, 3, 4, 1]
```

Success screen shows the final heap, alarm steps/capacity, score,
penalties, stars, and **Continue to Level 4.2**.

## 4.4 Incorrect actions

  -----------------------------------------------------------------------
  Player action                       Game response
  ----------------------------------- -----------------------------------
  Selects smaller child when another  Reject; +1 alarm; explain "Choose
  child is larger                     the larger child"; no data change

  Selects a non-child node            Reject; +1 alarm; no data change

  Attempts swap when parent is        Reject; +1 alarm; explain no swap
  already valid                       is needed

  Clicks No Swap Needed when a child  Reject; +1 alarm
  is larger                           

  Clicks a leaf                       Automatically mark complete

  Parent has only one child           Compare only the existing child
  -----------------------------------------------------------------------

No -50 penalty applies.

## 4.5 Implementation workflow

``` text
Load initial array
Compute optimalSteps and alarmCapacity
currentParent = floor(n / 2) - 1

While currentParent >= 0:
    currentSiftIndex = currentParent

    While currentSiftIndex has a child:
        Highlight currentSiftIndex and existing children
        Wait for player choice

        If No Swap Needed:
            Validate parent >= all existing children
            If valid: finish this sift path
            Else: add +1 alarm and keep state

        If child selected:
            Validate it is the largest existing child
            If invalid: add +1 alarm and keep state
            If valid: show confirmation

        If swap confirmed:
            Snapshot state
            Model performs one sift-down swap
            Emit HEAP_SWAP
            Animate array and tree
            currentSiftIndex = selectedChildIndex

        If alarm reaches capacity:
            Fail level and stop input

    currentParent = currentParent - 1

Validate Max Heap
Complete if valid
```

## 4.6 Implementation notes and tests

-   Use clicks, not drag-and-drop.
-   Game chooses the parent; player chooses the child.
-   Only the current parent and its existing children are selectable.
-   The array is the source of truth.
-   Use a one-step sift-down interaction so the player makes each
    meaningful choice.
-   Test valid construction, wrong child, no-swap case, one-child/leaf
    cases, synchronisation, and final `isMaxHeap() == true`.

------------------------------------------------------------------------

# 5. Level 4.2 --- Repair the Security Locks

## 5.1 Objective

**Mission:** Restore the mechanism.\
**Concepts:** Heapify, sift-up, sift-down.\
**Source-defined success:** Repair a deliberately disturbed heap until
the heap property is restored.

The deliverable explicitly names both sift-up and sift-down. A practical
design is to use two short repair rounds so the player sees both
directions. The exact disturbed input is not specified by the source;
the scenarios below are proposals.

## 5.2 Entry workflow

1.  Player selects **Level 4.2 --- Repair the Security Locks**.
2.  Briefing explains that a security fault has disturbed heap
    relationships.
3.  Player clicks **Start Level**.
4.  Game displays the disturbed heap and array.
5.  Invalid parent-child relationships are indicated and the current
    repair task is highlighted.

Use seeded, reproducible scenarios. Do not generate an arbitrary invalid
structure that makes the expected repair unclear.

## 5.3 Round A --- Repair downward (sift-down)

Example:

``` text
[4, 10, 3, 5, 1]
```

1.  Game highlights root `4` and children `10`, `3`.
2.  Instruction: **Choose the largest child if it is greater than the
    parent.**
3.  Player selects `10`, previews the swap, and confirms.
4.  Model swaps and emits `HEAP_SWAP`.
5.  Game follows the moved value `4` downward.
6.  It highlights children `5` and `1`.
7.  Player selects `5` and confirms.
8.  The resulting array is `[10, 5, 3, 4, 1]`.
9.  The affected path is now valid; mark Round A complete.

## 5.4 Round B --- Repair upward (sift-up)

To make sift-up easy to understand, present a valid heap and then place
a new treasure at the next available leaf.

Before insertion:

``` text
[10, 5, 3, 4, 1]
```

Insert `12` at index 5:

``` text
[10, 5, 3, 4, 1, 12]
```

The game highlights new leaf `12` and its parent `3`.

Player workflow: 1. Instruction: **The new treasure is larger than its
parent. Move it upward.** 2. Game highlights the only possible
parent-child pair. 3. Player clicks **Confirm Move Up**. 4. Model swaps
the value with its parent and emits `HEAP_SWAP`. 5. Continue comparing
with the new parent. 6. Stop when the parent is greater/equal or the
value reaches the root.

Result:

``` text
[12, 5, 10, 4, 1, 3]
```

This is a valid Max Heap.

**Implementation simplification:** During sift-up, there is only one
parent candidate. Do not ask the player to choose among multiple nodes.
Highlight the parent and ask for confirmation.

## 5.5 Final validation

1.  Game runs `isMaxHeap()` on the active heap.
2.  If valid, show **Security Locks Repaired**.
3.  Display final heap, operations performed, alarm, score, and stars.
4.  Player clicks **Continue to Level 4.3**.

## 5.6 Incorrect actions

-   Select smaller child during sift-down: reject, +1 alarm.
-   Confirm a swap that does not repair the current violation: reject,
    +1 alarm.
-   Try moving a value up when it is not greater than its parent:
    reject, +1 alarm if classified as an invalid action.
-   Skip a required repair: reject, +1 alarm.
-   Select a node outside the current repair relationship: reject, +1
    alarm.

No -50 penalty applies.

## 5.7 Implementation workflow

``` text
Load seeded disturbed-heap scenario
Display array and derived tree

Round A: sift-down
    Start at configured disturbed index
    Highlight parent and existing children
    Player selects largest child if larger than parent
    Confirm one swap
    Emit HEAP_SWAP and animate
    Continue from moved value's new index
    Stop when current path is valid

Round B: sift-up
    Insert/configure new value at next leaf
    Highlight new value and its parent
    If child > parent:
        Ask player to confirm upward swap
        Emit HEAP_SWAP and animate
        Continue from new index
    Stop at root or when parent >= child

Run isMaxHeap()
Complete only if validator succeeds
```

## 5.8 Acceptance tests

-   Downward violation is repaired by repeatedly selecting the largest
    child.
-   Inserted value larger than parent moves upward.
-   Sift-up stops at root or when parent is greater/equal.
-   Sift-down stops at leaf or when parent is greater/equal to children.
-   `isMaxHeap()` is false before repair and true after repair.
-   Tree and array remain synchronised.

------------------------------------------------------------------------

# 6. Level 4.3 --- Extract the Greatest Treasure

## 6.1 Objective

**Mission:** Remove the highest-value treasure.\
**Concepts:** Extract-max, root/last swap, active heap-size reduction,
heapify.\
**Source-defined success:** Repeatedly extract the root and restore the
remaining heap; each extracted maximum enters the sorted region.

Start with a valid Max Heap. Do not ask the player to rebuild it from
scratch.

## 6.2 Entry workflow

1.  Player selects **Level 4.3 --- Extract the Greatest Treasure**.
2.  Briefing explains root is maximum, root swaps with last active
    element, heap size shrinks, and remaining heap is repaired.
3.  Player clicks **Start Level**.
4.  Game shows a valid Max Heap, array, active heap-size indicator, and
    empty sorted region.

Example:

``` text
Array: [10, 5, 3, 4, 1]
heapSize = 5
```

## 6.3 Player workflow --- one extraction

### Step 1: Identify maximum

Root `10` is highlighted and labelled **Current Maximum**. Player clicks
**Extract Maximum**.

### Step 2: Root/last swap

Game highlights root index 0 and last active index `heapSize - 1`.
Instruction:

> Confirm the root/last-element swap.

Player clicks **Confirm Extraction**. Model swaps index 0 with index 4:

``` text
[1, 5, 3, 4, 10]
```

Emit `HEAP_SWAP` with extraction reason.

### Step 3: Shrink active heap

Game reduces `heapSize` from 5 to 4:

``` text
Active heap: [1, 5, 3, 4]
Sorted region: [10]
```

Emit `EXTRACT_MAX`, `HEAP_SIZE_CHANGE`, and `MARK_SORTED`. The extracted
maximum is visually separated and excluded from future heap operations.

### Step 4: Repair active heap

Active root `1` has children `5` and `3`.

1.  Player selects `5` and confirms the swap.
2.  Array becomes `[5, 1, 3, 4, 10]`.
3.  Game follows value `1` to its new position and highlights child `4`.
4.  Player selects `4` and confirms.
5.  Array becomes `[5, 4, 3, 1, 10]`.
6.  Active heap is valid; emit `HEAPIFY_DONE`.

### Step 5: Repeat

Player repeats: 1. Extract root. 2. Swap root with last active element.
3. Reduce heap size. 4. Mark extracted maximum sorted. 5. Sift down the
new root until the active heap is valid.

Continue until one active element remains. Mark that final element
sorted.

## 6.4 Completion

The full array should be in ascending order. Show: - Sorted array. -
Extracted maximum sequence. - Number of extractions. - Alarm
steps/capacity. - Score, penalties, stars. - **Continue to Level 4.4**.

## 6.5 Incorrect actions

  -----------------------------------------------------------------------
  Player action                       Response
  ----------------------------------- -----------------------------------
  Extract when active heap is empty   Reject; +1 alarm

  Select non-root as maximum          Reject; +1 alarm; root contains
                                      maximum

  Select wrong last active index      Prevent in UI or reject; +1 alarm

  Extract from sorted region          Reject; +1 alarm

  Change heap size directly           Prevent; controller/model owns
                                      heap-size change

  Choose smaller child during repair  Reject; +1 alarm

  Perform unnecessary heapify action  Count as unnecessary operation
                                      under shared policy
  -----------------------------------------------------------------------

No -50 penalty applies.

## 6.6 Implementation workflow

``` text
Load valid Max Heap
heapSize = array.size
sorted region = empty

While heapSize > 1:
    Highlight root as maximum
    Wait for Extract Maximum

    Validate extraction
    Snapshot state

    lastIndex = heapSize - 1
    Swap array[0] and array[lastIndex]
    Emit HEAP_SWAP

    extractedValue = array[lastIndex]
    heapSize = heapSize - 1
    Emit EXTRACT_MAX
    Emit HEAP_SIZE_CHANGE
    Emit MARK_SORTED

    Highlight active heap
    Player-guided sift-down from index 0:
        Compare parent with existing children
        If a child is larger:
            Player selects largest child
            Confirm swap
            Model swaps and emits HEAP_SWAP
            Continue from child index
        Else:
            Sift-down complete

    Emit HEAPIFY_DONE
    If alarm reaches capacity:
        Fail and stop

Mark final remaining element sorted
Validate ascending order
Complete level
```

## 6.7 Implementation notes and tests

-   `heapSize` is essential.
-   The root is maximum only within the active heap.
-   Sorted elements stay in the array but are not part of the tree's
    active heap.
-   Do not shift array elements after extraction; Heap Sort uses
    root/last swap and a shrinking active boundary.
-   Test maximum correctness, root/last swap, one-step heap-size
    reduction, sorted marking, valid heapify, and ascending final
    output.

------------------------------------------------------------------------

# 7. Level 4.4 --- Race Against the Guards

## 7.1 Objective

**Mission:** Beat the alarm.\
**Concepts:** Heap Sort steps and complexity.\
**Source-defined success:** Complete multiple extraction/heapify
iterations under an alarm/operation limit.

This level reuses the extraction workflow from Level 4.3. The challenge
is pressure and efficiency, not a new algorithm.

## 7.2 Entry workflow

1.  Player selects **Level 4.4 --- Race Against the Guards**.
2.  Briefing explains the guards are approaching and the player must
    complete multiple extraction/heapify iterations before alarm
    capacity is reached.
3.  Player clicks **Start Level**.
4.  Game loads a seeded valid Max Heap, or a configured input that is
    built into a heap as part of the level.
5.  Show active heap, sorted region, heap size, alarm meter, and
    progress.

Illustrative HUD:

``` text
Extractions completed: 2 / 5
Active heap size: 3
Sorted elements: 3
Alarm: 5 / 10
```

These numbers are examples, not fixed project values.

## 7.3 Player workflow

Each round uses the same controls as Level 4.3:

1.  Click **Extract Maximum**.
2.  Confirm root/last-active-element swap.
3.  Watch active heap size decrease.
4.  Confirm extracted maximum is moved to sorted region.
5.  Repair active heap by selecting the largest child where necessary.
6.  Continue until heapify completes.
7.  Proceed to next extraction round.

The HUD updates after every completed extraction and heapify.

## 7.4 Difficulty design

Use a controlled seeded input that requires multiple meaningful
extraction and heapify iterations. The Act 4 generator should exercise
the specified input categories: - Random. - Reverse-sorted. -
Already-heap. - Duplicate-heavy.

Choose array size and round count that are more demanding than Level 4.3
but completable within the computed alarm capacity. Do not make
difficulty depend on hidden rules or unpredictable array changes.

## 7.5 Operation-pressure feature

Use the alarm meter as the pressure mechanic. Unnecessary
comparisons/operations and invalid actions increase alarm. Do not add a
real-time countdown unless the team explicitly chooses to; the
deliverable specifies an alarm/operation limit and the existing Alarm
Meter already provides it.

## 7.6 Incorrect actions

Reuse Level 4.3 validation: - Wrong root/maximum choice: +1 alarm. -
Invalid root/last swap: +1 alarm. - Invalid extraction: +1 alarm. -
Wrong child during heapify: +1 alarm. - Unnecessary operation: +1 alarm
under shared policy. - Automatic animation: 0 alarm.

At alarm capacity: - Stop the current sequence. - Do not start another
extraction. - Show failure with current active heap and sorted region. -
Offer Retry.

No -50 penalty applies.

## 7.7 Completion

Recommended condition: - All extraction rounds configured for this level
are complete. - Every extraction has a corresponding heapify
completion. - Active heap and sorted region are consistent. - Alarm
capacity has not been reached.

The source calls for multiple iterations under pressure; the final
independent full-sort task is explicitly the focus of Level 4.5. Level
4.4 can therefore use a configured round target without duplicating the
entire boss challenge.

## 7.8 Implementation workflow

``` text
Load seeded Level 4.4 configuration
Build/load configured valid Max Heap
Compute optimalSteps and alarmCapacity
Set extractionRoundTarget from level data

While completedExtractions < extractionRoundTarget:
    Wait for Extract Maximum
    Validate request
    Swap root with last active element
    Reduce heapSize
    Mark extracted value sorted
    Animate events

    Player-guided sift-down:
        Compare parent with existing children
        Player chooses largest child when needed
        Confirm valid swap
        Continue until valid

    Mark heapify round complete
    Update progress and HUD

    If alarmSteps >= alarmCapacity:
        Fail and stop

Validate active heap and sorted-region consistency
Complete if configured rounds finish within capacity
```

## 7.9 Complexity learning moment

At the end, display a short explanation: - Bottom-up heap construction:
`O(n)`. - Each extraction/heapify: `O(log n)`. - Full Heap Sort:
`O(n log n)`.

This is an educational summary, not a new quiz unless the team adds one
later.

## 7.10 Acceptance tests

-   Multiple extraction/heapify rounds occur.
-   Alarm/operation limit is visible.
-   Invalid actions increase alarm.
-   Each extraction reduces heap size exactly once.
-   Extracted maxima enter sorted region.
-   Heapify restores active heap before next extraction.
-   Alarm capacity causes failure.
-   Successful completion requires configured rounds.
-   Event log and both views remain synchronised.

------------------------------------------------------------------------

# 8. Level 4.5 --- Final Vault Boss

## 8.1 Objective

**Mission:** Unlock the final vault.\
**Concept:** Complete Heap Sort workflow.\
**Source-defined success:** Sort a new array independently in ascending
order; final door opens only when the array is correctly sorted.

This is the final assessment. The player is responsible for the complete
process. "Independently" means the game does not automatically choose
every operation or child for the player; basic instructions, controls,
and validation remain available.

## 8.2 Entry workflow

1.  Player selects **Level 4.5 --- Final Vault Boss**.
2.  Briefing explains that a new array is provided and the player must
    build a Max Heap and complete Heap Sort.
3.  Player clicks **Start Final Challenge**.
4.  Game loads a new seeded array, different from tutorial examples.
5.  Tree and array views appear; sorted region is initially empty.

## 8.3 Phase A --- Build Max Heap

1.  Player clicks **Build Max Heap** or begins the manual build action.
2.  Game displays the last non-leaf parent.
3.  Player processes parents bottom-up, selecting the largest child and
    confirming required swaps.
4.  If a parent is already valid, player confirms **No Swap Needed**.
5.  Continue until every parent has been processed.
6.  Model validates `isMaxHeap()`.

Reuse Level 4.1's construction component. Do not create a second
build-heap implementation.

## 8.4 Phase B --- Complete Heap Sort

After building the heap:

1.  Player clicks **Extract Maximum**.
2.  Root is highlighted as maximum.
3.  Player confirms root/last-active-element swap.
4.  Game reduces active heap size.
5.  Extracted maximum moves to sorted region.
6.  Player repairs active heap by choosing largest child and confirming
    swaps.
7.  Player repeats until one active element remains.
8.  Game marks the final remaining element sorted.

Do not automatically perform all remaining extraction steps. The player
must operate the complete workflow.

## 8.5 Phase C --- Final validation

Check: - Full array is sorted in ascending order. - Active heap size is
consistent with completion. - Extracted values are in the correct sorted
region. - Event log contains the expected build, swap, extraction,
heapify, and sorted-marking sequence. - Model validator passes.

If valid: - Emit `HEAP_SORT_COMPLETE`. - Display final array. - Unlock
final vault. - Award **Final Vault Unlocked** achievement.

If invalid: - Keep vault locked. - Highlight the first detected issue. -
Let the player correct the state if the level permits, or reset/retry
according to configuration.

## 8.6 Incorrect actions

-   Incorrect parent-child comparison: +1 alarm.
-   Invalid swap: +1 alarm.
-   Invalid extraction: +1 alarm.
-   Unnecessary operation: +1 alarm under shared policy.
-   Attempt to mark an active/unsorted value as sorted: reject; +1 alarm
    if classified as invalid operation.
-   Attempt to extract from sorted region: reject; +1 alarm.

No -50 penalty applies.

## 8.7 Implementation workflow

``` text
Load new seeded array
Compute optimalSteps and alarmCapacity
Display tree and array

Phase A: Build Max Heap
    Process parents bottom-up
    Player chooses required child swaps
    Validate each action
    Continue sift-down
    Verify isMaxHeap()

Phase B: Heap Sort
    While heapSize > 1:
        Player requests Extract Maximum
        Validate request
        Swap root with last active element
        Reduce heapSize
        Mark extracted maximum sorted
        Player guides sift-down on active heap
        Validate active heap
        Stop if alarm reaches capacity

    Mark final remaining element sorted

Phase C: Final validation
    Check ascending order
    Check heapSize/sorted-region consistency
    Check model validator

    If valid:
        Emit HEAP_SORT_COMPLETE
        Unlock final vault
        Award Final Vault Unlocked
    Else:
        Keep vault locked and show feedback
```

## 8.8 Completion screen

Show: - Final sorted array. - Final score. - Steps versus optimal
steps. - Alarm steps and capacity. - Penalties. - Star rating. - **Final
Vault Unlocked** achievement badge.

The deliverable explicitly requires the final score, steps, penalties,
sorted array, and achievement badge after Act 4 is completed.

## 8.9 Acceptance tests

-   A new array is used.
-   Player can build a Max Heap.
-   Player can perform all root/last swaps.
-   Active heap size decreases correctly.
-   Extracted maxima enter sorted region.
-   Heapify completes after every extraction.
-   Final array is ascending.
-   `HEAP_SORT_COMPLETE` is emitted only after successful validation.
-   Vault remains locked if validation fails.
-   Completion screen displays required results and badge.

------------------------------------------------------------------------

# 9. Shared event model

Use the event names and payload conventions from the project:

  -----------------------------------------------------------------------
  Event                               Purpose
  ----------------------------------- -----------------------------------
  `HEAP_BUILD`                        Start/build operation; include
                                      array snapshot or relevant start
                                      information

  `HEAP_SWAP`                         Swap two indices; include `indexA`,
                                      `indexB`, and reason

  `HEAPIFY_START`                     Begin heapify; include `startIndex`
                                      and `heapSize`

  `HEAPIFY_DONE`                      Heapify completed

  `EXTRACT_MAX`                       Record extracted value, root index,
                                      and heap size

  `MARK_SORTED`                       Mark an array index/value as sorted

  `HEAP_SIZE_CHANGE`                  Record old and new active heap
                                      sizes

  `HEAP_SORT_COMPLETE`                Final sorted array; signals
                                      successful full Heap Sort

  `DONE`                              End a batch/operation summary
  -----------------------------------------------------------------------

Typical extraction sequence:

``` text
HEAP_SWAP
EXTRACT_MAX
HEAP_SIZE_CHANGE
MARK_SORTED
HEAPIFY_START
HEAP_SWAP
HEAPIFY_DONE
DONE
```

Typical build sequence:

``` text
HEAP_BUILD
HEAPIFY_START
HEAP_SWAP
HEAPIFY_DONE
DONE
```

Use the project's agreed payload structure. Do not create a parallel
event system for one level.

------------------------------------------------------------------------

# 10. Recommended internal state

Names may follow the existing codebase, but the LevelManager needs
equivalent state:

``` text
initialArray
currentArray
heapSize
currentParentIndex
currentSiftIndex
selectedChildIndex
pendingOperation
sortedStartIndex
completedExtractions
extractionRoundTarget
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

Suggested `levelStatus` values (use the project's string-state
convention; do not introduce enums if prohibited):

``` text
briefing
playing
selecting_child
confirming_swap
extracting
heapifying
animating
level_complete
level_failed
```

Suggested `pendingOperation` values:

``` text
build_heap
sift_down
sift_up
extract_max
heap_sort
```

Centralise accepted state strings rather than scattering literals.

------------------------------------------------------------------------

# 11. Architecture responsibilities

## 11.1 MaxHeap model

Responsible for: - Array storage and active heap size. - Parent/child
index calculations. - `buildMaxHeap`. - `siftUp`. - `siftDown` /
`heapify`. - `extractMax`. - `insert`. - `heapSort`. - `isMaxHeap`. -
Returning event data.

Must not include SFML, draw nodes, display dialogs, or directly update
alarm/score.

## 11.2 Level logic / LevelManager

Responsible for: - Loading seeded level data. - Selecting current
objective and operation. - Tracking current parent, sift index, active
heap size, and sorted region. - Deciding allowed player actions. -
Validating player choices. - Calling MaxHeap operations. - Applying
alarm and score rules through ScoreSystem. - Checking win/fail
conditions. - Coordinating hint, undo, reset, and replay.

## 11.3 ScoreSystem

Responsible for: - Alarm steps and capacity. - Optimal-step
comparison. - Score formula. - Penalty points and bonuses. - Star
calculation. - Hint star cap. - Result-screen data.

The GUI must not calculate score independently.

## 11.4 EventLog and AnimationController

Responsible for: - Recording model events. - Replaying event logs. -
Undo through inverse events or snapshots. - Animating swaps, heap-size
changes, extraction, and sorted marking. - Pause, step, and speed
control.

## 11.5 Renderer / UI

Responsible for: - Drawing heap tree and array. - Highlighting current
parent and children. - Showing active heap and sorted-region labels. -
Showing selected nodes and proposed swaps. - Drawing alarm, score,
steps, and penalties. - Displaying contextual instructions and
feedback. - Routing mouse/keyboard input to LevelManager.

The GUI must not implement heap algorithms or directly modify array
values.

------------------------------------------------------------------------

# 12. Level data and reproducibility

The deliverable requires Act 4 inputs to include controlled examples
from: - Random. - Reverse-sorted. - Already-heap. - Duplicate-heavy.

Every level should use a seed so the same level can be reproduced and
graded.

Store level-specific configuration in JSON rather than hardcoding each
level's gameplay into separate GUI logic. Configuration may include: -
Level ID/title. - Seed. - Initial array or generation parameters. -
Operation/round type. - Extraction-round target for Level 4.4. - Hint
text. - Difficulty/size parameters.

`optimalSteps` must be computed by the model, never typed manually.
Select input sizes so every required operation is exercised and the
level remains completable within its computed alarm capacity.

------------------------------------------------------------------------

# 13. Testing plan

## 13.1 Model unit tests

Test MaxHeap independently of SFML: - Empty and single-element heap. -
Duplicates and negative values. - Already-heap, reverse-sorted, and
random input. - Bottom-up build. - Sift-up and sift-down. - Extract
maximum. - Active heap-size changes. - Heap Sort against a trusted
sorted reference. - `isMaxHeap()` after build, insert, extraction, and
heapify.

## 13.2 Gameplay tests

  Level   Test                           Expected result
  ------- ------------------------------ ---------------------------------
  4.1     Build from unsorted array      Valid Max Heap
  4.1     Wrong child selection          Rejected; +1 alarm
  4.1     Already-valid parent           No Swap Needed accepted
  4.2     Downward violation             Sift-down repairs it
  4.2     New leaf greater than parent   Sift-up repairs it
  4.2     Final validation               `isMaxHeap()` true
  4.3     Extract maximum                Root value enters sorted region
  4.3     Heap-size reduction            Decreases by exactly one
  4.3     Heapify after extraction       Active region valid
  4.3     Repeated extraction            Sorted output ascending
  4.4     Multiple rounds                All configured rounds complete
  4.4     Alarm reaches capacity         Failure and input lock
  4.5     Full Heap Sort                 Final array ascending
  4.5     Invalid final order            Vault stays locked
  All     Undo                           State restored consistently
  All     Replay                         Event log replays correctly
  All     Reset                          Original seeded state restored
  All     Hint                           Maximum star rating capped at 1
  All     Tree/array swap                Both views remain synchronised

## 13.3 GUI tests

-   Every level can be completed, failed, and retried.
-   Active heap and sorted region are clearly labelled.
-   Colour is never the only indicator.
-   No stale node/cell positions after undo.
-   Input is disabled while a swap animation is running.
-   Tree and array remain synchronised throughout Heap Sort.
-   Event Log matches model events.
-   Alarm and score update exactly once per action.

------------------------------------------------------------------------

# 14. Player progression across Act 4

  -----------------------------------------------------------------------
  Level                   Player experience       New responsibility
  ----------------------- ----------------------- -----------------------
  4.1 --- Activate the    Follow highlighted      Understand Max Heap
  Vault                   parents and build a Max property and bottom-up
                          Heap                    construction

  4.2 --- Repair the      Repair a disturbed heap Understand sift-down
  Security Locks          in both directions      and sift-up

  4.3 --- Extract the     Repeatedly remove root  Understand extract-max
  Greatest Treasure       and repair remaining    and active heap size
                          heap                    

  4.4 --- Race Against    Repeat extraction and   Perform workflow
  the Guards              heapify under alarm     efficiently
                          pressure                

  4.5 --- Final Vault     Independently build a   Demonstrate the full
  Boss                    heap and complete Heap  algorithm
                          Sort on a new array     
  -----------------------------------------------------------------------

The same controls, event system, heap model, ScoreSystem, and
visualisations should be reused throughout Act 4. Difficulty should come
from the algorithmic task and input configuration, not unrelated
mechanics or changing controls.

------------------------------------------------------------------------

# 15. Decisions to confirm before implementation

1.  **Level 4.2 scenario:** The source requires sift-up and sift-down
    but does not specify the exact disturbed input. This guide proposes
    a downward-repair round and an insertion-style upward-repair round.
2.  **Level 4.3 interaction:** Confirm whether the player confirms each
    root/last swap and each heapify child choice, or whether extraction
    is one button followed by guided heapify. Recommended: one Extract
    Maximum button plus player-guided heapify.
3.  **Level 4.4 round target:** Choose a configured number of extraction
    rounds and input size. The source does not specify a fixed count.
4.  **Optimal-step convention:** Agree on exactly which
    comparisons/operations count as a step. The model computes the
    optimal value.
5.  **Incorrect-action accounting:** Confirm each wrong child selection
    or invalid operation adds +1 alarm exactly once, without
    double-counting.
6.  **Undo counters:** Confirm whether Undo restores alarm and score.
    This guide recommends restoring the complete snapshot.
7.  **Hint point cost:** The source specifies a one-star cap but no
    extra point deduction. Do not invent one.
8.  **Act 4 point penalties:** Keep the -50 wrong-rotation penalty in
    Act 3. Do not add it to Act 4 unless the team formally changes the
    rules.

------------------------------------------------------------------------

# 16. Final instruction to the implementation agent

Implement Act 4 as one reusable MaxHeap gameplay system with five level
configurations.

-   Do not duplicate heap algorithms across levels.
-   Do not put data-structure logic in SFML or renderer classes.
-   Do not maintain separate tree and array models.
-   Do not allow arbitrary swaps.
-   Do not let sorted-region elements re-enter the active heap.
-   Do not apply the Act 3 -50 rotation penalty to Act 4.
-   Do not hardcode optimal-step counts.
-   Do not invent new mechanics when the required heap operations
    already provide the intended challenge.

**The player makes the meaningful decisions; the model performs and
validates algorithms; the event system describes what happened; and the
GUI visualises those events.**
