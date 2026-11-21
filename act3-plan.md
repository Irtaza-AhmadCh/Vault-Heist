# Vault Heist: The Living Vault
# Act 3 — The Architect's Wing (AVL)
## Consolidated Gameplay and Implementation Specification

**Document status:** Consolidated design specification  
**Scope:** Act 3, Levels 3.1–3.5  
**Basis:** Project Deliverable 1, Coding Guardrails, System Architecture, Two-Person Development Roadmap, and the gameplay decisions discussed by the team.

---

## 1. Purpose and authority

This document defines the agreed gameplay flow and recommended implementation decisions for Act 3. It is intended to be the shared reference for the Act 3 designer, game-logic developer, AVL developer, and GUI/animation developer.

### 1.1 Rules that take priority

All Act 3 decisions must follow these constraints:

1. **Do not contradict Project Deliverable 1.** Act 3 must teach AVL balance factors, LL/RR single rotations, LR/RL double rotations, automatic rebalancing after insertion, and order statistics using `subtreeSize`, `kth`, and `rank`.
2. **Follow the Coding Guardrails.** Keep code beginner-friendly; implement the AVL tree manually; use structs for node data and classes for operations; keep declarations in headers and implementations in CPP files; do not use enums or STL containers; use SFML for the GUI.
3. **Follow the architecture.** The GUI collects input; game logic validates the action and calls the data structure; the model performs the operation; events are recorded; the renderer animates the events. The AVL model must not depend on SFML or GUI code.
4. **Prefer simple implementation.** Use direct functions, clear conditions, pointers, and the existing event system. Do not add unnecessary frameworks or a second gameplay system.
5. **Keep continuity across levels.** Each level should build on the skills learned in the previous level rather than introducing a different interaction style without a reason.

### 1.2 Source requirements versus design decisions

The Deliverable explicitly specifies the core learning objectives and some penalties. This document preserves those requirements. Where the Deliverable does not give a precise gameplay detail—such as the alarm cost of a wrong prediction—this document supplies a recommended design decision and labels it as such.

---

## 2. Act 3 overview

**Act name:** The Architect's Wing  
**Data structure:** AVL Tree  
**Main learning progression:**

1. Read balance-factor badges and identify unbalanced nodes.
2. Manually repair LL and RR cases using single rotations.
3. Manually repair LR and RL cases using double rotations.
4. Predict the rotation before each insertion, then watch the AVL system rebalance automatically.
5. Repair an unbalanced BST into an AVL tree, then use subtree-size information to find the k-th largest treasure.

The act should make the player understand why AVL trees are useful: they keep a search tree balanced so that operations such as searching and order-statistic selection can follow short paths.

### 2.1 Shared level flow

Each level follows the campaign flow from the Deliverable:

**Level briefing → Level play → Result screen → Next level / Retry**

The level screen should reuse the common HUD and toolbar:

- Current act and level
- Objective
- Tree visualization
- Alarm meter
- Score
- Steps taken and optimal steps
- Undo, Replay, Reset, and Hint controls where supported

The player begins each level at its defined starting state. The level briefing explains the objective and the interaction expected in that level.

---

## 3. Shared AVL concepts and system responsibilities

### 3.1 Balance factor

For every node:

`balanceFactor(node) = height(left subtree) - height(right subtree)`

Use the same height convention throughout the AVL implementation. The Deliverable's node definition uses leaf height `1`.

A node is balanced when its balance factor is `-1`, `0`, or `+1`. A node is unbalanced when its balance factor is less than `-1` or greater than `+1`.

The Deliverable's Level 3.1 wording says green means `1`, `0`, or `1`; the intended range is clearly **−1, 0, +1**. The GUI should display that correct range.

### 3.2 AVL node data

An AVL node needs the fields required by the existing AVL design, including:

- Key / treasure value
- Left and right child pointers
- Height
- Subtree size

Parent pointers may be used if the existing implementation needs them, but they are not required solely for these level mechanics.

### 3.3 Subtree size

`subtreeSize(node) = 1 + subtreeSize(node.left) + subtreeSize(node.right)`

For a null child, subtree size is `0`.

The model maintains this value automatically. The player does not count nodes or manually enter subtree sizes.

After insertion, deletion, or rotation, the AVL implementation updates the affected nodes' heights and subtree sizes. The update order must be bottom-up: update the lower affected node first, then its new parent.

The validator checks that each stored size matches the size recomputed from the actual tree.

### 3.4 Required order-statistic operations

The Deliverable's AVL API defines:

- `kth(k)`: returns the k-th smallest key using subtree sizes.
- `rank(key)`: returns the number of keys smaller than or equal to `key`.
- `countInRange(lo, hi)`: `rank(hi) - rank(lo - 1)`.

The Act 3.5 mission is specifically the **k-th most valuable treasure**, meaning k-th largest. The existing `kth(k)` function is defined as k-th smallest. Reuse it by converting the requested rank:

`convertedRank = nodeCount - k + 1`

Then call:

`kth(convertedRank)`

Example: If there are 7 treasures and the mission asks for the 3rd largest, call `kth(7 - 3 + 1)`, which is `kth(5)`. The returned value is the 5th smallest, which is also the 3rd largest.

The user-facing mission should continue to say **k-th largest**. The conversion is an internal implementation detail.

### 3.5 System versus player responsibilities

The system performs calculations and validation that are part of the data structure:

- Calculate balance factors.
- Calculate and update heights.
- Calculate and update subtree sizes.
- Determine whether an insertion causes an imbalance.
- Determine the required AVL rotation after an insertion.
- Convert k-th largest into the equivalent k-th smallest rank.
- Use subtree sizes to determine the direction of the order-statistic search.
- Validate the final AVL invariants and the selected treasure.

The player performs the gameplay decisions:

- Select unbalanced nodes in Level 3.1.
- Select the pivot and rotation in manual repair levels.
- Identify the LR/RL case in Level 3.3.
- Predict the rotation before an insertion in Level 3.4.
- Perform the balancing phase and follow the search path in Level 3.5.

**Important:** Subtree-size comparisons in Level 3.5 are internal system calculations. They are not a separate player minigame and do not consume alarm steps by themselves.

---

## 4. Alarm, score, and optimal-step rules

This section applies the Deliverable's common game rules to Act 3.

### 4.1 Alarm capacity

For every level:

`AlarmCapacity = ceil(1.5 * optimalSteps) + 2`

When the alarm meter reaches its capacity, the level fails.

### 4.2 Alarm costs explicitly defined by the Deliverable

- Entering / visiting a node: `+1 alarm step`.
- Comparing against a room's key: `+1 alarm step`.
- Wrong rotation choice: `+2 alarm steps` and `-50 score points`.
- The Deliverable also specifies a wrong traversal click as `+1 alarm step`; this is the closest existing rule for an incorrect node-selection action.

### 4.3 Act 3 decisions for alarm costs

The following decisions are used consistently in Act 3:

| Player action / mistake | Alarm effect | Score effect | Status |
|---|---:|---:|---|
| Visit a node during a required interaction or search | +1 | None | Deliverable rule |
| Compare against a node's key | +1 | None | Deliverable rule |
| Select a balanced node instead of an unbalanced node in 3.1 | +1 | None | Recommended: same as an incorrect traversal/node click |
| Choose the wrong pivot or rotation in 3.2 / 3.3 | +2 | −50 | Deliverable rule |
| Misidentify the LR/RL case in 3.3 | +2 | −50 | Recommended: it is an incorrect rotation choice |
| Predict the wrong rotation in 3.4 | +1 | None | Confirmed that prediction mistakes raise alarm; +1 is recommended to match the existing wrong-click penalty |
| System calculates a balance factor, height, or subtree size | 0 | 0 | System operation, not a player action |
| System performs the correct automatic rotation in 3.4 | 0 | 0 | Automatic model operation |
| System calculates a subtree-size comparison in 3.5 | 0 | 0 | System operation, not a player action |
| Correct manual rotation | 0 additional penalty | None | Recommended: no penalty for a correct action |

A required node visit or key comparison still counts under the common alarm rule even if the action is otherwise correct. “No penalty” means no extra mistake penalty.

### 4.4 Score

Use the Deliverable's score system:

`Score = 1000 - 10 * alarmSteps - penalties + bonuses`

The score is floored at `0`.

For an incorrect rotation, apply the specified `-50` score penalty in addition to the alarm increase. Do not apply the `-50` rotation penalty to an incorrect Level 3.4 prediction unless the player also makes an actual incorrect rotation choice.

### 4.5 Optimal steps

`optimalSteps` is calculated by the level/game logic from the shortest valid completion route for that level's configured starting state and objective. It is not manually typed by the player.

The calculation must be deterministic for a given level configuration. The level data should define the starting tree, target, and required interaction. The game logic should calculate the minimum required player-facing visits, comparisons, and required selections.

System-only calculations—such as updating a height, recomputing a subtree size, or deciding which child contains the requested rank—do **not** add alarm steps and do not count as player steps.

The actual `stepsTaken` counter should follow the same event accounting used by the alarm system. Do not count a single interaction twice. For example, when a node visit is also the point at which the key is compared, use the event rules consistently and avoid charging two visits for one arrival. If the game emits both `VISIT` and `COMPARE`, both may be counted only where the player genuinely incurs both costs under the Deliverable's rules.

For manual repair levels, the optimal route is the minimum correct sequence of player actions for the provided tree. For Level 3.5, it is the minimum valid balancing interaction sequence plus the shortest correct order-statistic search path under the agreed interaction model.

**Implementation note:** Because the exact starting trees and interaction events determine the numerical optimum, do not hard-code one universal optimal-step number for an entire level type. Calculate it from each level's actual configuration.

### 4.6 Stars

Use the Deliverable's common star rules:

- 3 stars: steps are at or below optimal, with no penalties.
- 2 stars: steps are at or below `1.25 * optimal`, with at most one penalty.
- 1 star: level completed within alarm capacity.
- 0 stars: alarm reaches capacity and the level fails.

A hint caps the level at 1 star, as stated in the Deliverable.

---

## 5. Level 3.1 — Read the Badges

### Objective

Identify all unbalanced nodes in the given AVL-style tree.

### Learning concept

Balance factor.

### Starting state

- The game displays a tree with balance-factor badges on every room.
- Green means the node's balance factor is `-1`, `0`, or `+1`.
- Red means the node violates the AVL balance condition: its balance factor is below `-1` or above `+1`.

### Gameplay flow

1. The level briefing explains that the player must identify all unbalanced rooms.
2. The tree appears with the balance-factor badge visible on every node, matching the Deliverable.
3. The player clicks each red / unbalanced node.
4. The system checks the selected node against its actual balance factor.
5. Correct selections are marked as found.
6. Selecting a balanced node or selecting an already selected node is treated as an incorrect selection and adds the defined mistake cost.
7. The level is completed when every unbalanced node has been selected.

### Success condition

Every unbalanced node is selected, and no unbalanced node remains unmarked.

### Alarm and optimal steps

- Each required node visit/selection costs one alarm step.
- An incorrect selection adds one alarm step.
- The optimal route is the number of distinct unbalanced nodes that must be selected, plus any required player-facing visits defined by the actual interaction implementation.
- The system's calculation/display of balance factors is free.

### Implementation guidance

- Balance factors should be computed by the AVL model or supplied by validated level data; do not ask the player to calculate them.
- The GUI displays the badges and receives clicks.
- Game logic checks whether the clicked node is unbalanced.
- Use a simple selected/found state per node in level logic.
- Do not make the player navigate to each red node by following a search path; the agreed interaction is direct selection of the visible red nodes.

---

## 6. Level 3.2 — Manual Repair: Single Rotations

### Objective

Repair the given LL and RR imbalances using the correct single rotation.

### Learning concept

- LL imbalance → right rotation.
- RR imbalance → left rotation.

### Starting state

The level presents a BST-shaped tree with a single-rotation imbalance. The player is expected to repair the tree manually.

### Gameplay flow

1. The briefing explains the current imbalance and asks the player to repair it.
2. The player inspects the tree and identifies the unbalanced node / pivot required by the level.
3. The player selects the pivot and chooses the required rotation through the agreed drag/selection interaction.
4. The game logic validates the chosen pivot and rotation type.
5. If correct, the AVL model performs the rotation and emits the rotation/update events.
6. The renderer animates the structural change.
7. Heights, balance factors, and subtree sizes are updated by the model.
8. The validator checks the resulting BST ordering, heights, subtree sizes, and AVL balance.
9. If the choice is wrong, the wrong-rotation penalty is applied and the player may try again, subject to the alarm limit.

### Success condition

The tree is a valid AVL tree, with BST ordering preserved and all balance factors in the allowed range.

### Alarm and optimal steps

- Player-facing node visits and key comparisons follow the common alarm rule.
- Incorrect rotation choice: `+2 alarm steps` and `-50 score`.
- Correct rotation: no additional mistake penalty.
- `optimalSteps` is the minimum valid interaction sequence for the specific starting tree: required pivot selection and correct rotation choice, plus any node visits/comparisons that the actual UI charges.

### Implementation guidance

- Keep the manual rotation operation in the AVL model.
- The GUI must not directly change child pointers.
- The model emits a `ROTATE` event and the relevant height/size update events.
- A wrong choice should be rejected before modifying the tree. This is the simplest safe approach and avoids needing to roll back an invalid rotation.
- Provide separate level configurations for LL and RR cases, or a level sequence containing one of each if the design calls for both.

---

## 7. Level 3.3 — Manual Repair: Double Rotations

### Objective

Identify and repair LR and RL imbalances using the correct two-rotation sequence.

### Learning concept

- LR case → left rotation on the left child, then right rotation on the unbalanced node.
- RL case → right rotation on the right child, then left rotation on the unbalanced node.

### Starting state

The level presents a tree with an LR or RL imbalance.

### Gameplay flow

1. The briefing explains that this is a double-rotation challenge.
2. The player examines the unbalanced node and its heavy child.
3. The player identifies whether the case is LR or RL.
4. The player chooses the corresponding double-rotation case.
5. The game logic validates the selected case.
6. If correct, the AVL model performs the two primitive rotations in the correct order.
7. The renderer animates the two rotations as a clear sequence.
8. The model updates heights and subtree sizes after the rotations.
9. The validator checks the final AVL tree.
10. If the player identifies the wrong case, apply the wrong-rotation penalty and allow another attempt if alarm capacity remains.

### Success condition

The correct double rotation is completed, BST ordering remains valid, and the resulting tree satisfies AVL balance.

### Alarm and optimal steps

- Incorrect LR/RL identification is treated as an incorrect rotation choice: `+2 alarm steps` and `-50 score`.
- Correct double rotation has no additional mistake penalty.
- Count required player-facing visits/comparisons according to the common event rules.
- `optimalSteps` is the minimum valid sequence: inspect/select the relevant pivot/case and choose the correct double rotation. The model's two internal primitive rotations are not two separate player decisions unless the UI explicitly asks the player to perform them separately.

### Implementation guidance

The Deliverable says the player identifies the case first and then performs two rotations. Preserve that learning objective. To keep interaction simple, the player chooses the case and initiates the double rotation; the game then animates the two required primitive rotations in sequence. Do not ask the player to manually manipulate multiple pointers.

If the existing agreed GUI specifically requires the player to initiate each of the two rotations separately, keep that interaction, but the model must still validate the required order.

---

## 8. Level 3.4 — Auto Mode and Prediction

### Objective

Predict which rotation, if any, will occur after an insertion, then watch the AVL tree rebalance automatically.

### Learning concept

AVL insertion with automatic rebalancing.

### Starting state and agreed interaction

The level provides a sequence/array of values to insert and an initially empty tree (the first insertion becomes the root). The player works through the sequence one value at a time.

### Gameplay flow

For each value in the provided sequence:

1. The next value is shown to the player.
2. Before insertion, the player predicts the result:
   - no rotation,
   - LL,
   - RR,
   - LR, or
   - RL.
3. The game records the prediction.
4. The AVL model inserts the value using normal BST insertion.
5. The model updates heights and checks balance while returning toward the root.
6. The model determines whether rebalancing is required and performs the appropriate rotation automatically.
7. The renderer animates the insertion and any rotation.
8. The game compares the player's prediction with the actual result.
9. If the prediction is correct, continue to the next value.
10. If the prediction is incorrect, add the prediction mistake cost and continue, provided the alarm has not reached capacity.

### Success condition

All values in the sequence are inserted, the player's predictions have been evaluated, and the final tree is a valid AVL tree.

### Prediction options

Use these exact logical choices:

- `no_rotation`
- `ll`
- `rr`
- `lr`
- `rl`

These are string identifiers, not enums, in accordance with the Coding Guardrails.

### Alarm and optimal steps

- Incorrect prediction: `+1 alarm step`.
- Correct automatic rotation: no additional alarm cost.
- Node visits/key comparisons incurred by insertion follow the common alarm rules if those events are exposed as player-facing gameplay steps.
- `optimalSteps` is the minimum required sequence of insertion interactions and any player-facing visits/comparisons charged by the chosen implementation. The system's automatic balancing work does not count as player input.
- A wrong prediction does not receive the `-50` rotation-choice score penalty because the player did not perform an incorrect manual rotation.

### Implementation guidance

- The player predicts before insertion; do not reveal the actual rotation before the prediction is submitted.
- The AVL model, not the GUI, decides which rotation is required.
- The GUI displays the actual rotation through model events.
- Use the same AVL insertion function as the data-structure implementation and tests.
- Do not create a separate simplified balancing algorithm just for the level.

---

## 9. Level 3.5 — Boss: Order-Statistic Heist

### Objective

Repair the supplied unbalanced BST into a valid AVL tree, then steal the k-th largest treasure using subtree-size information.

### Learning concepts

- AVL repair / rotations
- `subtreeSize`
- `kth`
- `rank` as a required AVL API operation
- Order-statistic selection in `O(log n)` on a valid AVL tree

### Core design decision

The level has two consecutive phases:

1. **Balance the tree:** the player manually repairs the supplied unbalanced BST using the appropriate AVL rotations.
2. **Find the treasure:** after the tree is a valid AVL tree, the player follows the k-th-largest search path. The system uses stored subtree sizes to determine the correct direction.

There is no separate subtree-size calculation or comparison minigame. The system performs those calculations.

### Phase A — Balance the tree

1. The level briefing explains that the vault is unbalanced and must first be repaired.
2. The game displays the starting BST and its balance-factor information.
3. The player identifies the imbalance and performs the required manual AVL rotation(s), using the same interaction style learned in Levels 3.2 and 3.3.
4. The AVL model performs each accepted rotation.
5. After every rotation, the model updates heights and subtree sizes.
6. The boss validator checks the stored subtree sizes after every rotation, as required by the Deliverable.
7. If a size is wrong, the validator flags the affected node. The player should not manually edit the size; the implementation must correct its update logic and the level should not proceed while the AVL metadata is invalid.
8. Continue until the tree is a valid AVL tree.

**Important terminology:** Before repair, call the starting structure an **unbalanced BST** or **BST to be repaired**, not a valid AVL tree. It becomes an AVL tree only after successful balancing.

### Phase B — Find the k-th largest treasure

1. Once the tree passes AVL validation, the boss provides a target rank `k`, expressed as k-th largest.
2. The game converts it internally to the equivalent k-th smallest rank:

   `convertedRank = nodeCount - k + 1`

3. The player starts at the root.
4. At each visited node, the system reads the stored size of the left subtree and determines the node's rank within the current subtree.
5. The system indicates the correct direction:
   - If the target rank is within the left subtree, go left.
   - If it equals the current node's rank, the current node is the target.
   - If it is greater, go right after adjusting the rank to account for the left subtree and current node.
6. The player follows the indicated path by selecting the next node.
7. When the target node is reached, the player selects/confirms the treasure.
8. The system verifies that the selected node is the requested k-th largest value.

The system performs all subtree-size arithmetic. The player follows the resulting path; the player is not asked to count descendants or compare subtree sizes manually.

### Example: Finding the 3rd largest

Suppose the balanced tree contains these values:

```text
             40
           /    \
         20      60
        /  \    /  \
      10   30  50   70
```

The sorted values are:

`10, 20, 30, 40, 50, 60, 70`

The mission asks for the 3rd largest. There are 7 nodes:

`convertedRank = 7 - 3 + 1 = 5`

So the model calls `kth(5)` because the existing `kth()` API returns the 5th smallest.

The system uses subtree sizes to guide the path:

- At 40, the left subtree has size 3, so 40 is the 4th smallest. The 5th smallest is to the right.
- At 60, within that right subtree, the left subtree has size 1. The target is the 1st smallest in that subtree, so the path goes left.
- The target is 50, which is the 3rd largest in the full tree.

### Success condition

- The supplied BST has been repaired into a valid AVL tree.
- BST ordering is preserved.
- Every stored height and subtree size is correct.
- The boss validator reports no invalid subtree sizes after rotations.
- The player reaches and selects the correct k-th largest treasure.

### Alarm and optimal steps

- Incorrect manual rotation: `+2 alarm steps` and `-50 score`.
- Correct rotations: no additional mistake penalty.
- System height/size updates and subtree-size comparisons: `0 alarm steps`.
- Player-facing visits and key comparisons during the search: count according to the common alarm rules.
- `optimalSteps` consists of the minimum valid player-facing balancing interactions plus the minimum search-path visits/comparisons under the actual interaction model.
- Do not charge alarm for the internal conversion from k-th largest to k-th smallest.

### Complexity requirement

The order-statistic selection phase must be `O(log n)` after the tree is a valid AVL tree and subtree sizes are correct. The balancing phase is a separate preparation stage. The level's `O(log n)` mission claim refers to finding the k-th treasure in the balanced, augmented AVL tree, not to the entire manual repair phase.

### Implementation guidance

- Reuse the existing AVL rotation and `kth()` implementations.
- Use the stored `subtreeSize` fields; do not traverse and recount an entire subtree at every search step.
- The boss validator recomputes sizes independently for validation, but this validation work is system-side and not charged to the player.
- `rank(key)` must be implemented and unit-tested as part of the required AVL API. The main Level 3.5 mission remains k-th largest; do not add a separate mandatory rank minigame unless the team later chooses to expand the level.
- Reject invalid `k` values (`k < 1` or `k > nodeCount`) safely and clearly.

---

## 10. Event and architecture integration

The existing architecture requires model operations to be independent of the GUI and communicate changes through events.

### 10.1 Suggested Act 3 event use

Use the existing event conventions where possible:

- `VISIT`: a player visits a node; renderer highlights it; alarm system receives the event.
- `COMPARE`: a key comparison occurs; renderer indicates the result.
- `ROTATE`: an AVL rotation occurs; renderer animates the changed links and positions.
- `UPDATE_HEIGHT`: a node's stored height changes.
- `UPDATE_SIZE`: a node's stored subtree size changes.
- `BALANCE_ALERT`: a node is found unbalanced.
- `HIGHLIGHT_SUBTREE`: renderer highlights the relevant subtree or search direction.
- `DONE`: operation batch is complete.

Event names must be standardized lowercase `snake_case` string identifiers in code, consistent with the Coding Guardrails. The names above reflect the Deliverable's event concepts; coordinate the exact string spelling with the existing event system before implementation.

### 10.2 Layer responsibilities

| Layer | Act 3 responsibility |
|---|---|
| GUI / SFML | Display tree, badges, prompts, choices, alarm, score; collect clicks/drags |
| Game logic | Enforce level rules, validate player choices, manage predictions and win conditions |
| AVL model | Insert, rotate, rebalance, update height/size, `kth`, `rank`, validate AVL metadata |
| Event manager | Record model events for animation, undo, and replay |
| Renderer / animation | Animate visits, rotations, badge updates, and search-path highlights |
| Score system | Apply alarm, penalties, stars, and result calculations |

The GUI must not directly change AVL pointers or calculate the data structure result. It submits the player's action to game logic.

---

## 11. Validation and testing requirements

### 11.1 AVL invariants to validate

After every relevant operation in debug mode and in unit tests, verify:

- BST ordering: every left-subtree key is smaller and every right-subtree key is larger.
- Stored height equals recomputed height.
- Stored subtree size equals recomputed subtree size.
- Every node's balance factor is in `[-1, +1]` after a completed AVL operation.
- Parent pointers are consistent if parent pointers are used.
- There are no cycles or shared nodes.
- The traversal count equals the expected node count.

### 11.2 Level-specific tests

**Level 3.1**
- Tree with no unbalanced nodes.
- Tree with one unbalanced node.
- Tree with multiple unbalanced nodes.
- Correct selection, wrong selection, and repeated selection.

**Level 3.2**
- LL case uses right rotation.
- RR case uses left rotation.
- Wrong pivot / wrong rotation is rejected and penalized.
- BST ordering, heights, and sizes remain valid after a correct rotation.

**Level 3.3**
- LR case performs left rotation on the left child, then right rotation on the pivot.
- RL case performs right rotation on the right child, then left rotation on the pivot.
- Wrong case identification is rejected and penalized.
- Final tree passes all AVL checks.

**Level 3.4**
- Insert sequence causes no rotation.
- Insert sequence causes LL, RR, LR, and RL respectively.
- Correct and incorrect predictions are scored correctly.
- The model's actual automatic rotation matches the validator.

**Level 3.5**
- Subtree sizes are correct in a balanced tree.
- Subtree sizes update correctly after each rotation.
- k-th largest conversion is correct for several values of `n` and `k`.
- Invalid `k` values are handled safely.
- Search reaches the correct treasure.
- The order-statistic search follows an AVL path and uses stored sizes.
- `rank(key)` returns the number of keys less than or equal to the key, as specified in the Deliverable.

---

## 12. Recommended decisions and rationale

These decisions are included so the team can implement a coherent version without reopening every small design question.

| Topic | Decision | Why |
|---|---|---|
| Wrong node selection in 3.1 | +1 alarm, no score deduction | Matches the Deliverable's wrong traversal-click penalty |
| Wrong rotation in 3.2 / 3.3 / 3.5 | +2 alarm and −50 score | Explicit Deliverable rule |
| Wrong LR/RL identification | Treat as wrong rotation choice: +2 alarm and −50 score | The player has selected the wrong repair case |
| Wrong prediction in 3.4 | +1 alarm, no score deduction | User confirmed alarm rises; +1 matches the smaller existing mistake penalty |
| Correct manual rotation | No additional mistake penalty | Rewards correct repair and avoids inventing a cost |
| Incorrect rotation handling | Reject before mutating the tree | Simple and avoids rollback complexity |
| Double rotation interaction | Player identifies the case and initiates the double rotation; model animates its two primitive rotations in order | Preserves the Deliverable's learning objective while keeping interaction manageable |
| Level 3.4 insertion | Player predicts before each insertion; model inserts and automatically rebalances | Matches the Deliverable and the agreed sequence-based flow |
| Level 3.5 starting structure | An unbalanced BST that the player repairs into an AVL tree | Correct terminology and preserves the agreed two-phase boss flow |
| Subtree-size calculations | System-only; no alarm charge | They are internal metadata calculations, not player actions |
| k-th largest | Convert to k-th smallest and reuse `kth()` | Reuses the required API with minimal additional code |
| `rank()` gameplay | Implement and test the API; no separate required minigame in 3.5 | The Deliverable requires the API, while the stated boss mission is k-th treasure |
| Optimal steps | Calculate from the actual level configuration and charged player-facing actions | Avoids arbitrary hard-coded values and keeps alarm fair |

---

## 13. Open questions for team discussion

The core gameplay rules above have been decided for this specification. The remaining questions are limited to configuration and presentation choices that the team should confirm together.

1. **Exact level tree data:** Which exact starting tree values and shapes will be used in Levels 3.1–3.5? The AVL model and game flow can be implemented independently, but the final optimal-step numbers require fixed level configurations.
2. **Level 3.2 packaging:** Should LL and RR appear as two separate short stages within Level 3.2, or as two consecutive repair tasks in one level screen?
3. **Level 3.3 packaging:** Should LR and RL appear as two separate short stages within Level 3.3, or as two consecutive repair tasks in one level screen?
4. **Level 3.4 sequence length:** How many values should the insertion sequence contain? Choose a short enough sequence that the player can predict each insertion without making the level tedious.
5. **Level 3.5 rank range:** Should the boss choose a fixed `k` for each configured level, or should the game select a valid `k` from the tree's node count? Either approach must ensure `1 <= k <= nodeCount`.
6. **Prediction display:** Should the player choose the prediction from five labelled choices, or select a rotation diagram? The underlying accepted values remain `no_rotation`, `ll`, `rr`, `lr`, and `rl`.

These are team configuration/presentation questions, not blockers to implementing the AVL model or the agreed core gameplay.

---

## 14. Implementation checklist

### AVL model
- [ ] AVL node stores key, child pointers, height, and subtree size.
- [ ] Balance factor is calculated consistently.
- [ ] LL, RR, LR, and RL rotations are implemented manually.
- [ ] Heights are updated after insertions and rotations.
- [ ] Subtree sizes are updated after insertions and rotations.
- [ ] `kth(k)` returns k-th smallest using stored subtree sizes.
- [ ] `rank(key)` returns the number of keys `<= key`.
- [ ] AVL validator checks ordering, height, size, and balance.

### Game logic
- [ ] Level 3.1 validates direct clicks on unbalanced nodes.
- [ ] Levels 3.2 and 3.3 validate manual rotation choices.
- [ ] Level 3.4 records predictions before insertion and compares them with actual model results.
- [ ] Level 3.5 separates balancing and order-statistic search into two phases.
- [ ] k-th largest is converted using `nodeCount - k + 1`.
- [ ] Alarm and score penalties follow the tables in this document.
- [ ] Optimal steps are calculated from the configured level and the same event accounting used during play.

### GUI and events
- [ ] Balance-factor badges use green for `-1`, `0`, `+1`, and red otherwise.
- [ ] Correct and incorrect player selections are clearly indicated.
- [ ] Manual rotations are animated through model events.
- [ ] Automatic Level 3.4 rotations are animated through model events.
- [ ] Level 3.5 subtree-size information is visible as system metadata if useful, but the player is not asked to calculate it.
- [ ] The k-th-largest search path is highlighted step by step.
- [ ] Undo/replay uses the existing event/snapshot architecture.

### Integration
- [ ] No SFML dependency is added to the AVL model.
- [ ] No STL containers or enums are introduced.
- [ ] Shared event names and interfaces are agreed before implementation.
- [ ] Unit tests pass before integrating Act 3 screens.

---

## 15. Final agreed Act 3 flow

```text
ACT 3 — THE ARCHITECT'S WING
        |
        v
3.1 READ THE BADGES
Player clicks every unbalanced node
        |
        v
3.2 SINGLE ROTATION REPAIR
Player repairs LL / RR using one rotation
        |
        v
3.3 DOUBLE ROTATION REPAIR
Player identifies LR / RL and initiates the double rotation
        |
        v
3.4 AUTO MODE AND PREDICTION
Player predicts before each insertion
System inserts and automatically rebalances
        |
        v
3.5 BOSS: ORDER-STATISTIC HEIST
Phase A: Player repairs an unbalanced BST into a valid AVL tree
System updates and validates height and subtreeSize
        |
        v
Phase B: Player follows the system-guided order-statistic path
System converts k-th largest and uses subtreeSize
        |
        v
TREASURE FOUND — ACT 3 COMPLETE
```

**Central design principle:** The player makes the learning decisions; the AVL model performs the data-structure algorithms; the event system communicates the results to the GUI. This keeps Act 3 educational, consistent with the earlier levels, and straightforward to implement.
