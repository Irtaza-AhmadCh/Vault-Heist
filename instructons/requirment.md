# Vault Heist – Software Requirements Specification

## 1. Project Overview

**Project Name:** Vault Heist: The Living Vault  
**Project Type:** GUI-Based Puzzle Strategy Game  
**Core Concepts:** Binary Trees, BST, AVL Trees, Max Heap and Heap Sort.

Vault Heist is a single-player puzzle-strategy game in which players explore a vault represented by a tree structure. Players complete missions by performing data structure operations while managing an alarm meter.

The game teaches data structures through interactive challenges, visualizations and animations.

The project consists of four campaign acts, each focusing on a different data structure.

## 2. Project Objectives

The system must:

- Implement Binary Trees, BSTs, AVL Trees and Max Heaps from scratch.
- Provide an interactive graphical user interface.
- Visualize data structure operations through step-by-step animations.
- Support undo and replay functionality.
- Include a four-act campaign with 20 levels.
- Include Endless Mode and Sandbox Mode.
- Implement scoring, alarms, penalties and level progression.
- Support saving and loading game progress.
- Provide unit testing and performance benchmarking.

## 3. Functional Requirements

### FR-01: Main Menu and Game Navigation

The system shall provide a main menu with the following options:

- Campaign Mode
- Endless Mode
- Sandbox Mode
- Leaderboard
- Settings
- Quit

The campaign shall follow this flow:

Main Menu → Campaign Map → Level Briefing → Level Play → Result Screen → Next Level.

### FR-02: Campaign Mode

The campaign shall contain four acts and 20 levels.

| Act | Name | Data Structure | Levels |
|---|---|---|---|
| Act 1 | The Blueprint Wing | Binary Tree | 1.1–1.5 |
| Act 2 | The Sorted Wing | BST | 2.1–2.5 |
| Act 3 | The Architect's Wing | AVL Tree | 3.1–3.5 |
| Act 4 | The Heap Fortress | Max Heap | 4.1–4.5 |

The system shall:

- Display available and locked levels.
- Track completed levels and earned stars.
- Provide level objectives and instructions.
- Allow failed levels to be retried.
- Unlock subsequent levels upon successful completion.

### FR-03: Alarm and Scoring System

The system shall maintain an alarm meter during gameplay.

**Alarm Rules:**

- Visiting a tree node costs one alarm step.
- Comparing a key against a node costs one alarm step.
- Incorrect actions may generate additional alarm steps.
- Each level shall have an alarm capacity calculated as:

`ceil(1.5 × optimalSteps) + 2`

- The level shall fail when the alarm capacity is exhausted.

**Scoring Rules:**

| Condition | Stars |
|---|---|
| Steps ≤ optimal, no penalties | 3 |
| Steps ≤ 1.25 × optimal, at most 1 penalty | 2 |
| Completed within alarm capacity | 1 |
| Alarm capacity exhausted | 0 |

Score calculation:

`Score = max(0, 1000 - 10 × alarmSteps - penalties + bonuses)`

**Penalties:**

- Wrong traversal selection: +1 alarm.
- Wrong rotation choice: +2 alarm and -50 points.
- Using a hint: Maximum star rating reduced by one.
- Invalid heap comparison or swap: +1 alarm.

### FR-04: Act 1 – Binary Tree

The system shall implement a general binary tree where nodes contain arbitrary values without ordering restrictions.

Required operations:

- Preorder traversal.
- Inorder traversal.
- Postorder traversal.
- Level-order traversal.
- Tree reconstruction from preorder and inorder.
- Tree reconstruction from postorder and inorder.
- Tree height calculation.
- Tree diameter calculation.
- Lowest Common Ancestor (LCA).
- Path between two nodes.
- Tree inversion (mirror).
- Structural comparison and symmetry checking.

**Levels:**

| Level | Requirement |
|---|---|
| 1.1 | Predict and perform preorder, inorder, postorder and level-order traversals. |
| 1.2 | Reconstruct a tree from traversal sequences. |
| 1.3 | Calculate node heights and tree diameter. |
| 1.4 | Find LCA and path length between two nodes. |
| 1.5 | Mirror the tree by swapping left and right children. |

The player shall interact with the tree through clicking, selecting and dragging nodes.

### FR-05: Act 2 – Binary Search Tree

The system shall implement a Binary Search Tree.

**BST Rules:**

- Smaller keys go to the left.
- Larger keys go to the right.
- Duplicate keys must be rejected.

Required operations:

- Insert.
- Search.
- Delete.
- Find minimum and maximum.
- Successor and predecessor.
- Range query.
- BST Lowest Common Ancestor.
- Height and degeneration analysis.

**Deletion Cases:**

1. Delete a leaf node.
2. Delete a node with one child.
3. Delete a node with two children using its inorder successor.

**Levels:**

| Level | Requirement |
|---|---|
| 2.1 | Search for a target key by navigating through the BST. |
| 2.2 | Insert a new treasure at the correct position. |
| 2.3 | Delete nodes using all three deletion cases. |
| 2.4 | Perform range queries and visualize skipped subtrees. |
| 2.5 | Handle a degenerated BST created through sorted insertions. |

### FR-06: Act 3 – AVL Tree

The system shall implement an AVL Tree with automatic balancing.

Required functionality:

- AVL insertion and deletion.
- Height calculation and updates.
- Balance factor calculation.
- Automatic rebalancing.
- Left and right rotations.
- Subtree size maintenance.
- Order-statistic operations.

**Balance Factor:**

`BF(node) = height(left) - height(right)`

The balance factor of every node must remain between -1 and +1.

**Required Rotations:**

- LL Rotation.
- RR Rotation.
- LR Rotation.
- RL Rotation.

**Order Statistics:**

- `subtreeSize`
- `kth(k)`
- `rank(key)`
- `countInRange(lo, hi)`

The kth operation must use subtree sizes to achieve O(log n) time.

**Levels:**

| Level | Requirement |
|---|---|
| 3.1 | Identify unbalanced nodes using balance factors. |
| 3.2 | Perform manual LL and RR rotations. |
| 3.3 | Identify and perform LR and RL rotations. |
| 3.4 | Automatically rebalance after insertion and predict rotations. |
| 3.5 | Use subtree sizes to perform kth and rank operations. |

### FR-07: Endless Survival Mode

The system shall provide an endless mode involving mixed insertion and deletion operations.

Requirements:

- Generate a continuous stream of operations.
- Maintain a valid AVL Tree.
- Increase difficulty over time.
- Track survival time and score.
- End the session when the player fails to maintain the required tree properties or loses according to the game rules.

### FR-08: Act 4 – Max Heap and Heap Sort

The system shall implement an array-based Max Heap.

**Required Operations:**

- Build Max Heap.
- Sift Up.
- Sift Down.
- Heapify.
- Insert.
- Extract Maximum.
- Heap Sort.
- Max Heap validation.

For a zero-based array:

- Parent: `(i - 1) / 2`
- Left Child: `2 * i + 1`
- Right Child: `2 * i + 2`

**Levels:**

| Level | Requirement |
|---|---|
| 4.1 | Build a Max Heap using bottom-up heap construction. |
| 4.2 | Repair a disturbed heap using heapify, sift-up and sift-down. |
| 4.3 | Repeatedly extract the maximum and restore the heap. |
| 4.4 | Complete Heap Sort under an alarm and operation limit. |
| 4.5 | Independently sort a new array in ascending order. |

**Heap Visualization Requirements:**

The system shall display two synchronized representations:

1. Heap Tree View.
2. Array View.

Both views must update whenever a swap occurs.

The array view shall distinguish between:

- Active heap elements.
- Extracted and sorted elements.

### FR-09: Sandbox Mode

The system shall provide a free-form editor where users can experiment with data structures.

Required functionality:

- Switch between Binary Tree, BST, AVL Tree and Max Heap.
- Insert and delete nodes.
- Search for values.
- Perform traversals.
- Perform rotations.
- Execute heap operations.
- Generate random structures.
- Visualize operations through animations.

Sandbox operations shall not require campaign progression.

### FR-10: Event System

The system shall use an event-driven architecture to separate data structures from the GUI.

Every major operation shall generate events describing what happened.

**Required Events:**

- VISIT
- COMPARE
- INSERT
- UNLINK
- SWAP_VALUE
- ROTATE
- UPDATE_HEIGHT
- UPDATE_SIZE
- BALANCE_ALERT
- HIGHLIGHT_SUBTREE
- HEAP_BUILD
- HEAP_SWAP
- HEAPIFY_START
- HEAPIFY_DONE
- EXTRACT_MAX
- MARK_SORTED
- HEAP_SIZE_CHANGE
- HEAP_SORT_COMPLETE
- DONE

Each event shall contain the information needed to identify the affected nodes or array positions and visualize the operation.

### FR-11: Animation and Visualization

The system shall visualize data structure operations step by step.

Required functionality:

- Animate node insertion and deletion.
- Animate AVL rotations.
- Highlight visited and compared nodes.
- Display balance factors and subtree sizes.
- Highlight skipped subtrees during range queries.
- Animate heap swaps and extraction.
- Synchronize heap tree and array views.
- Support animation speed adjustment.
- Support pause, resume and step-by-step playback.

### FR-12: Undo and Replay

The system shall support undo and replay for player operations.

Requirements:

- Undo the previous operation.
- Replay previously recorded operations.
- Support stepping through individual events.
- Provide pause and resume controls.
- Maintain consistent tree and heap states after undo.

The initial implementation may use serialized state snapshots before each action. Inverse events may be implemented as an improvement.

### FR-13: Graphical User Interface

The system shall provide the following screens:

| Screen | Requirements |
|---|---|
| Main Menu | Campaign, Endless, Sandbox, Leaderboard, Settings and Quit. |
| Campaign Map | Four acts, level progression and earned stars. |
| Level Screen | Tree/heap visualization, objectives, alarm and score. |
| Result Screen | Stars, score, steps, penalties and progression. |
| Sandbox | Data structure editor and operation controls. |
| Leaderboard | Locally stored player scores. |

**Shared GUI Components:**

- Tree/Heap Canvas.
- HUD (alarm, score and objectives).
- Event Log.
- Operation Panel.
- Toolbar.
- Instruction Panel.

**User Controls:**

| Input | Action |
|---|---|
| Mouse Click | Select nodes or perform actions. |
| Mouse Drag | Move tokens or perform rotations. |
| Mouse Wheel | Zoom. |
| Background Drag | Pan the view. |
| Space | Play/Pause animation. |
| Left/Right | Previous/Next event. |
| Z | Undo. |
| H | Hint. |
| Esc | Return to menu. |

Color shall not be the only indicator of a node's status. Text labels, outlines or icons must also be provided.

### FR-14: Level Generation

The system shall support data-driven level generation.

Requirements:

- Store level definitions in JSON files.
- Use seeded random generation for reproducibility.
- Generate Binary Trees with 6–15 nodes for Act 1.
- Generate BSTs with controlled difficulty for Act 2.
- Include all four AVL rotation cases in Act 3.
- Generate different heap input types for Act 4.
- Calculate optimal steps automatically.
- Allow new levels to be added without modifying core game logic.

### FR-15: Save and Load

The system shall save and restore game progress.

Saved information shall include:

- Campaign progress.
- Earned stars.
- Current Endless Mode state.
- Current tree or heap.
- Random seed.

**Serialization Requirements:**

- Serialize trees using preorder traversal with null markers.
- Serialize heaps using the underlying array and active heap size.
- Recalculate AVL height and subtree size after loading.
- Validate loaded structures before starting the game.

### FR-16: Validation

The system shall provide a Validator to check data structure correctness.

It shall verify:

- BST ordering properties.
- Correct AVL heights.
- Correct subtree sizes.
- Valid AVL balance factors.
- Consistent parent pointers, if used.
- Absence of cycles and shared nodes.
- Valid Max Heap properties.
- Consistency between active heap size and sorted region.

Validation shall run in debug mode after operations and during unit testing.

## 4. Non-Functional Requirements

### NFR-01: Performance

- The GUI should target 60 FPS.
- The system should support visualization of a 200-node tree or large heap without significant frame drops.
- Data structure operations should execute independently of GUI rendering.
- Benchmarking must run without GUI animation and event recording.

### NFR-02: Maintainability

- Each module shall have a clearly defined responsibility.
- Data structures shall remain independent of GUI components.
- Shared visualization components shall be reusable.
- Level configuration shall be data-driven.

### NFR-03: Reliability

- Invalid operations shall be handled safely.
- Data structure properties shall remain valid after operations.
- Undo and replay shall not leave inconsistent states.
- Loaded game data shall be validated.

### NFR-04: Testability

- Data structures and algorithms shall be independently testable.
- Unit tests shall not require the GUI.
- Major operations and edge cases shall be covered.

### NFR-05: Usability

- The GUI shall provide clear instructions.
- Operations shall have visible feedback.
- The alarm meter and objectives shall remain accessible during gameplay.
- Color shall not be the only means of communicating information.

## 5. Testing Requirements

### 5.1 Unit Testing

The system shall include tests for:

**Binary Tree**
- Empty and single-node trees.
- Skewed and full trees.
- Recursive and iterative traversal comparison.
- Reconstruction correctness.

**BST**
- Insertion and duplicate rejection.
- Search.
- All three deletion cases.
- Root deletion and missing keys.
- Range queries.

**AVL Tree**
- LL, RR, LR and RL rotations.
- Insertion and deletion.
- Height and balance factor updates.
- Subtree size updates.
- kth and rank correctness.

**Max Heap**
- Heap construction.
- Heapify.
- Insertion and extraction.
- Duplicate and negative values.
- Heap Sort correctness.

### 5.2 GUI Testing

The system shall be manually tested for:

- Successful and failed level completion.
- Level retries.
- Zoom and pan.
- Animation consistency.
- Undo and replay.
- Heap tree and array synchronization.

## 6. Benchmarking Requirements

The project shall include performance comparisons and complexity analysis.

| Experiment | Input | Metrics |
|---|---|---|
| BST vs AVL Height | n = 100, 1,000, 10,000 | Final tree height. |
| Search Performance | 10,000 random searches | Average comparisons and execution time. |
| AVL Rotations | Random and sorted inputs | Average rotations and maximum rotations per deletion. |
| Order Statistics | kth on AVL vs BST inorder scan | Time for 1,000 queries. |
| Heap Construction | n = 100, 1,000, 10,000 | Build time and O(n) verification. |
| Heap Sort | Random, sorted, reverse-sorted and duplicate-heavy inputs | Execution time, comparisons and swaps. |

The report shall explain:

- BST and AVL performance differences.
- AVL balancing overhead.
- O(n) bottom-up heap construction.
- O(n log n) Heap Sort complexity.
- Heap Sort comparison with other sorting algorithms covered in the course.

## 7. Project Deliverables

The final submission shall include:

1. **Source Code:** Complete functional and executable project.
2. **README.md:** Installation, execution instructions and game controls.
3. **Unit Tests:** Test suite covering major data structures and algorithms.
4. **Test Report:** Results of the unit testing.
5. **Project Report:** 8–12 pages covering:
   - System architecture.
   - Algorithmic complexity analysis.
   - Benchmarking results.
   - GUI screenshots.
   - Screenshots of all four AVL rotation types.
   - Heap construction, extraction, heapify and final sorted result.
6. **Level Data:** JSON level configuration files.
7. **Level Guide:** Instructions for adding new levels.

## 8. Project Constraints

- All major data structures and algorithms must be implemented from scratch.
- Built-in tree, BST, AVL and priority queue implementations are prohibited.
- Code copying from other groups is prohibited.
- A group may contain up to two members.
- Both members must understand the complete implementation.
- Both members must attend the project demonstration and viva.
- The application must be functional and executable.

## 9. Submission Requirements

- Submit through Google Classroom.
- Submit all project files in a single ZIP archive.
- ZIP naming convention: `25I-XXXX_25I-XXXX_Section.zip`.
- Only one group member submits on behalf of the group.
- Verify the ZIP file for corruption, missing files and submission errors.
- Late submissions will not be accepted.

**Project Deadline:** 20 November 2026, 11:59 PM.

## 10. Acceptance Criteria

The project shall be considered complete when:

- All four data structures work correctly.
- All 20 campaign levels are implemented.
- Endless and Sandbox modes are functional.
- Required algorithms and operations are implemented.
- GUI visualizations and animations work correctly.
- Undo and replay work without corrupting the data structures.
- Heap tree and array views remain synchronized.
- Save and load functionality works.
- Unit tests pass for the required cases.
- Benchmarking results are available.
- All required documentation and submission files are prepared.

---

**End of Requirements Document**