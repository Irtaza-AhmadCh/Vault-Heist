# Vault Heist: The Living Vault

# Act 1 --- The Blueprint Wing

## Complete Level Design, Implementation, Scoring, and Edge-Case Specification

**Document purpose:** This document is the implementation guide for Act
1 (Levels 1.1--1.5). It describes the learning objective, player flow,
game rules, data-structure logic, SFML interaction, scoring, failure
handling, and test cases.

**Status labels used in this document**

-   **\[PROJECT REQUIREMENT\]** --- explicitly stated in the project
    deliverable or SRS.
-   **\[CONFIRMED DESIGN\]** --- decided during the design discussion
    for this project.
-   **\[IMPLEMENTATION RECOMMENDATION\]** --- a practical rule proposed
    to make an underspecified requirement implementable. Keep it
    consistent across the codebase.
-   **\[OPEN DECISION\]** --- the project source does not settle this
    detail; confirm it with the team before treating it as final.

------------------------------------------------------------------------

# 1. Act 1 Overview

## 1.1 Purpose

Act 1 is named **The Blueprint Wing** and teaches the player how a
general binary tree works. The player explores a vault whose rooms are
represented by tree nodes. Unlike a Binary Search Tree, a general binary
tree has **no ordering rule**: a node's value does not determine whether
it belongs on the left or right.

Act 1 contains five levels:

  -----------------------------------------------------------------------
  Level                   Main concept            Player's task
  ----------------------- ----------------------- -----------------------
  1.1                     Preorder, inorder,      Visit rooms in the
                          postorder, level-order  requested traversal
                                                  order

  1.2                     Tree reconstruction     Rebuild a hidden tree
                                                  from traversal
                                                  sequences

  1.3                     Height and diameter     Calculate node heights
                                                  bottom-up, then report
                                                  the diameter

  1.4                     Lowest Common Ancestor  Find the common
                          and path                ancestor and trace the
                                                  path between two rooms

  1.5                     Tree inversion / mirror Swap left and right
                                                  children to create the
                                                  mirror tree
  -----------------------------------------------------------------------

**\[PROJECT REQUIREMENT\]** Act 1 uses a general binary tree with
arbitrary values and no ordering restriction. It must support all four
traversals, reconstruction from preorder + inorder and postorder +
inorder, height, diameter, LCA, paths, mirror/inversion, structural
comparison, and symmetry checking.

## 1.2 Shared rules for all Act 1 levels

1.  The tree is a general binary tree. Do not apply BST ordering rules.
2.  A node has a unique identity (`id`) for GUI selection and event
    tracking. Its displayed value is separate from its identity.
3.  A tree can be empty or contain one node at the data-structure level,
    even if campaign level generation normally creates larger trees.
4.  All tree algorithms must work independently of SFML.
5.  The GUI sends player actions to game logic. Game logic validates the
    action and calls the data structure. The GUI displays events and
    animations; it must not directly change pointers.
6.  The level's target answer must be calculated or stored by the
    model/level data, not guessed from the current screen.
7.  A failed attempt must not silently corrupt the tree. Invalid actions
    should be rejected or rolled back.
8.  A level's result screen shows the final score, alarm steps,
    penalties, and stars.
9.  The player can retry a failed level. A retry starts a fresh attempt
    with reset level counters and a restored initial tree.

## 1.3 Recommended shared node model

The project architecture describes a `TreeNode` containing an ID, value,
left pointer, right pointer, and optionally a parent pointer.

``` cpp
struct TreeNode
{
    int id;
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
};
```

-   `id`: stable unique identifier for selection, highlighting, events,
    and layout.
-   `value`: integer shown inside the room.
-   `left` and `right`: child links.
-   `parent`: recommended because it makes path construction and
    ancestor operations easier. If parent pointers are used, validate
    them after every structural change.

Do not use the node's displayed value as its GUI identity. This matters
especially if a future level contains repeated values or if values are
changed while the node remains the same logical room.

**Reconstruction levels require distinct values.** A preorder/inorder or
postorder/inorder pair is not generally sufficient to identify one
unique tree if values repeat. The level generator must therefore create
distinct values for Level 1.2.

## 1.4 Tree height and path-length convention

**\[IMPLEMENTATION RECOMMENDATION\]** Use the **edge-count convention**
throughout Act 1:

-   Empty tree height = `-1`.
-   Leaf node height = `0`.
-   A non-leaf node height = `1 + max(leftHeight, rightHeight)`.
-   Tree height = height of the root.
-   Path length = number of edges in the path.

This convention makes a leaf's height zero and makes the distance
between a node and itself zero. The project document explicitly says to
choose either edge-count or node-count height and document the choice;
it does not mandate one convention.

Example:

``` text
        A
       / \
      B   C
     /
    D
```

-   `height(D) = 0`
-   `height(B) = 1`
-   `height(C) = 0`
-   `height(A) = 2`
-   The path from D to C is D → B → A → C, which has 3 edges.

------------------------------------------------------------------------

# 2. Shared Alarm, Score, and Star System

## 2.1 Official scoring rules

The following rules come from the project specification.

### Alarm steps

-   Visiting a tree node costs **1 alarm step**.
-   Comparing a key against a node costs **1 alarm step**.
-   Incorrect actions may add extra alarm steps.
-   Every level has an alarm capacity:

``` text
alarmCapacity = ceil(1.5 * optimalSteps) + 2
```

-   When the alarm meter reaches its capacity, the level fails.

### Star conditions

  Result condition                                                        Stars
  ------------------------------------------------------------------ ----------
  Steps are at most optimal and there were no penalties                       3
  Steps are at most 1.25 × optimal and there was at most 1 penalty            2
  Level is completed before the alarm capacity is exhausted                   1
  Alarm capacity is exhausted                                          0; retry

### Score formula

``` text
score = max(0, 1000 - 10 * alarmSteps - numericPenalties + bonuses)
```

The formula starts from 1000 because that is the specified base score.
The project source does not provide a separate explanation for why 1000
was selected.

### Explicit penalties

  Action                                                     Official effect
  ---------------------------------------------------------- ----------------------------------
  Wrong traversal selection                                  +1 alarm
  Wrong rotation choice                                      +2 alarm and -50 points
  Use a hint                                                 Maximum star rating reduced by 1
  Invalid parent-child comparison or invalid swap in Act 4   +1 alarm

The wrong-rotation penalty belongs to the AVL rotation levels, not
Act 1. The Act 4 invalid-comparison/swap penalty also does not
automatically apply to Act 1.

## 2.2 Important distinction: alarm steps vs. numeric penalties

Do not mix these counters.

-   `alarmSteps` is the number used by the alarm meter and the
    `10 × alarmSteps` part of the score.
-   `numericPenalties` is a separate direct point deduction where the
    specification explicitly gives one, such as the -50 points for a
    wrong rotation.
-   A wrong traversal selection adds one alarm step. It does **not**
    also deduct another 50 points.
-   A hint reduces the maximum possible star rating. It is not specified
    as a fixed point deduction.

Suggested state:

``` cpp
struct LevelScore
{
    int alarmSteps;
    int alarmCapacity;
    int optimalSteps;
    int numericPenalties;
    int bonuses;
    int hintCount;
    int maxStars;
    int finalStars;
    int finalScore;
};
```

Initialize `maxStars` to 3 at the start of an attempt. Each hint reduces
it by one, but never below zero.

## 2.3 Star evaluation order

Use this order to avoid ambiguous results:

1.  If `alarmSteps >= alarmCapacity`, the attempt has failed: 0 stars.
2.  Otherwise calculate the normal star result:
    -   3 stars if `alarmSteps <= optimalSteps` and there are no
        penalties.
    -   2 stars if `alarmSteps <= 1.25 * optimalSteps` and the attempt
        has at most one penalty.
    -   1 star if the level was completed before alarm capacity was
        exhausted.
3.  Apply the hint star cap: `finalStars = min(normalStars, maxStars)`.

**\[IMPLEMENTATION RECOMMENDATION\]** Count each separately penalized
wrong action as one penalty incident for the "at most 1 penalty" star
condition. Keep the alarm increase separate. If the team instead defines
"penalty" as a numeric point deduction only, document that
interpretation consistently. The source does not fully define this
counting detail.

## 2.4 What counts as an optimal step?

The source gives the formula but does not define the exact
`optimalSteps` calculation separately for each Act 1 interaction. This
document proposes the following operational rules so the level manager
can calculate a target consistently.

  -----------------------------------------------------------------------
  Level                               Recommended `optimalSteps` basis
  ----------------------------------- -----------------------------------
  1.1                                 Total required successful room
                                      visits across the four traversal
                                      stages

  1.2                                 Total required successful token
                                      placements across both
                                      reconstruction stages

  1.3                                 Required node visits in postorder;
                                      height typing and final diameter
                                      submission are answer-entry
                                      actions, not extra room visits

  1.4                                 The minimum required room visits
                                      for the selected pair/LCA/path
                                      task; define this in the level data
                                      and use the same rule for every
                                      generated instance

  1.5                                 Required node swaps in manual mode;
                                      if Auto is used, use the same
                                      target optimal count for scoring
                                      rather than silently granting a
                                      better score
  -----------------------------------------------------------------------

These are **recommendations**, not definitions stated in the project
source. The team must finalize them before implementing the score
calculation. Do not change `optimalSteps` during an attempt.

For Level 1.1, the confirmed design is one level containing four
traversal stages. Therefore calculate one aggregate `optimalSteps` value
for all four stages, not four independent star ratings.

## 2.5 Alarm event policy for actions not explicitly covered

The official source explicitly defines the cost of visiting a node and
comparing a key. It does not explicitly say whether every drag, answer
submission, child swap, or path-tracing click costs an alarm step.

**\[IMPLEMENTATION RECOMMENDATION\]** Use a consistent rule: - Charge 1
alarm for each required successful node visit/placement/swap that
represents a deliberate operation in the level's learning objective. -
Charge the explicit extra penalty for an incorrect action where the
level rules define one. - Do not charge twice for one physical click.
For example, a wrong traversal click should add the wrong-traversal
penalty once; do not also count it as a successful visit. - Do not
charge for opening a panel, moving the mouse, dragging a token without
releasing it, or clicking empty background. - Do not charge for a
rejected input that is purely a UI mistake unless the level explicitly
defines it as a gameplay error.

This recommendation should be adopted or adjusted once, centrally,
rather than reimplemented differently in each level.

## 2.6 Alarm-capacity boundary

Treat the capacity as a hard limit. If capacity is 14, the attempt fails
as soon as the counter becomes 14. Do not allow the player to complete
an action at 14 and then continue.

A safe scoring helper should check the limit immediately after each
alarm increment:

``` cpp
if (score.alarmSteps >= score.alarmCapacity)
{
    levelState = "failed";
}
```

The level manager must stop accepting further gameplay actions after
failure, except retry, menu navigation, or replay.

------------------------------------------------------------------------

# 3. Level 1.1 --- Guards Patrol in a Given Order

## 3.1 Learning objective

Teach the player the four standard binary-tree traversals:

1.  Preorder: Root → Left → Right
2.  Postorder: Left → Right → Root
3.  Inorder: Left → Root → Right
4.  Level-order: visit nodes level by level, from left to right

**\[PROJECT REQUIREMENT\]** The player clicks rooms in the order that
the patrol will visit them. The clicked sequence must equal the
requested traversal.

## 3.2 Confirmed level structure

**\[CONFIRMED DESIGN\]** All four traversal challenges occur
sequentially inside one Level 1.1.

The order is:

1.  Preorder
2.  Postorder
3.  Inorder
4.  Level-order

After a stage is completed, the tree refreshes with new values and the
next traversal challenge begins.

There is: - one shared alarm counter, - one shared score, - one
aggregate optimal-step target, - one final star rating at the end of
Level 1.1.

There are not four separate level result screens.

## 3.3 Example tree and expected sequences

``` text
        A
       / \
      B   C
     / \ / \
    D  E F  G
```

  Traversal     Expected order
  ------------- ---------------------
  Preorder      A, B, D, E, C, F, G
  Postorder     D, E, B, F, G, C, A
  Inorder       D, B, E, A, F, C, G
  Level-order   A, B, C, D, E, F, G

The example is illustrative. The actual level should use generated or
configured trees.

## 3.4 Player flow

1.  Show the current tree.
2.  Show the requested traversal name and a short instruction.
3.  The player clicks the first room.
4.  Validate the selected node against the next expected node in the
    traversal sequence.
5.  If correct:
    -   mark the node as visited,
    -   highlight it,
    -   advance the expected index,
    -   record the visit event and alarm step.
6.  If incorrect:
    -   do not advance the expected index,
    -   show incorrect-selection feedback,
    -   add +1 alarm for the wrong traversal selection,
    -   allow the player to try again.
7.  When all nodes have been selected correctly, complete that traversal
    stage.
8.  Refresh the tree values and start the next traversal stage.
9.  After the fourth stage, calculate the final result for Level 1.1.

## 3.5 Implementation logic

### Traversal generation

Implement the traversal algorithms in `BinaryTree`, not in SFML or the
screen class.

Required model functions: - `preorder()` - `inorder()` - `postorder()` -
`levelOrder()`

The project API calls for recursive and iterative versions of the
traversals. The level can use the appropriate version for gameplay,
while unit tests should verify both versions return the same sequence.

The model should return node IDs (or a fixed/dynamic array of node IDs),
not screen coordinates.

### Level state

Suggested state:

``` cpp
struct TraversalStage
{
    const char* traversalName;
    int expectedNodeIds[MAX_TREE_NODES];
    int expectedCount;
    int nextExpectedIndex;
    int completed;
};
```

Do not use an enum for traversal state; project guardrails prohibit
enums. Use standardized lowercase `snake_case` string identifiers such
as `"preorder"` and `"level_order"`.

The level controller stores four stage definitions and a
`currentStageIndex`.

### Click handling

The GUI identifies which node ID is under the mouse and sends that ID to
game logic. Game logic compares it with:

``` text
expectedNodeIds[nextExpectedIndex]
```

If they match, accept the action. Otherwise, reject it and add the
wrong-traversal penalty.

### Refreshing the tree

After a traversal stage completes: - clear old highlights, - generate or
load the next tree, - compute the next traversal sequence, - reset the
stage's expected index, - keep the same level score object.

Do not reset the alarm counter or score between stages.

## 3.6 Level 1.1 scoring

**Optimal steps recommendation:**

``` text
optimalSteps = nodeCount(stage1)
             + nodeCount(stage2)
             + nodeCount(stage3)
             + nodeCount(stage4)
```

For four seven-node trees:

``` text
optimalSteps = 7 + 7 + 7 + 7 = 28
alarmCapacity = ceil(1.5 * 28) + 2 = 44
```

This is only an example. The actual values must be calculated from the
generated trees.

Example attempt: - 28 correct room visits. - 2 incorrect traversal
selections. - Total alarm steps = 30. - Score before any bonuses or
numeric penalties = `1000 - 10 × 30 = 700`. - If the two wrong
selections are counted as two penalty incidents, this does not qualify
for 2 stars because it exceeds one penalty. It can still earn 1 star if
completed before alarm capacity is exhausted.

A single wrong selection: - adds 1 alarm, - does not count as a correct
visit, - leaves the expected node unchanged, - does not deduct an
additional 50 points.

## 3.7 Edge cases and expected handling

  -----------------------------------------------------------------------
  Edge case                           Required handling
  ----------------------------------- -----------------------------------
  Player clicks a node that was       Reject as an incorrect traversal
  already correctly visited           selection; +1 alarm; expected index
                                      unchanged

  Player clicks a later node before   Reject; +1 alarm; no progress
  the expected node                   

  Player clicks empty space           No gameplay penalty

  Player clicks during the            Ignore input until the stage is
  tree-refresh animation              ready

  Tree contains only one node         All four traversals contain that
                                      node; one correct click completes
                                      the stage

  Tree is empty                       Do not generate an empty campaign
                                      challenge; show a level-data error
                                      or regenerate

  Tree is skewed                      Traversal algorithms must still
                                      return correct order

  Duplicate displayed values          Use node IDs for validation; visual
                                      labels alone must not determine
                                      identity

  Player reaches alarm capacity       Immediately fail the whole level;
  mid-stage                           do not start later stages

  Player completes stage 1 but fails  Retry the entire level from stage 1
  stage 3                             unless checkpointing is
                                      deliberately added

  Tree refresh accidentally resets    Bug: score and alarm must persist
  score                               for the entire level
  -----------------------------------------------------------------------

## 3.8 Acceptance tests

-   Each traversal returns the expected order for a known tree.
-   Recursive and iterative traversal implementations agree.
-   Correct clicks advance exactly one position.
-   Wrong clicks do not advance and add exactly one alarm step.
-   Repeated clicks on an already visited node are rejected.
-   Stage transitions do not reset the shared score.
-   Final stars are awarded once, after all four stages.
-   Alarm exhaustion at any stage fails the whole level.

------------------------------------------------------------------------

# 4. Level 1.2 --- Decode the Blueprint

## 4.1 Learning objective

Teach reconstruction of a binary tree using traversal sequences.

**\[PROJECT REQUIREMENT\]** The level has two reconstruction forms: -
preorder + inorder, - postorder + inorder.

The player drags room tokens into tree slots. Success occurs when the
built tree equals the hidden target tree.

## 4.2 Why two traversal sequences are needed

A single traversal does not always uniquely identify a tree's shape.

For example, preorder `A, B, C` could describe: - a chain where B is A's
left child and C is B's left child, - a chain where B is A's right child
and C is B's right child, - a root A with B as left child and C as right
child.

The inorder sequence identifies which values belong to the left and
right subtrees.

For reconstruction to be unambiguous, the node values must be distinct.
The Level 1.2 generator must ensure this.

## 4.3 Example target tree

``` text
        A
       / \
      B   C
     / \ / \
    D  E F  G
```

Sequences:

  Sequence    Values
  ----------- ---------------------
  Preorder    A, B, D, E, C, F, G
  Inorder     D, B, E, A, F, C, G
  Postorder   D, E, B, F, G, C, A

## 4.4 Confirmed player interaction

**\[CONFIRMED DESIGN\]** Use drag-and-drop, not text fields.

The screen should show: - the two labelled traversal sequences at the
top, - a tree-building area, - draggable room tokens containing the
values, - empty tree slots, - visual feedback for correct and incorrect
placement.

On a correct placement, highlight the placement blue. On an incorrect
placement, highlight it red and trigger the level's error/alarm
handling.

The player may choose any currently available empty slot; do not force a
left-to-right or level-order placement sequence.

The same interaction is reused for the second reconstruction stage.

## 4.5 Two-stage structure

**\[IMPLEMENTATION RECOMMENDATION\]** Keep both reconstruction
challenges inside one Level 1.2, consistent with the four-stage
structure used in Level 1.1:

-   Stage 1: reconstruct from preorder + inorder.
-   Stage 2: reconstruct from postorder + inorder.

Use one shared score and alarm counter, with one final star rating after
both stages. This grouping was proposed for consistency but should be
confirmed if the team wants a different level boundary.

After Stage 1 succeeds: 1. show a short success state, 2. clear the
construction board, 3. load the second sequence pair and target, 4.
reset only the stage-specific token/slot state, 5. preserve the level
score and alarm counter.

## 4.6 Reconstruction algorithm

### Preorder + inorder

1.  The first preorder value is the root.
2.  Find that value in the inorder sequence.
3.  Values before it in inorder belong to the left subtree.
4.  Values after it belong to the right subtree.
5.  Recursively reconstruct the left and right subtrees using the
    matching preorder ranges.

### Postorder + inorder

1.  The last postorder value is the root.
2.  Find that value in the inorder sequence.
3.  Values before it in inorder belong to the left subtree.
4.  Values after it belong to the right subtree.
5.  Recursively reconstruct the left and right subtrees using the
    matching postorder ranges.

The model's reconstruction functions should build a target tree from the
supplied arrays. The level should not duplicate the reconstruction
algorithm inside the GUI.

## 4.7 Drag-and-drop interaction implementation

A drag operation does not require a physics engine.

### Input state

Keep simple state variables: - `dragging` --- whether a token is
currently being dragged, - `selectedTokenId` or token index, - original
token position, - current mouse position, - drag offset, - an array of
token bounds, - an array of available slot bounds.

### Input sequence

1.  On mouse press, test whether the pointer is inside a token
    rectangle.
2.  If it is, mark that token as selected and begin dragging.
3.  While the mouse is held, draw the selected token at the pointer
    position using the drag offset.
4.  On mouse release, test whether the pointer is inside an available
    slot.
5.  If no slot is found, return the token to its original position.
6.  If a slot is found, send a placement request to game logic.
7.  Game logic validates the placement against the target tree and the
    current construction rules.
8.  Emit a placement/highlight event for the GUI.
9.  The renderer animates the token into the slot or returns it to the
    token tray.

### Architecture

-   `InputHandler`: detects press, drag, release, and the target slot.
-   `GameEngine` / level logic: validates the placement.
-   `BinaryTree`: owns and updates the actual tree structure.
-   `EventLog`: records the accepted structural change or rejection
    feedback.
-   `AnimationController`: plays the event.
-   `Renderer`: draws tokens, nodes, edges, empty slots, and highlights.

Do not let `InputHandler` directly assign `parent->left` or
`parent->right`.

## 4.8 Important design issue: do not reveal the hidden tree

If all target slots are drawn in their final positions before the player
starts, the shape of the hidden tree may be revealed. This can make the
reconstruction challenge too easy.

**\[IMPLEMENTATION RECOMMENDATION\]** Use a progressive tree-building
board: - initially show only the root placement area, - after a valid
parent is established, make its possible child positions available, -
keep the actual target relation hidden, - validate the player's choice
when they place a token.

However, the exact slot-reveal rule needs careful design. If the GUI
reveals only the correct child slot, it may reveal the answer. If it
always reveals both left and right slots, the player needs a clear rule
for when a branch is complete. A good implementation is to provide
left/right child placeholders for each placed node and let the player
decide which token belongs in each position, while the hidden target
validator checks the resulting structure.

The project source requires tree slots and drag-and-drop but does not
prescribe the exact slot-reveal method.

## 4.9 Validation rules

A stage succeeds only when: - every target node has been placed, - no
token is used more than once, - no node has more than one parent, - the
root is correct, - every left/right child relationship is correct, - the
constructed tree is structurally equal to the hidden target.

Use `isSameTree()` or an equivalent structural comparison. Compare both
values and child positions, not just the set of values.

The model should reject: - a second root, - a child assigned to two
parents, - a cycle, - a node placed under itself, - more nodes than the
target contains, - an invalid left/right relation.

## 4.10 Level 1.2 scoring

**Recommended optimal steps:**

``` text
optimalSteps = numberOfSuccessfulPlacementsInStage1
             + numberOfSuccessfulPlacementsInStage2
```

For two seven-node target trees:

``` text
optimalSteps = 7 + 7 = 14
alarmCapacity = ceil(1.5 * 14) + 2 = 23
```

This is an illustrative calculation, not a fixed value for every
generated level.

**\[OPEN DECISION\]** The source does not explicitly define the alarm
cost for a correct drag-and-drop placement or an incorrect placement in
Act 1. Recommended consistent policy: - each accepted token placement
costs 1 alarm step, - an incorrect placement adds +1 alarm as an
additional error step, - an invalid drop outside any slot costs no
alarm, - a token may be returned to the tray without penalty if the
player cancels before committing.

If the team chooses not to charge for correct placements, `optimalSteps`
must instead be based on the actual alarm-costing actions. Do not
calculate capacity from actions that do not increment the alarm meter.

## 4.11 Edge cases and expected handling

  -----------------------------------------------------------------------
  Edge case                           Required handling
  ----------------------------------- -----------------------------------
  Duplicate values in traversal       Reject the level data and
  arrays                              regenerate; reconstruction is
                                      ambiguous

  Traversal arrays have different     Invalid level data; do not start
  lengths                             gameplay

  A value in one traversal is missing Invalid level data
  from the other                      

  Same value appears more than once   Invalid level data for this
                                      reconstruction challenge

  Preorder/postorder and inorder      Reconstruction validator rejects
  describe inconsistent trees         the data

  Player drops token outside a slot   Return token to its original tray
                                      position; no tree change

  Player drops token into an occupied Reject placement; show red feedback
  slot                                

  Player tries to use one token twice Reject; token identity/state
                                      prevents duplicate use

  Player tries to place a second root Reject

  Player attempts to create a cycle   Reject before modifying pointers
  or shared child                     

  All values are placed but shape is  Do not complete; show mismatch
  wrong                               feedback and allow correction

  Player completes Stage 1            Start Stage 2 without resetting
                                      level score

  Player reaches alarm capacity       Fail Level 1.2 immediately

  Target tree has a missing left or   Represent the missing child as
  right child                         null; do not invent a node
  -----------------------------------------------------------------------

## 4.12 Acceptance tests

-   Reconstruct a known tree correctly from preorder + inorder.
-   Reconstruct the same tree correctly from postorder + inorder.
-   Reject duplicate values and inconsistent traversal pairs.
-   Dragging outside a slot does not mutate the tree.
-   An invalid placement cannot create a cycle or shared node.
-   Correct placement feedback and incorrect placement feedback are
    distinct.
-   The final check compares full structure and values.
-   Stage 2 begins with a clean board and preserved level score.

------------------------------------------------------------------------

# 5. Level 1.3 --- The Alarm Spreads from the Leaves Upward

## 5.1 Learning objective

Teach: - postorder traversal, - bottom-up height calculation, - tree
diameter.

**\[PROJECT REQUIREMENT\]** The player enters each room's height before
the alarm reaches the root. When all node heights are correct, the
player reports the tree diameter.

## 5.2 Height rule

Using the recommended edge-count convention:

``` text
height(null) = -1
height(leaf) = 0
height(node) = 1 + max(height(node.left), height(node.right))
```

A node's height cannot be calculated until the heights of its children
are known. This is why postorder is used: Left → Right → Root.

## 5.3 Example

``` text
           A
          / \
         B   C
        / \
       D   E
```

Postorder:

``` text
D, E, B, C, A
```

Correct heights:

  Node     Height Reason
  ------ -------- -----------------
  D             0 Leaf
  E             0 Leaf
  B             1 `1 + max(0, 0)`
  C             0 Leaf
  A             2 `1 + max(1, 0)`

Tree height is 2 edges.

Diameter is the longest path between any two nodes. For this example:

``` text
D → B → A → C
```

The diameter is 3 edges.

## 5.4 Confirmed player interaction

1.  Show the tree.
2.  Show the postorder instruction.
3.  Player manually clicks the next node in postorder.
4.  When the correct node is selected, show one shared grey input box.
5.  Player types the height for that selected node.
6.  If correct, save the answer and proceed to the next node.
7.  If incorrect:
    -   keep the selected node active,
    -   show incorrect-answer feedback,
    -   add +1 alarm step,
    -   allow retry,
    -   do not advance.
8.  After all node heights are correct, ask the player to enter the tree
    diameter.

The grey input box is reused for every node; do not create a separate
box beside every node.

## 5.5 Implementation workflow

### Model calculations

Implement a recursive height function in `BinaryTree`. The model should
be able to calculate the correct height for any node.

For the game, generate the expected postorder node-ID sequence. Store
the expected height for each node or query it from the model when
validating the player's answer.

The GUI must not calculate heights by inspecting screen positions.

### Player selection and answer entry

Maintain: - the expected postorder sequence, - the index of the next
expected node, - the currently selected node ID, - the input text, - the
node-height answer state.

When the player clicks: - if the node ID is not the next expected ID,
apply the wrong traversal selection penalty and keep the expected index
unchanged; - if it is correct, select it and enable the height input
box.

When the player submits a height: - parse the input as an integer, -
compare it with the model's expected height, - on success, store/mark
the answer and advance the postorder index, - on failure, keep the node
selected and add the agreed wrong-answer alarm penalty.

### Diameter calculation

After all node heights are correct: 1. calculate the tree diameter using
the model, 2. show a diameter input prompt, 3. validate the player's
submitted integer, 4. complete the level only if it matches the expected
diameter.

A standard efficient implementation calculates the diameter using a
postorder recursion that returns subtree height while updating the best
diameter. For a node:

``` text
candidateDiameter = leftHeight + rightHeight + 2
```

The empty-child height is -1 under the selected convention. This formula
handles leaves because `-1 + -1 + 2 = 0`.

## 5.6 Level 1.3 scoring

**Recommended optimal steps:** one required correct node visit per node
in postorder.

``` text
optimalSteps = numberOfNodes
alarmCapacity = ceil(1.5 * optimalSteps) + 2
```

For a seven-node tree:

``` text
optimalSteps = 7
alarmCapacity = ceil(10.5) + 2 = 13
```

Example: - 7 correct node selections. - 1 wrong traversal click. - 1
incorrect height submission. - Total alarm steps = 9 if each wrong
action adds one alarm and correct visits cost one. - Score =
`1000 - 10 × 9 = 910`, before any bonuses or numeric penalties. - There
are two penalty incidents, so the attempt does not qualify for the
2-star threshold under the recommended penalty-count interpretation. If
completed before alarm capacity, it earns 1 star.

The +1 alarm for an incorrect height entry is a **confirmed design
choice from the discussion**, not a separately listed official SRS
penalty. The official SRS explicitly lists wrong traversal selection;
this additional height-answer penalty is a game-specific rule agreed
during design.

The final diameter answer is also an answer-entry action. The source
does not specify a numerical penalty for an incorrect diameter
submission.

**\[IMPLEMENTATION RECOMMENDATION\]** Apply the same +1 alarm and retry
behavior to an incorrect diameter answer, so all numeric answer errors
are handled consistently. Mark this as a team-approved gameplay rule
before coding.

## 5.7 Edge cases and expected handling

  -----------------------------------------------------------------------
  Edge case                           Required handling
  ----------------------------------- -----------------------------------
  Player selects a node out of        +1 alarm; do not advance
  postorder sequence                  

  Player selects a node already       Reject; +1 alarm as a wrong
  completed                           traversal selection

  Player enters a negative height     Reject unless the selected
                                      convention and node type make it
                                      valid; for a non-empty node, height
                                      cannot be negative

  Player enters decimal or            Show validation message; do not
  non-numeric text                    treat malformed text as a correct
                                      answer

  Player enters a valid but incorrect +1 alarm under the confirmed
  integer                             height-answer rule; allow retry

  Player submits an empty input       Do not evaluate; no alarm

  Player presses Enter repeatedly     Process only one submission per
                                      input event; do not double-count

  Player enters the correct height    Reject because node selection must
  for the wrong node                  be correct first

  A node has only one child           Height is `1 +` the existing
                                      child's height; the missing child
                                      is -1

  Tree is a single node               Node height 0; diameter 0

  Empty tree                          Do not start a campaign height
                                      challenge; handle as invalid level
                                      data

  Diameter is measured in edges       Use the documented edge-count
                                      convention consistently

  Diameter endpoints are in different Candidate diameter passes through
  subtrees                            the current node

  Longest path is entirely inside one Recursive diameter must preserve
  subtree                             the best child diameter

  Alarm reaches capacity while        Fail immediately; do not show the
  entering heights                    diameter stage
  -----------------------------------------------------------------------

## 5.8 Acceptance tests

-   Heights are correct for leaves, one-child nodes, balanced trees, and
    skewed trees.
-   Postorder sequence is correct.
-   Wrong node selection does not advance.
-   Wrong height answer keeps the same node active.
-   The grey input box is reused.
-   Diameter is correct for a single node, a chain, and a tree whose
    diameter passes through the root.
-   The diameter stage appears only after all heights are correct.

------------------------------------------------------------------------

# 6. Level 1.4 --- Find Where Two Escape Routes Meet

## 6.1 Learning objective

Teach: - Lowest Common Ancestor (LCA) in a general binary tree, - the
path between two nodes, - path length.

**\[PROJECT REQUIREMENT\]** The player selects two rooms, clicks the
meeting room, then traces the path. Success requires the correct LCA and
path length.

## 6.2 Definition of LCA

The Lowest Common Ancestor of two nodes is the deepest node that is an
ancestor of both selected nodes. A node can be considered an ancestor of
itself for the purpose of this operation.

Example:

``` text
          A
         / \
        B   C
       / \ / \
      D  E F  G
```

-   LCA(D, E) = B.
-   LCA(D, G) = A.
-   LCA(B, D) = B.
-   LCA(C, C) = C.

## 6.3 Path and path length

The path is the unique sequence of connected nodes from the first
selected node to the second.

Using the edge-count convention: - D → B → E has 2 edges. - D → B → A →
C → G has 4 edges. - A node to itself has path length 0.

The project source says "path length" but does not explicitly say
whether it counts edges or nodes. This document recommends edges to
remain consistent with the height convention.

## 6.4 Player flow

1.  Show the tree.
2.  Ask the player to select the first room.
3.  Ask the player to select the second room.
4.  Highlight both selected rooms.
5.  Ask the player to click the LCA (the meeting room).
6.  Validate the selected LCA.
7.  If correct, display or enable path tracing.
8.  The player traces the path from the first room to the second room.
9.  Ask for or derive the path length, according to the selected UI
    design.
10. Complete the level only when the LCA and path length are correct.

**\[OPEN DECISION\]** The exact path-tracing input has not yet been
finalized. The source requires the player to trace the path but does not
define whether this is done by clicking each node in sequence, dragging
a line, or selecting a displayed path. Recommended implementation: click
each node in order from the first selected room to the second, including
both endpoints. This is easy to validate and animate in SFML.

## 6.5 Recommended interaction details

-   First selected node: blue highlight.
-   Second selected node: a different highlight, such as amber.
-   Correct LCA: green highlight.
-   Incorrect LCA: red feedback and retry.
-   Correct path nodes/edges: animate the route from start to
    destination.
-   Path length: display the edge count or ask the player to enter it.
    The team should choose one approach and use it consistently.

**\[OPEN DECISION\]** The earlier design discussion considered two ways
to test LCA: - player directly clicks the LCA; - game highlights common
ancestors and player chooses the lowest. The first is recommended
because it tests the concept directly and is simpler to implement, but
it has not been explicitly confirmed by the user.

## 6.6 Model implementation

Required model operations: - `lca(a, b)` for a general binary tree, -
`pathTo(node)` or an equivalent root-to-node path function, - path
construction between two nodes, - path length calculation.

If parent pointers are maintained: 1. Build the ancestor chain from the
first node to the root. 2. Build the ancestor chain from the second node
to the root. 3. Find the first common ancestor. 4. Construct the path by
moving from the first node up to the LCA, then from the LCA down to the
second node.

If parent pointers are not maintained, use recursive search/path
construction. Do not use BST ordering to find the LCA; Act 1 is a
general binary tree.

## 6.7 Path-tracing validation

For a path represented as node IDs:

``` text
expectedPath = [firstNode, ..., lca, ..., secondNode]
```

The game validates each selected node against the next expected node.
The player must not jump to a non-adjacent node.

If the player is required to click the endpoints as part of the path,
define that consistently. Recommended: the first and second nodes are
already selected, so the trace phase asks for the intermediate nodes
only. This avoids making the player click the same endpoints twice.

## 6.8 Level 1.4 scoring

The official score formula applies, but the source does not specify the
exact alarm cost for selecting the two endpoints, selecting the LCA, or
tracing each path node.

**\[IMPLEMENTATION RECOMMENDATION\]** - Count each accepted room
selection/visit as one alarm step. - Count each accepted path node visit
as one alarm step if the player physically traces the path by clicking
nodes. - An incorrect LCA selection should add +1 alarm and allow retry.
This is a proposed Act 1 rule, not an explicit SRS penalty. - An
incorrect path-node selection should add +1 alarm and leave the expected
path position unchanged. This is a proposed rule. - Clicking empty
background should not add alarm. - Avoid charging twice for the same
node when it was selected as an endpoint and then included again in the
trace.

Calculate `optimalSteps` from the exact action policy selected for the
level. Store the target value in level data or calculate it from the
target path. Do not hardcode a single alarm capacity for every tree.

## 6.9 Edge cases and expected handling

  -----------------------------------------------------------------------
  Edge case                           Required handling
  ----------------------------------- -----------------------------------
  First and second selected node are  LCA is that node; path contains one
  the same                            node; edge length is 0

  One selected node is an ancestor of LCA is the ancestor node
  the other                           

  Nodes are in separate root subtrees LCA may be the root

  Player clicks the same node for     Accept as a same-node task if
  both endpoints                      generated; otherwise regenerate if
                                      the level requires distinct
                                      endpoints

  Player selects a node ID that is    Reject without changing the task
  not in the current tree             

  Player selects the wrong LCA        Show feedback; add the agreed error
                                      alarm; allow retry

  Player traces a non-adjacent node   Reject; keep the expected path
                                      index unchanged

  Player traces in reverse order      Either accept the reverse path if
                                      the task allows either direction,
                                      or reject consistently; recommended
                                      to accept reverse order because the
                                      undirected path is the same

  Player clicks an endpoint twice     Ignore duplicate endpoint input or
                                      reject without advancing

  The tree is empty                   Invalid level data; do not start

  The path has no intermediate nodes  Complete the trace immediately
                                      after the endpoints/LCA are
                                      validated

  LCA calculation accidentally uses   Unit test must fail; use
  BST ordering                        general-tree logic only

  Alarm reaches capacity              Fail the level immediately
  -----------------------------------------------------------------------

## 6.10 Acceptance tests

-   LCA of siblings is their parent.
-   LCA of nodes in different root subtrees is the root.
-   LCA of ancestor/descendant pair is the ancestor.
-   LCA of a node with itself is that node.
-   Path sequence is correct in both directions.
-   Path length counts edges, not nodes.
-   A wrong LCA or path selection does not silently advance.
-   Invalid node IDs are rejected safely.

------------------------------------------------------------------------

# 7. Level 1.5 --- Mirror the Vault

## 7.1 Learning objective

Teach tree inversion (mirror) and recursion.

**\[PROJECT REQUIREMENT\]** The player swaps left and right children
bottom-up, or presses Auto. Success occurs when the resulting tree
equals the mirror of the original tree.

## 7.2 What mirroring means

Mirroring a tree means swapping the left and right child pointers at
every node.

Original:

``` text
       A
      / \
     B   C
    / \
   D   E
```

Mirror:

``` text
       A
      / \
     C   B
        / \
       E   D
```

This changes the tree structure. It is not merely a visual flip of the
drawing.

## 7.3 Recursive algorithm

``` cpp
void mirror(TreeNode* node)
{
    if (node == nullptr)
        return;

    TreeNode* temp = node->left;
    node->left = node->right;
    node->right = temp;

    mirror(node->left);
    mirror(node->right);
}
```

The operation: 1. stops when it reaches a null pointer, 2. swaps the
current node's children, 3. recursively mirrors the new left subtree, 4.
recursively mirrors the new right subtree.

Swapping the children before recursion is valid because both subtrees
will still be processed.

## 7.4 Player interaction

The source requires bottom-up child swaps by clicking nodes, or an Auto
option.

**\[IMPLEMENTATION RECOMMENDATION\] Manual mode** 1. Show the original
tree and preserve a copy/snapshot of its structure. 2. Identify nodes
whose child swap has not yet been completed. 3. The player clicks a
node. 4. The game checks whether the node is eligible under the
bottom-up rule. 5. If eligible, swap its left and right child pointers.
6. Emit a swap event and animate the children changing sides. 7. Mark
that node as processed. 8. Continue until all nodes are processed. 9.
Compare the resulting tree with the expected mirror of the original.

The simplest click interaction is: click a node to swap its children
immediately. This is simpler than requiring the player to drag both
child nodes across each other.

**\[OPEN DECISION\]** The source says "clicking nodes bottom-up" but
does not define whether a click immediately swaps the children or opens
a separate left/right selection interaction. Immediate swap on click is
the recommended approach.

**\[PROJECT REQUIREMENT\] Auto mode:** the player may press Auto. The
game performs the mirror operation recursively and displays the swaps
through animation. The Auto control should call the model's mirror
operation; the GUI should not independently perform pointer swaps.

## 7.5 Bottom-up eligibility

A strict bottom-up interpretation means a node is processed only after
its existing children have been processed.

A simple implementation can compute a postorder sequence of node IDs and
use it as the manual click order: - left subtree, - right subtree, -
current node.

This provides a deterministic bottom-up order.

If the player clicks a parent before its children: - reject the
action, - keep the node unchanged, - show feedback.

**\[OPEN DECISION\]** The source does not state the exact penalty for
clicking an ineligible node or performing an incorrect manual swap. Do
not automatically apply the wrong-rotation penalty; that penalty is for
AVL rotation choices. The team must approve a Level 1.5 error policy.

## 7.6 Expected mirror and equality validation

Before the player starts: 1. Preserve the original tree or create a
structural copy. 2. Create the expected mirror from that copy. 3. Keep
the expected mirror separate from the player's working tree.

After manual or Auto mode: - compare the player's tree with the expected
mirror using `isSameTree()`, - compare node values and left/right
relationships, - ensure all nodes are present exactly once.

Do not compare only the traversal values. A traversal alone may not
prove that the final structure is the exact mirror.

## 7.7 Level 1.5 scoring

The official scoring formula applies. The source does not define a
special numeric penalty for an incorrect mirror action.

**\[IMPLEMENTATION RECOMMENDATION\]** - In manual mode, each accepted
node swap costs 1 alarm step. - Use the number of nodes as the
manual-mode `optimalSteps`, because each node is swapped once. - A click
on a node before its children are processed is an invalid action.
Recommended: +1 alarm and one penalty incident, then allow retry. This
is a proposed rule, not an explicit source penalty. - Do not subtract 50
points for a wrong mirror click; that amount is specified for a wrong
AVL rotation choice only. - Auto mode should still be scored against the
same level target. Do not silently award a different optimal value. If
the team wants Auto to reduce the star cap, that must be an explicit new
rule; the source does not state such a reduction.

For a seven-node tree:

``` text
optimalSteps = 7
alarmCapacity = ceil(1.5 * 7) + 2 = 13
```

If the player completes seven valid swaps and makes one invalid early
click: - alarm steps = 8, - score = 920 before other numeric penalties
or bonuses, - one penalty incident means at most 2 stars under the
recommended interpretation, assuming the step threshold is also met.

## 7.8 Edge cases and expected handling

  -----------------------------------------------------------------------
  Edge case                           Required handling
  ----------------------------------- -----------------------------------
  Empty tree                          Mirror operation returns safely; do
                                      not start an empty campaign
                                      challenge

  Single-node tree                    No child links to exchange; it is
                                      already its own mirror

  Leaf node                           Mark as processed; swapping two
                                      null children has no visible effect

  Node has only a left child          After swap, that child becomes the
                                      right child

  Node has only a right child         After swap, that child becomes the
                                      left child

  Node has two children               Swap left and right pointers

  Player clicks parent before         Reject under bottom-up rule; apply
  children                            only the team-approved penalty

  Player clicks the same node twice   Prevent a second swap; otherwise
                                      the tree could be restored to its
                                      original state

  Player clicks during animation      Ignore or queue input; recommended
                                      to disable input until animation
                                      completes

  Auto is pressed after partial       Do not blindly mirror the already
  manual progress                     partially mirrored tree and compare
                                      to the original expected mirror;
                                      either resume only unprocessed
                                      nodes or restore the initial
                                      snapshot and run Auto from the
                                      beginning

  Auto is pressed repeatedly          Disable while running; once
                                      complete, do not run a second
                                      inversion

  Parent pointers are used            Update parent links consistently
                                      when child pointers swap; the
                                      parent of each child remains the
                                      same node, but left/right ownership
                                      changes

  Tree has repeated values            Use stable node IDs and structural
                                      position, not values alone

  Final structure has the right       `isSameTree()` must reject it
  values but wrong child sides        

  Alarm reaches capacity              Fail the level immediately
  -----------------------------------------------------------------------

## 7.9 Acceptance tests

-   Mirroring a known tree produces the expected structure.
-   Mirroring twice restores the original tree.
-   Empty and single-node trees do not crash.
-   One-child nodes are mirrored correctly.
-   Manual mode enforces bottom-up order.
-   A node cannot be swapped twice in the same manual attempt.
-   Auto mode runs the model operation and animates the resulting
    events.
-   Partial manual progress followed by Auto has a defined, tested
    behavior.
-   Final success uses structural equality against a preserved expected
    mirror.

------------------------------------------------------------------------

# 8. Recommended Act 1 Implementation Architecture

## 8.1 Separation of responsibilities

Follow the project architecture and coding guardrails. The core rule is:

**Data structures know nothing about SFML. Game logic owns the rules.
The GUI displays and animates the results.**

Recommended flow:

``` text
SFML Input
    |
    v
InputHandler / UIManager
    |
    v
GameEngine / LevelManager
    |
    v
BinaryTree model
    |
    v
EventLog
    |
    v
AnimationController
    |
    v
Renderer
```

The model operation produces events. The animation controller reads
those events and asks the renderer to display them.

## 8.2 Responsibilities

  -----------------------------------------------------------------------
  Component                           Responsibility
  ----------------------------------- -----------------------------------
  `TreeNode`                          Stores node ID, value, child
                                      pointers, optional parent pointer

  `BinaryTree`                        Traversals, reconstruction, height,
                                      diameter, LCA, path, mirror,
                                      equality

  `LevelManager`                      Loads Act 1 level data, controls
                                      stage progression, checks
                                      success/failure

  `GameEngine`                        Receives validated player actions
                                      and coordinates model operations

  `ScoreSystem`                       Alarm steps, capacity, penalties,
                                      score, stars

  `InputHandler` / `UIManager`        SFML mouse and keyboard input;
                                      identifies clicked
                                      nodes/tokens/slots

  `LayoutEngine`                      Calculates positions for nodes,
                                      edges, and slots

  `Renderer`                          Draws tree, tokens, sequences,
                                      input boxes, highlights, HUD

  `EventLog`                          Stores events and the information
                                      needed for animation/undo/replay

  `AnimationController`               Plays events with timing, pause,
                                      and step controls

  `Validator`                         Checks tree integrity and other
                                      invariants
  -----------------------------------------------------------------------

## 8.3 Event-driven GUI

The project architecture defines events such as:

  -----------------------------------------------------------------------
  Event                   Useful payload          Visual effect
  ----------------------- ----------------------- -----------------------
  `VISIT`                 `nodeId`                Node pulses; alarm
                                                  increases

  `HIGHLIGHT_SUBTREE`     `nodeId`, mode          Highlights
                                                  visited/result regions

  `UPDATE_HEIGHT`         `nodeId`, old value,    Height label updates
                          new value               

  `SWAP_CHILDREN`         `nodeId`, old           Children animate to
                          left/right IDs          exchanged sides

  `TREE_REBUILT`          tree snapshot or        Renderer refreshes tree
                          rebuild identifier      layout

  `LEVEL_STAGE_CHANGED`   stage name/index        Instruction panel
                                                  updates

  `LEVEL_COMPLETED`       summary                 Result screen appears

  `LEVEL_FAILED`          failure reason, score   Failure screen appears
                          summary                 
  -----------------------------------------------------------------------

`SWAP_CHILDREN`, `TREE_REBUILT`, and stage events are proposed additions
for Act 1. Add them to the central event contract and use consistent
lowercase `snake_case` identifiers. Do not scatter arbitrary event
strings throughout the code.

Each event should contain enough information for the GUI to animate the
change and, where required, undo/replay it.

## 8.4 Layout and hit testing

The `LayoutEngine` should calculate screen positions from the tree
structure. The renderer draws nodes and edges at those positions.

For mouse selection: 1. Iterate through the visible node hitboxes. 2.
Check whether the mouse position is inside a node's rectangle/circle. 3.
Return the corresponding stable node ID. 4. Send the ID to game logic.

For Level 1.2: - tokens and tree slots also need hitboxes, - drag hit
testing belongs in the input/UI layer, - slot validation belongs in game
logic.

Do not use the node's screen coordinate as its identity.

## 8.5 Coding guardrails

The uploaded coding guardrails require: - structs for data models and
nodes; - classes for related operations and functionality; -
declarations in `.h` files and implementations in `.cpp` files; - manual
implementation of required data structures; - no STL containers such as
`std::vector`; - no enums; use standardized lowercase `snake_case`
strings; - no unnecessary templates, operator overloading, try/catch, or
global variables; - keep SFML dependencies out of the core data
structures; - handle empty structures, invalid operations, and memory
management; - test modules independently before integration.

Use fixed arrays or manually managed dynamic arrays where appropriate.
The campaign generator currently targets 6--15 nodes for Act 1 in the
architecture notes; size arrays accordingly or allocate them safely
based on the configured node count.

## 8.6 Suggested source organization

Adapt names to the existing repository rather than moving working files
unnecessarily.

``` text
src/
├── structures/
│   └── binary_tree/
│       ├── BinaryTree.h
│       └── BinaryTree.cpp
├── game/
│   ├── levels/
│   │   ├── LevelManager.h
│   │   ├── LevelManager.cpp
│   │   ├── Act1Level11.h
│   │   ├── Act1Level11.cpp
│   │   ├── Act1Level12.h
│   │   ├── Act1Level12.cpp
│   │   ├── Act1Level13.h
│   │   ├── Act1Level13.cpp
│   │   ├── Act1Level14.h
│   │   ├── Act1Level14.cpp
│   │   ├── Act1Level15.h
│   │   └── Act1Level15.cpp
│   └── scoring/
│       ├── ScoreSystem.h
│       └── ScoreSystem.cpp
├── core/
│   └── events/
├── visualization/
│   ├── tree/
│   ├── renderer/
│   └── animation/
└── ui/
```

This is a suggested organization, not a requirement to create one class
per level. If the current project already has a level framework, extend
it instead of duplicating it.

------------------------------------------------------------------------

# 9. Level Data and Generation

## 9.1 Level data requirements

The project requires level definitions to be data-driven and supports
adding levels without rewriting core game logic. The architecture notes
specify seeded generation and Act 1 general binary trees of 6--15 nodes.

Each Act 1 level definition should contain or reference: - level ID, -
initial tree data or generation seed, - node IDs and values, - stage
definitions, - expected traversal sequences or target tree, -
optimal-step calculation inputs, - alarm capacity (calculated from
optimal steps), - any allowed hint or assistance settings.

Do not store screen coordinates as part of the logical tree data. The
layout engine calculates those.

## 9.2 Seeded generation

Use a seed so a level can be reproduced during debugging and testing.

The generator should: 1. create the requested number of nodes, 2. assign
stable unique IDs, 3. assign values according to the level's rules, 4.
connect nodes into a valid binary tree, 5. validate the tree, 6.
calculate the required expected answers, 7. calculate `optimalSteps` and
alarm capacity.

For Level 1.2, ensure all values are distinct. For other Act 1 levels,
the source permits arbitrary values; node identity must remain
independent of value.

## 9.3 Level-data validation

Before showing a level, validate: - node count is within the configured
range, - every node ID is unique, - every node is reachable from the
root, - no cycles exist, - no node has more than one parent, - parent
pointers are consistent if used, - traversal arrays match the tree, -
Level 1.2 reconstruction arrays are valid and distinct, - target answer
data is consistent, - `optimalSteps` is positive for a non-empty
campaign challenge, - alarm capacity is calculated from the final
optimal-step value.

If validation fails, do not launch a broken level. Regenerate the level
or show a controlled level-data error.

------------------------------------------------------------------------

# 10. Global Edge Cases and Defensive Rules

These cases apply across all Act 1 levels.

  -----------------------------------------------------------------------
  Situation                           Required behavior
  ----------------------------------- -----------------------------------
  Null root                           Algorithms return safely; campaign
                                      generator should not start an empty
                                      challenge

  Null child                          Treat as an absent branch; do not
                                      dereference it

  Invalid node ID                     Reject the action without changing
                                      the tree

  Node ID repeated                    Validator detects duplicate
                                      identity

  Cycle in tree                       Validator detects and rejects it

  Shared child pointer                Validator detects a node with
                                      multiple parents

  Parent pointer mismatch             Validator reports an integrity
                                      error

  Node deleted or replaced during an  Do not allow stale input to
  animation                           reference it

  Input during animation              Disable input or queue it safely;
                                      do not mutate the model halfway
                                      through an animation

  Window resized                      Recalculate layout and hitboxes;
                                      node IDs remain unchanged

  Player closes the level             Save only a consistent state; do
                                      not save a half-applied pointer
                                      operation

  Retry                               Restore the original level state
                                      and reset attempt counters

  Hint                                Reduce maximum star rating by one;
                                      do not assume a point deduction

  Alarm full                          Stop gameplay immediately and show
                                      failure result

  Score becomes negative              Clamp final score to zero

  Integer input overflows             Reject malformed/out-of-range input
                                      safely

  Animation is skipped                Model state remains correct;
                                      animation is presentation, not the
                                      source of truth

  Undo/replay                         Restore or replay model events
                                      without changing the meaning of the
                                      level's expected answer
  -----------------------------------------------------------------------

The project deliverable recommends validating tree integrity, including
no cycles, no shared nodes, and consistent parent pointers where used.

------------------------------------------------------------------------

# 11. Act 1 Test Plan

## 11.1 BinaryTree unit tests

Test the model independently of SFML.

### Traversals

-   empty tree,
-   single-node tree,
-   balanced tree,
-   left-skewed tree,
-   right-skewed tree,
-   tree with missing left or right children,
-   recursive and iterative versions return the same results.

### Reconstruction

-   preorder + inorder reconstruction,
-   postorder + inorder reconstruction,
-   single-node reconstruction,
-   left-skewed and right-skewed reconstruction,
-   inconsistent arrays rejected,
-   duplicate values rejected for reconstruction.

### Height and diameter

-   empty tree height,
-   single-node height and diameter,
-   balanced tree,
-   left-skewed tree,
-   right-skewed tree,
-   longest path passing through root,
-   longest path contained entirely in one subtree.

### LCA and path

-   siblings,
-   ancestor and descendant,
-   nodes in separate subtrees,
-   node with itself,
-   path in both directions,
-   invalid node ID.

### Mirror and equality

-   mirror a balanced tree,
-   mirror a skewed tree,
-   mirror a one-child node,
-   mirror twice restores original,
-   equality checks values and structure,
-   symmetry check on symmetric and asymmetric trees.

## 11.2 Gameplay tests

### Level 1.1

-   all four traversal stages occur in the confirmed order;
-   stage tree refresh works;
-   one shared score is preserved;
-   wrong click adds one alarm and does not advance;
-   final stars appear only once.

### Level 1.2

-   drag/drop and cancel work;
-   valid reconstruction succeeds;
-   invalid structure fails validation;
-   no duplicate token use;
-   both reconstruction methods work;
-   stage transition preserves score.

### Level 1.3

-   postorder selection is enforced;
-   one grey input box is reused;
-   wrong height keeps the same node active;
-   diameter appears only after all heights are correct;
-   height and diameter conventions match.

### Level 1.4

-   selected nodes are valid;
-   LCA is correct for all tree relationships;
-   path is adjacent and ordered;
-   edge-count length is correct;
-   invalid path selections do not corrupt progress.

### Level 1.5

-   manual mode follows bottom-up order;
-   nodes cannot be swapped twice;
-   Auto produces the expected mirror;
-   final structural equality check is correct;
-   partial manual/Auto transition is handled.

## 11.3 Scoring tests

For every level: - `alarmCapacity = ceil(1.5 * optimalSteps) + 2`; -
exactly reaching capacity causes failure; - one step below capacity can
still complete; - score formula is applied once; - final score never
falls below zero; - wrong traversal adds exactly one alarm; - numeric
point penalties are applied only when explicitly defined; - hint cap is
applied after normal star calculation; - stages within one level do not
reset the level score.

------------------------------------------------------------------------

# 12. Recommended Implementation Order

Build Act 1 in this order so GUI work does not hide model bugs.

## Phase A --- Complete and test BinaryTree

1.  Implement node creation and destruction.
2.  Implement recursive traversals.
3.  Implement iterative traversals using manually implemented helper
    structures/arrays as allowed by the guardrails.
4.  Implement preorder + inorder reconstruction.
5.  Implement postorder + inorder reconstruction.
6.  Implement height and diameter.
7.  Implement LCA and path construction.
8.  Implement mirror.
9.  Implement `isSameTree()` and `isSymmetric()`.
10. Run model unit tests before GUI integration.

## Phase B --- Shared level and scoring foundation

1.  Implement level state and stage state.
2.  Implement `ScoreSystem`.
3.  Implement alarm capacity calculation.
4.  Implement penalty tracking.
5.  Implement score and star calculation.
6.  Test capacity boundary behavior.
7.  Ensure retry restores initial level state.

## Phase C --- GUI foundations

1.  Draw a tree from the model.
2.  Calculate node positions through `LayoutEngine`.
3.  Add node hit testing by ID.
4.  Add selected/visited highlights.
5.  Add the alarm meter and score display.
6.  Add event-driven animation.
7.  Disable or queue input during animations.

## Phase D --- Build levels in order

1.  Level 1.1: traversal click validation and four sequential stages.
2.  Level 1.2: draggable tokens, tree slots, reconstruction stages,
    structural validation.
3.  Level 1.3: postorder node selection, reusable height input, diameter
    input.
4.  Level 1.4: two-node selection, LCA selection, path trace and path
    length.
5.  Level 1.5: bottom-up manual child swaps and Auto mirror.

## Phase E --- Integration and quality checks

1.  Run each level with a known fixed tree.
2.  Test all edge cases in this document.
3.  Test alarm exhaustion at different stages.
4.  Test retry after failure.
5.  Test window resize and animation input locking.
6.  Test seeded level reproduction.
7.  Confirm all Act 1 model logic works without SFML.
8.  Confirm the GUI never directly modifies tree pointers.

------------------------------------------------------------------------

# 13. Decisions That Must Be Confirmed Before Final Implementation

The following details are not fully specified in the project source or
have only been proposed in the design discussion. They should be agreed
by the team and then implemented consistently.

  -----------------------------------------------------------------------
  Decision                Current recommendation  Status
  ----------------------- ----------------------- -----------------------
  Height convention       Edge-count: empty = -1, Recommended; document
                          leaf = 0                in code/report

  Path-length convention  Count edges             Recommended

  Level 1.1 grouping      Four traversal stages   Confirmed
                          in one level, one score 

  Level 1.1 traversal     Preorder → Postorder →  Confirmed
  order                   Inorder → Level-order   

  Level 1.2 grouping      Two reconstruction      Recommended; confirm
                          stages in one level,    
                          one score               

  Level 1.2 correct       1 per accepted          Recommended; source is
  placement alarm cost    placement               silent

  Level 1.2 wrong         +1 alarm                Recommended; source is
  placement penalty                               silent

  Level 1.2 slot          Progressive slots that  Recommended; exact UX
  visibility              do not reveal the       needs design
                          hidden shape            

  Level 1.3 wrong height  +1 alarm, retry same    Confirmed design choice
  answer                  node                    

  Level 1.3 wrong         +1 alarm, retry         Recommended; confirm
  diameter answer                                 

  Level 1.4 LCA           Player clicks the LCA   Recommended; not
  interaction             directly                confirmed

  Level 1.4 path          Click intermediate      Recommended; not
  interaction             nodes in path order     confirmed

  Level 1.4 wrong         +1 alarm, retry         Recommended; source is
  LCA/path action                                 silent

  Level 1.5 manual action Click a node to swap    Recommended; not
                          its children            confirmed
                          immediately             

  Level 1.5 bottom-up     Require postorder order Recommended
  validation                                      

  Level 1.5 wrong early   +1 alarm, retry         Recommended; source is
  click                                           silent

  Level 1.5 Auto scoring  Same target optimal     Recommended; source
                          count; no extra star    does not specify an
                          cap                     Auto penalty

  Penalty count for       Count each wrong action Recommended
  2-star rule             as one penalty incident interpretation
  -----------------------------------------------------------------------

Do not silently turn an open decision into a project requirement. Once
the team approves a recommendation, update this table and the relevant
level implementation.

------------------------------------------------------------------------

# 14. Definition of Done --- Act 1

Act 1 is complete when:

-   [ ] The general BinaryTree is implemented manually and independently
    of SFML.
-   [ ] All required BinaryTree operations are implemented and
    unit-tested.
-   [ ] Height and path-length conventions are documented.
-   [ ] Level 1.1 has four sequential traversal stages in the agreed
    order.
-   [ ] Level 1.2 reconstructs trees using both required traversal-pair
    methods.
-   [ ] Level 1.3 validates every node height and the final diameter.
-   [ ] Level 1.4 validates the LCA and path length.
-   [ ] Level 1.5 mirrors the tree and verifies structural equality.
-   [ ] Each level uses the shared alarm and score system.
-   [ ] Alarm capacity is calculated from `optimalSteps`.
-   [ ] Alarm exhaustion immediately fails the attempt.
-   [ ] Retry resets the attempt correctly.
-   [ ] All GUI operations communicate through game logic and events.
-   [ ] The GUI does not directly manipulate BinaryTree pointers.
-   [ ] Edge cases and invalid inputs are handled.
-   [ ] The five levels can be demonstrated and explained by both team
    members.

------------------------------------------------------------------------

# 15. Source Alignment Notes

This guide is based primarily on the project's uploaded **Project
Deliverable 1.pdf**, **requirment.md**, **archtecture.md**, and
**gardrails.md**, plus the design decisions made during the discussion.

The following are explicit project requirements: - Act 1 contains Levels
1.1--1.5 and uses a general binary tree. - The five level concepts and
core interactions listed in the level specification. - Alarm capacity
formula, star thresholds, score formula, and listed penalties. -
BinaryTree API operations. - Event-driven separation between model,
event log, animation controller, and renderer. - Manual data structures,
no STL containers, no enums, SFML for GUI, and separation of data
structure/game logic/GUI.

The following are design decisions or recommendations, not explicit
source requirements: - The exact stage grouping and shared scoring for
Level 1.2. - The exact alarm cost of drag placements, LCA selection,
path tracing, and mirror swaps. - The edge-count convention selected for
height and path length. - The precise drag-slot reveal strategy. - The
exact wrong-answer penalty for Level 1.3 diameter, Level 1.4, and Level
1.5. - The precise manual click behavior and error handling for Level
1.5. - The exact optimal-step formula for each individual Act 1
interaction.

These distinctions are intentional so implementation remains faithful to
the project specification without hiding design assumptions.
