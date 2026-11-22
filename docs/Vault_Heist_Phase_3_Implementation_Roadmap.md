# Vault Heist: The Living Vault

# Phase 3 --- GUI Repair, Visualization and Implementation Roadmap

**Document type:** AI coding-agent implementation specification\
**Phase:** Phase 3 --- GUI and Visualization\
**Estimated duration in the team roadmap:** 4--6 working days\
**Framework:** SFML\
**Primary objective:** Repair and complete the existing
GUI/visualization implementation so that all four data structures can be
operated and visualized through the agreed event-driven architecture.

------------------------------------------------------------------------

## 0. Mandatory instructions to the coding agent

You are working on **Vault Heist: The Living Vault**, an existing C++
project. The project already has core data structures from Phases 1 and
2, and a GUI has already been attempted. The current GUI is not
functioning correctly.

Your task is **not** to create a different game, replace the project
architecture, or redesign the supplied visual theme. Your task is to
inspect the current code, identify why the existing GUI is failing,
repair it, and complete the Phase 3 scope described in this document.

### 0.1 Source-of-truth priority

The following project files are authoritative and must be followed
together:

1.  `archtecture.md` --- layered architecture, module responsibilities,
    and event-driven communication.
2.  `gardrails.md` --- mandatory C++ coding, file organization, SFML,
    STL, enum, and implementation restrictions.
3.  `requirment.md` --- functional and non-functional requirements,
    event names, visualization requirements, controls, and testing
    expectations.
4.  `workdivision.md` --- exact Phase 3 work division, integration
    milestone, and phase boundaries.
5.  `vault_heist_bio_organic_theme_guide.md` --- visual art direction
    and theme.

The existing repository is also authoritative for its already-agreed
names, folder structure, public interfaces, and working implementations.
Preserve those decisions unless they directly prevent compliance with
the source documents.

**Do not silently reinterpret, weaken, or contradict any requirement in
these files.** If two source statements appear inconsistent or the
current code has made a decision that conflicts with them, stop and
report the exact conflict with file/module references. Do not invent a
new architecture to resolve it without approval.

### 0.2 Required behavior before editing

Before making code changes:

1.  Inspect the complete repository and identify the actual entry point,
    build configuration, source folders, headers, implementation files,
    data-structure APIs, GUI classes, event classes, and existing
    assets.
2.  Build and run the current project using its existing build process.
3.  Record the exact compiler errors, runtime errors, rendering
    problems, input problems, and broken interactions.
4.  Trace each visible GUI operation from input to game logic, data
    structure operation, event recording, animation, and rendering.
5.  Identify which existing components work and which do not.
6.  Create a repair plan that changes the smallest possible amount of
    existing code.
7.  Preserve working Phase 1 and Phase 2 data-structure implementations.
    Do not rewrite them merely to make GUI integration easier.

Do not assume the existing GUI is disposable. Diagnose it first. Prefer
repairing and integrating existing components over replacing them.

### 0.3 Hard coding guardrails

These rules are mandatory:

-   Use **C++ and SFML** for the GUI.
-   Keep data structures independent of SFML and GUI code.
-   Do not implement data-structure algorithms inside GUI classes.
-   Do not use enums. Use centrally defined, standardized lowercase
    `snake_case` string literals.
-   Do not use STL containers. In particular, do not use `std::vector`,
    `std::queue`, `std::stack`, `std::map`, or other STL containers.
-   Do not replace the required manually implemented structures with
    built-in containers or algorithms.
-   Use structs for data models and nodes; use classes to organize
    operations and component functionality.
-   Keep declarations in `.h` files and implementations in `.cpp` files
    by default.
-   Use include guards or `#pragma once`.
-   Avoid circular dependencies, unnecessary inheritance, templates,
    global variables, operator overloading, try-catch blocks, and
    unnecessary abstractions.
-   Do not add libraries or dependencies without approval.
-   Do not change shared interfaces or function signatures without
    coordinating with the relevant owner.
-   Keep code straightforward, readable, and explainable in a student
    demonstration and viva.
-   Avoid unnecessary comments. Prefer meaningful names and clear code.
-   Preserve the existing folder structure and naming conventions.
-   Test components independently before integration.

If the existing code violates a guardrail, report the violation and
propose the smallest compliant correction. Do not quietly add more
violations.

------------------------------------------------------------------------

## 1. Phase 3 purpose and boundaries

Phase 3 is **GUI and Visualization**. The core data structures are
assumed to have been implemented in Phases 1 and 2. Phase 3 connects
those structures to a working SFML visualization and interaction layer.

The Phase 3 team roadmap estimates **4--6 working days**. This is an
estimate, not permission to skip requirements or tests.

### 1.1 Phase 3 outcome

At the end of Phase 3:

-   Binary Tree and BST operations can be visualized in the existing
    application.
-   AVL nodes display balance-factor and subtree-size information, and
    all four rotation cases can be visualized.
-   Max Heap is shown in both tree and array form, and both views remain
    synchronized.
-   Operations are communicated through the agreed event-driven
    architecture.
-   Animation can be queued, played, paused, resumed, and stepped
    through.
-   Act 1, Act 2, Act 3, and Act 4 gameplay/visualization screens are
    integrated.
-   GUI actions call the existing game/data-structure interfaces rather
    than implementing algorithms in the GUI.
-   Both members' visualizations run in the same application and use the
    same agreed event architecture.
-   GUI operations correctly update the underlying models.

### 1.2 Exact Phase 3 work division

Follow the assignment in `workdivision.md`.

  Member A --- Tree Systems               Member B --- AVL, Heap and Game Interface
  --------------------------------------- ---------------------------------------------
  Tree LayoutEngine                       Event animation controller
  Tree node and edge rendering            Animation queue, pause and step
  Tree selection and highlighting         AVL rotation animations
  Traversal animations                    Balance-factor and subtree-size badges
  BST insertion and deletion animations   Heap tree visualization
  Search-path visualization               Heap array visualization
  Tree operation panel                    Synchronization between heap tree and array
  Act 1 and Act 2 screens                 Act 3 and Act 4 screens

Both members must agree on interfaces and event formats before
integration. Work in parallel; do not make one member wait for the other
to finish all modules.

### 1.3 Explicitly outside Phase 3

Do not expand this task into the complete game systems phase. According
to `workdivision.md`, the following are Phase 4 shared work:

-   Main menu and campaign map.
-   Full LevelManager.
-   Full ScoreSystem and alarm meter.
-   Full EventLog/game-history integration.
-   Full undo and replay integration.
-   Save and load.
-   Sandbox mode.
-   Endless mode.
-   Final campaign integration and progression systems.

Phase 3 still needs an event stream/log interface sufficient for the
animation controller and current operation feedback. This does **not**
authorize implementing the complete Phase 4 EventLog, undo/replay,
campaign progression, or save/load systems in Phase 3.

The SRS describes the final application screens and controls. Implement
only the Phase 3 gameplay/visualization interfaces required for Acts
1--4 at this stage. Do not claim the complete main menu, campaign,
result, sandbox, leaderboard, save/load, or progression requirements are
complete unless they already exist and are explicitly being maintained
as existing functionality.

------------------------------------------------------------------------

## 2. Required architecture and data flow

### 2.1 Preserve the layered architecture

The architecture document requires separate modules for:

-   `structures` --- Binary Tree, BST, AVL Tree, and Max Heap.
-   `game` --- game logic and coordination.
-   `events` --- records data-structure operations for animation, undo,
    and replay.
-   `gui` --- screens and visual representation.
-   `utils` --- saving, loading, and validation.
-   `tests` --- unit tests.
-   `levels` --- level configurations and game data.

Do not move data-structure algorithms into the GUI. Keep existing folder
names if the repository already uses an agreed equivalent structure.

### 2.2 Communication flow

Preserve the architecture and the agreed event contract. The high-level
interaction must remain:

``` text
User Input
    |
    v
GUI / Operation Panel
    |
    v
Game Logic / GameEngine
    |
    v
Existing Data Structure
    |
    v
Event Manager / Agreed Event Log
    |
    v
Animation Controller
    |
    v
Renderer
    |
    v
Visible Animation
```

The architecture document describes the flow as GUI → GameEngine → Data
Structures → EventManager → Renderer. The team roadmap further specifies
the visualization pipeline as model → event log → animation controller →
renderer. Treat these as layers of the same agreed event-driven flow,
not as permission to add duplicate event systems.

**Important:** Data structures must not directly call SFML, draw nodes,
access mouse input, or depend on GUI classes. The GUI must not directly
alter internal node pointers or perform insert/delete/rotation/heap
algorithms.

### 2.3 Operation lifecycle

For every supported GUI operation:

1.  The user selects an operation and supplies the required input.
2.  The GUI validates only the input format and forwards the request to
    the game logic.
3.  Game logic calls the existing data-structure API.
4.  The operation produces the agreed event records.
5.  The event manager/log makes the events available to the animation
    controller.
6.  The animation controller processes the events in order.
7.  The renderer displays the corresponding visual state.
8.  The final rendered state must agree with the actual data structure
    model.

Do not make the GUI guess what happened by comparing screenshots or
independently re-running the algorithm. The model/event flow is the
source of truth.

### 2.4 Event contract is a prerequisite

Before implementing or repairing animations, inspect the current event
definitions and compare them against the SRS list in Section 5 of this
document.

For every event, document:

-   Exact event name.
-   Meaning.
-   Which operation emits it.
-   Which node IDs or array indices it refers to.
-   Any old/new values needed for the animation.
-   Whether it changes visual state or is informational.
-   Which component consumes it.

Use the existing event representation if it is compliant. If an event
payload is missing required information, propose the smallest extension
and coordinate before changing shared interfaces.

Do not introduce enums. Event names and state names must be centrally
defined lowercase `snake_case` strings. Do not scatter arbitrary
spellings across files.

------------------------------------------------------------------------

## 3. Phase 3 vertical implementation roadmap

Implement in vertical slices. Each slice must work end-to-end before
moving to the next. A feature is not complete merely because a class or
screen exists.

### Slice 0 --- Existing GUI audit and repair plan

**Owner:** Both\
**Dependency:** None\
**Purpose:** Understand why the current GUI is not working before adding
more features.

Tasks:

1.  Inspect project build files and SFML version/configuration already
    in use.
2.  Locate the application entry point and main loop.
3.  Locate window creation, event polling, update, and render functions.
4.  Locate all current screens and screen-switching logic.
5.  Locate all current visualization, layout, input, event, and
    animation components.
6.  Locate fonts, textures, sprites, backgrounds, and other supplied
    visual assets.
7.  Trace every current operation button and mouse/keyboard action to
    its handler.
8.  Check whether the GUI is calling data-structure methods through game
    logic or directly.
9.  Check whether event records are actually produced, retained,
    consumed, and rendered.
10. Check whether animation state and model state become inconsistent.
11. Build and run the application; capture reproducible failures.
12. Write a concise audit report with: issue, reproduction steps, root
    cause, affected files, smallest correction, and regression test.

**Do not** replace the current GUI or theme before this audit. Do not
delete supplied assets. Do not rewrite working model code.

**Exit criteria:**

-   The project builds, or every build failure is documented with a
    concrete cause.
-   The existing GUI's broken paths are identified.
-   Existing working components and interfaces are listed.
-   A minimal repair plan is agreed by both members.
-   No unapproved architecture change is made.

### Slice 1 --- Shared application and event integration foundation

**Owner:** Both; Member B coordinates the animation interface\
**Dependency:** Slice 0 and agreement on the event contract.

Tasks:

1.  Confirm the existing application/window loop works.
2.  Confirm SFML input polling is performed once per frame in the
    appropriate application layer.
3.  Separate input handling, model/game updates, animation updates, and
    drawing.
4.  Confirm that a GUI operation reaches the correct game-logic method.
5.  Confirm that the model operation generates the expected events.
6.  Confirm that the animation controller can receive the event
    sequence.
7.  Confirm that the renderer can access a read-only visual
    representation of the current model state.
8.  Keep model headers free of SFML includes.
9.  Confirm both Member A and Member B can integrate their components
    without duplicate event managers or competing render loops.

A suggested frame responsibility is:

``` text
Poll SFML input
    -> Handle screen and control input
    -> Send requested operation to game logic
    -> Update animation controller
    -> Render current screen and visual state
    -> Display frame
```

Use the existing project structure and class names where possible. This
is a responsibility outline, not a mandate to create a new framework.

**Exit criteria:**

-   The app opens and remains responsive.
-   A test operation can be sent from the GUI to the model through game
    logic.
-   The generated event sequence reaches the animation controller.
-   No GUI dependency has leaked into the data-structure layer.

### Slice 2 --- Binary Tree static visualization

**Owner:** Member A\
**Dependency:** Slice 1 and existing Binary Tree API.

Implement the foundational tree drawing before animations.

#### 2.1 LayoutEngine

The LayoutEngine calculates visual positions for tree nodes. It must not
modify the tree.

Required behavior:

-   Read the existing tree structure.
-   Calculate a position for each visible node.
-   Place the root above its children.
-   Place left and right children in their appropriate relative
    positions.
-   Keep parent-child edges connected to the correct node positions.
-   Avoid node overlap for supported tree sizes.
-   Recalculate positions when the tree structure changes.
-   Support the project's required tree visualization scale, including
    the SRS target of approximately 200 nodes without significant frame
    drops.
-   Keep layout calculations separate from drawing and data-structure
    algorithms.

Use a clear, understandable layout method compatible with the existing
tree representation. Do not add a complex layout framework unless
required.

#### 2.2 Node and edge rendering

Implement or repair the existing node and edge drawing components.

Each node must display its value clearly. Edges must connect the correct
parent and child. A missing child must not be rendered as a real node.

The renderer must distinguish at least:

-   Normal node.
-   Selected node.
-   Visited node.
-   Compared node.
-   Target/found node.
-   Inactive or absent node where a visual placeholder is required.

Color must not be the only status indicator. Use labels, outlines,
icons, or other visible cues as well.

#### 2.3 Initial static test

Render a fixed test tree containing:

-   A root.
-   A left child.
-   A right child.
-   At least one deeper descendant.
-   A missing-child case.
-   A skewed-tree case.

Compare the drawing with the actual model links. Verify that the layout
does not change the model.

**Exit criteria:**

-   The tree renders correctly.
-   Every visible edge connects the correct parent and child.
-   Node values are readable.
-   The model remains unchanged by layout/rendering.
-   Re-layout after a structural change is correct.

### Slice 3 --- Tree interaction and operation panel

**Owner:** Member A\
**Dependency:** Slice 2.

Implement the tree interactions required by the SRS and Phase 3 work
division.

#### 3.1 Node selection

-   Detect mouse clicks within a node's visual bounds.
-   Identify the selected node using a stable model/node identifier
    where available.
-   Show a visible selection outline or other clear status cue.
-   Display the selected node's relevant information in the existing
    interface.
-   Clicking empty canvas space must not accidentally select a node.
-   Selection must not change the data structure.

#### 3.2 Hover and highlighting

-   Provide visible hover feedback.
-   Highlight selected, visited, compared, and target nodes distinctly.
-   Keep status readable without relying only on color.
-   Clear temporary highlights when the relevant operation or animation
    ends, unless the operation's final state requires them to remain.

#### 3.3 View navigation

Implement the SRS controls:

-   Mouse wheel: zoom.
-   Background drag: pan the view.
-   Mouse drag: move tokens or perform the relevant interaction where
    required by the active level.
-   Mouse click: select nodes or perform the action associated with the
    active control.

Do not let dragging a node unintentionally trigger background panning.
Do not let a click become a drag unless movement passes the chosen
interaction threshold.

#### 3.4 Operation panel

The tree operation panel must expose only operations appropriate to the
active structure/act. It should call game logic, not implement
algorithms.

For the Binary Tree and BST acts, provide the controls needed by the
current Act 1 and Act 2 level requirements and existing model APIs.
Inputs must be checked for missing or invalid text before forwarding.

**Exit criteria:**

-   Node selection and hover work.
-   Zoom and pan work without changing model data.
-   Operation controls call the correct model/game methods.
-   Invalid input does not crash the app.
-   The selected node and operation feedback are visible.

### Slice 4 --- First complete event-to-animation vertical slice: traversal

**Owner:** Member A, with Member B's agreed animation-controller
interface\
**Dependency:** Slices 1--3.

This is the first end-to-end proof of the architecture. Complete it
before implementing a large set of animations.

Implement one traversal path from user action to final visual result:

1.  User selects a traversal operation.
2.  GUI forwards the request to game logic.
3.  Binary Tree performs the traversal using the existing
    implementation.
4.  The operation emits the agreed traversal events, including the
    relevant visit/compare/done events where applicable.
5.  The event sequence is made available to the animation controller.
6.  The controller processes events in order.
7.  The renderer highlights the correct node at each step.
8.  The final state and traversal result match the model output.

Then extend the same mechanism to:

-   Preorder.
-   Inorder.
-   Postorder.
-   Level-order.

Do not write a second traversal algorithm in the GUI. The model's
traversal implementation remains authoritative.

**Exit criteria:**

-   All four traversal types can be requested.
-   The displayed visit order matches the model's output.
-   Nodes are highlighted in the correct order.
-   The operation completes and returns the UI to a usable state.
-   Repeated traversals do not reuse stale events or leave stale
    highlights.

### Slice 5 --- BST search, insertion, deletion and range/search-path visualization

**Owner:** Member A\
**Dependency:** Slice 4 and existing BST API.

Implement the Act 2 visual behavior on top of the same tree renderer and
event path.

#### 5.1 Search path

-   Show each visited node in order.
-   Show comparisons as they occur.
-   Distinguish the current comparison from previously visited nodes.
-   Clearly indicate found and not-found outcomes.
-   The visible path must match the actual BST search path.

#### 5.2 Insertion

-   Forward the requested key to the existing BST/game-logic operation.
-   Animate the relevant search/compare path.
-   Animate the insertion event at the correct location.
-   Recalculate the layout after the structural change.
-   Confirm the final rendered BST obeys the actual model's links.

#### 5.3 Deletion

The SRS requires the three deletion cases:

1.  Leaf node.
2.  Node with one child.
3.  Node with two children using its inorder successor.

Visualize the events supplied by the model. The GUI must not implement
deletion or choose a different successor independently.

After deletion:

-   Update the visible links.
-   Remove or mark the deleted node as specified by the event sequence.
-   Recalculate layout.
-   Confirm the final tree matches the model.

#### 5.4 Range query and skipped subtrees

The SRS requires skipped subtrees to be visualized. Use the agreed
`HIGHLIGHT_SUBTREE` event where applicable. The visual state must
clearly distinguish visited/considered nodes from skipped subtrees.

**Exit criteria:**

-   Search path is accurate.
-   Insert animation ends with the correct BST.
-   All three deletion cases render correctly.
-   Layout updates after insertion and deletion.
-   Range-query skipped subtrees are visually identifiable.
-   Duplicate-key rejection and missing-key behavior are displayed
    safely according to the existing BST API.

### Slice 6 --- Animation Controller and playback controls

**Owner:** Member B\
**Dependency:** Slice 1 event contract; can be developed in parallel
with Member A's tree work using agreed test events.

The animation controller consumes events. It must not perform
data-structure algorithms.

#### 6.1 Animation queue

The project prohibits STL containers. Therefore, do not use
`std::queue`, `std::vector`, or another STL container for the animation
queue.

Use a simple manually managed array or another already-approved project
structure. Keep capacity, current position, and event count handling
explicit. Handle empty queues and capacity limits safely.

The queue must:

-   Preserve event order.
-   Start a new operation without accidentally replaying old events.
-   Avoid reading beyond its current event count.
-   Handle empty event sequences.
-   Provide a clear completion state.
-   Release any dynamically allocated memory correctly if dynamic
    storage is used.

#### 6.2 Playback states

Use standardized string literals, not enums. Centralize accepted state
values. The SRS requires pause, resume, step-by-step playback, and speed
adjustment.

At minimum, support the states needed by the current controller, such
as:

-   `idle`
-   `playing`
-   `paused`
-   `completed`

Do not introduce multiple inconsistent spellings.

#### 6.3 Playback behavior

-   Play processes events in order.
-   Pause stops progression without losing the current event position.
-   Resume continues from the paused position.
-   Step advances one event at a time.
-   Speed adjustment changes the duration between animation steps.
-   Completion stops playback and reports that the event sequence is
    finished.
-   A new operation resets or replaces the previous playback sequence
    according to the agreed operation policy.

Use elapsed frame time/delta time from the SFML update loop rather than
blocking sleeps or loops that freeze the window.

#### 6.4 Required user controls

The SRS specifies:

  Input   Required action
  ------- ----------------------
  Space   Play/Pause animation
  Left    Previous event
  Right   Next event
  Z       Undo
  H       Hint
  Esc     Return to menu

For Phase 3, implement the animation playback controls that belong to
the controller: Space, Left/Right event stepping, and speed adjustment.
`Z`, `H`, and `Esc` are final SRS controls, but full undo, hint/game
logic, and menu navigation belong to the broader game systems. Preserve
existing handlers if already implemented; do not fake full undo or hint
functionality in the animation controller.

**Important:** The final SRS says Left means previous event. Phase 3
must not claim true reverse playback/undo unless the event system
provides a safe previous-state mechanism. If only forward stepping is
implemented at this stage, document the limitation and leave the full
undo/replay behavior to Phase 4.

**Exit criteria:**

-   Queue preserves order and handles boundaries.
-   Play, pause, resume, step, and speed adjustment work.
-   The application remains responsive during playback.
-   No STL container or enum has been introduced.
-   A finished queue cannot accidentally replay stale events.

### Slice 7 --- AVL visualization and rotation animations

**Owner:** Member B\
**Dependency:** Slice 1, Slice 2's reusable tree rendering principles,
existing AVL API, and animation controller.

The AVL view reuses tree visualization concepts. Do not create a
separate, conflicting tree-rendering architecture.

#### 7.1 AVL node information

Display:

-   Node key/value.
-   Height.
-   Balance factor.
-   Subtree size.

The SRS defines:

``` text
BF(node) = height(left) - height(right)
```

Every AVL node must maintain a balance factor between -1 and +1 after a
completed valid AVL operation.

Display balance factor and subtree size as readable badges or labels.
These values must come from the actual AVL model or a safe read-only
query, not from GUI-side recomputation that could disagree with the
model.

#### 7.2 Balance alerts

Use the agreed `BALANCE_ALERT` event to identify the node that requires
attention. Highlight the affected node and make its balance information
visible.

#### 7.3 Rotation animations

Visualize all four required cases:

-   LL.
-   RR.
-   LR.
-   RL.

Use the agreed `ROTATE` event(s) and any required supporting events,
such as height/size updates. Inspect the existing event payload to
determine whether a rotation is represented as one event or multiple
steps. Do not invent a different event contract independently.

For each rotation:

1.  Identify the affected node(s).
2.  Highlight the relevant subtree.
3.  Show the structural transition using the event sequence.
4.  Update node positions and connecting edges.
5.  Display updated height, balance factor, and subtree size.
6.  Verify that the final visual structure matches the AVL model.

Do not rotate pointers or modify AVL nodes in the GUI. The AVL
implementation remains responsible for rotations.

#### 7.4 AVL insertion/deletion visualization

Where the existing Act 3 gameplay operations invoke AVL
insertion/deletion, display the sequence of comparisons, updates,
balance alerts, rotations, and completion events supplied by the model.

**Exit criteria:**

-   Each node displays key, height, balance factor, and subtree size.
-   All four rotation cases are visually distinguishable and correct.
-   Updated metadata matches the AVL model.
-   Final tree obeys AVL invariants.
-   Animation does not change the underlying AVL structure.

### Slice 8 --- Max Heap tree view, array view and synchronization

**Owner:** Member B\
**Dependency:** Slice 1 event contract, animation controller, existing
MaxHeap API.

The Max Heap is array-based in the model. The GUI must show two
synchronized representations:

1.  Heap Tree View.
2.  Array View.

#### 8.1 Heap tree view

-   Draw active heap elements in their correct parent/child
    relationships.
-   Use the zero-based index relationships defined in the SRS:
    -   Parent: `(i - 1) / 2`
    -   Left child: `2 * i + 1`
    -   Right child: `2 * i + 2`
-   Do not treat extracted/sorted elements as active heap nodes.
-   Show the current maximum/root clearly.
-   Recalculate visual positions when the active heap size changes.

#### 8.2 Array view

Display each array index and value. Distinguish:

-   Active heap elements.
-   Extracted and sorted elements.

Do not rely on color alone; use labels, borders, icons, or separated
regions.

#### 8.3 Synchronization

The tree and array are two views of the same underlying heap array. They
must never be independently mutated.

When a `HEAP_SWAP` event occurs:

1.  Read the two affected array indices from the event.
2.  Highlight the corresponding array cells.
3.  Highlight the corresponding tree nodes.
4.  Animate the swap consistently in both views.
5.  Complete the visual update.
6.  Confirm both views represent the same current model array.

Handle `HEAP_SIZE_CHANGE`, `EXTRACT_MAX`, and `MARK_SORTED` so that
active and sorted regions remain correct.

#### 8.4 Required heap operations to visualize

The SRS requires:

-   Build Max Heap.
-   Sift Up.
-   Sift Down.
-   Heapify.
-   Insert.
-   Extract Maximum.
-   Heap Sort.
-   Max Heap validation.

Use the agreed event names, including `HEAP_BUILD`, `HEAP_SWAP`,
`HEAPIFY_START`, `HEAPIFY_DONE`, `EXTRACT_MAX`, `MARK_SORTED`,
`HEAP_SIZE_CHANGE`, and `HEAP_SORT_COMPLETE`.

**Exit criteria:**

-   Tree and array views show the same heap state.
-   Every swap highlights the same two indices in both views.
-   Extracted/sorted elements are separated from the active heap.
-   Build, heapify, extraction, and sorting animations finish in the
    correct state.
-   No GUI action independently changes heap array values.

### Slice 9 --- Act 1--4 gameplay/visualization screen integration

**Owner:** Member A: Acts 1--2; Member B: Acts 3--4\
**Dependency:** Relevant visualization slices and shared
event/controller interface.

The work division assigns:

-   Member A: Act 1 and Act 2 screens.
-   Member B: Act 3 and Act 4 screens.

Each act screen must use the shared application and agreed architecture.
Do not create four unrelated applications or four different animation
systems.

#### 9.1 Shared gameplay interface

The SRS identifies these shared GUI components:

-   Tree/Heap Canvas.
-   HUD (alarm, score, and objectives).
-   Event Log.
-   Operation Panel.
-   Toolbar.
-   Instruction Panel.

For Phase 3, ensure the visualization screen has the components needed
to operate and understand the current act. If full HUD scoring/alarm
behavior or full EventLog history is not yet implemented, preserve the
existing interface or a clearly identified placeholder; do not fabricate
Phase 4 functionality.

The screen must provide:

-   Active act/structure identification.
-   Current structure visualization.
-   Relevant operation controls.
-   Level instruction/objective area where supplied by existing
    level/game logic.
-   Animation playback controls.
-   Clear operation feedback.

#### 9.2 Act-specific visualization

  -----------------------------------------------------------------------
  Act                     Structure               Phase 3 visual focus
  ----------------------- ----------------------- -----------------------
  Act 1 --- The Blueprint Binary Tree             Traversals and tree
  Wing                                            operations

  Act 2 --- The Sorted    BST                     Search path, insertion,
  Wing                                            deletion, range-query
                                                  visualization

  Act 3 --- The           AVL Tree                Balance factors,
  Architect's Wing                                subtree sizes, and four
                                                  rotations

  Act 4 --- The Heap      Max Heap                Heap tree, array,
  Fortress                                        swaps, extraction, and
                                                  sorting
  -----------------------------------------------------------------------

The SRS defines five levels per act. Phase 3 must make the visualization
components capable of supporting the relevant operations, but full level
progression, win conditions, scoring, and campaign integration remain
Phase 4 work.

#### 9.3 Act 4 layout

The SRS specifically describes Act 4 with:

-   Level information.
-   Alarm meter.
-   Heap tree.
-   Array visualization.
-   Operation panel.
-   Event log.
-   Instruction panel.

Preserve these elements in the Act 4 screen if they already exist. The
Phase 3 implementation must make the heap tree, array, and
operation/animation feedback functional. Full alarm-meter rules and
scoring belong to game systems integration.

**Exit criteria:**

-   All four act visualization screens can be opened through the
    existing application flow or a temporary development harness.
-   Each screen displays the correct structure.
-   Each screen uses the shared event/animation architecture.
-   Controls invoke the correct structure/game-logic APIs.
-   No act duplicates or bypasses the shared visualization pipeline.

------------------------------------------------------------------------

## 4. Required event vocabulary

The SRS lists the following required event names. Preserve their exact
spelling and capitalization as specified by the SRS. The coding
guardrails require string-based identifiers and centrally defined
values; do not use enums.

  -----------------------------------------------------------------------
  Event                               Purpose in visualization
  ----------------------------------- -----------------------------------
  `VISIT`                             Identify a node being visited.

  `COMPARE`                           Show a comparison involving a
                                      node/key.

  `INSERT`                            Visualize insertion.

  `UNLINK`                            Visualize a node/link removal.

  `SWAP_VALUE`                        Visualize a value swap where used
                                      by the model.

  `ROTATE`                            Visualize an AVL rotation.

  `UPDATE_HEIGHT`                     Refresh displayed height
                                      information.

  `UPDATE_SIZE`                       Refresh displayed subtree-size
                                      information.

  `BALANCE_ALERT`                     Highlight an AVL balance issue.

  `HIGHLIGHT_SUBTREE`                 Highlight a subtree, including
                                      range-query context.

  `HEAP_BUILD`                        Begin/show heap construction.

  `HEAP_SWAP`                         Synchronize and animate a heap
                                      swap.

  `HEAPIFY_START`                     Show the beginning of heapify.

  `HEAPIFY_DONE`                      Show completion of heapify.

  `EXTRACT_MAX`                       Visualize maximum extraction.

  `MARK_SORTED`                       Mark an extracted/sorted element.

  `HEAP_SIZE_CHANGE`                  Update the active heap region.

  `HEAP_SORT_COMPLETE`                Show completion of Heap Sort.

  `DONE`                              Mark the operation/event sequence
                                      as complete.
  -----------------------------------------------------------------------

The guardrails require lowercase `snake_case` for string-based
identifiers, while the SRS lists event names in uppercase. **Do not
silently choose one convention or create mixed aliases.** Inspect the
current event contract and resolve this exact naming convention with the
team before changing it. Once agreed, define one canonical mapping
centrally and use it consistently across producers and consumers. Do not
emit both forms as separate event types.

For every event, ensure the payload contains enough information to
identify the affected node(s), array index/indices, and values needed by
its visual action. The exact payload schema must be based on existing
model APIs and coordinated shared interfaces; do not invent payload
fields without inspecting the repository.

------------------------------------------------------------------------

## 5. SFML rendering and interaction requirements

### 5.1 Main loop

-   Keep the window responsive during all animations.
-   Poll input regularly.
-   Update animation using elapsed frame time.
-   Render the current state every frame.
-   Do not use blocking delays for animation.
-   Do not run an entire animation inside one input callback.
-   Avoid doing expensive layout or resource loading repeatedly when it
    is not needed.

### 5.2 Rendering responsibilities

The GUI/rendering layer is responsible for:

-   Drawing nodes and edges.
-   Drawing labels and metadata.
-   Drawing the heap array.
-   Drawing selection and operation highlights.
-   Drawing controls and interface components.
-   Displaying animation progress.

It is not responsible for:

-   Tree insertion/deletion algorithms.
-   BST search logic.
-   AVL rotations or balance calculations.
-   Heapify, sift-up/down, extraction, or Heap Sort algorithms.
-   Modifying model pointers or heap values.

### 5.3 Readability and accessibility

The SRS explicitly states that color must not be the only indicator of a
node's status. Use combinations of:

-   Text labels.
-   Outlines.
-   Icons or markers.
-   Shape/size differences where appropriate.
-   Clear panel labels.

Text and node values must remain readable against the background. Keep
the interface usable at the project's target window size and when the
view is zoomed.

### 5.4 Performance

The SRS sets a target of 60 FPS and requires support for a 200-node tree
or large heap without significant frame drops.

-   Avoid unnecessary per-frame allocations.
-   Avoid repeated full-model traversals for every small draw operation
    where cached layout data can be safely updated.
-   Recompute layout after relevant structural changes, not arbitrarily
    on every frame.
-   Keep benchmark runs independent of GUI animation and event
    recording, as required by the SRS.
-   Measure performance rather than claiming that the target is met
    without testing.

------------------------------------------------------------------------

## 6. Mandatory visual theme

The user has already supplied the visual theme and visual reference
images to the coding agent. **Do not redesign, replace, or reinterpret
the theme.** Use the supplied images as the visual reference for layout
and appearance. The theme guide remains the written art-direction
authority.

### 6.1 Theme identity

The theme is **Bio-Organic Neural Hive**. The environment is a massive
living subterranean biological hive. Trees and heaps are represented as
living neural networks and vascular systems, not ordinary stone/metal
diagrams.

### 6.2 Nodes --- Bioluminescent Synaptic Pods

Node appearance:

-   Circular, semi-translucent outer membrane.
-   Glowing, fluid-like nervous-energy core.
-   Value/key centered in the node.
-   Clean, contrasting text such as white or bright cyan.
-   Slight dark text shadow for readability.
-   Sprite-based rendering is the preferred approach described in the
    theme guide.

Theme-guide states:

**Default active node** - Core: radial appearance from bright cyan
`#E0FFFF` at the center toward turquoise `#40E0D0`. - Outer membrane:
deep teal `#008080` with slight fleshy purple `#4A0E4E` at the edge.

**Highlighted/selected/traversed/swapping node** - Core brightens toward
electric blue `#00FFFF` and white `#FFFFFF`. - Use a subtle sine-wave
pulse on alpha or scale to suggest a firing synapse.

**Inactive/null/extracted node** - Dormant greyish-purple core:
`#554455` to `#2A202A`. - Dark maroon outer membrane: `#301015`.

### 6.3 Edges --- Vascular Nerve Fibers

-   Edges represent parent-child links.
-   Their appearance should be organic, sinewy, and slightly twisted.
-   The theme guide recommends `sf::VertexArray` (quads or thick lines)
    so edges can stretch and rotate as nodes move.
-   Base tissue color transitions from dark crimson `#8B0000` to deep
    magenta `#8B008B`.
-   A faint turquoise `#40E0D0` energy line may run through the center.
-   Traversal/search may show a cyan impulse traveling along the edge.

Do not allow decorative edge effects to obscure the actual parent-child
relationship.

### 6.4 Environment and background

-   Dark, cavernous, living-hive environment.
-   Ribbed, bone-like structures and dormant bioluminescent roots may
    appear in the background.
-   Main shadows: deep purple `#0F0514` and deep crimson `#1A0508`.
-   Midtones: dark organic reds and browns.
-   Highlights: cyan/teal `#00FFFF`.
-   The theme guide describes a static high-resolution background image,
    with an optional subtle, low-opacity, slow-moving spore/dust
    overlay.

Use the already supplied background and reference images. Do not
generate or substitute a different art direction.

### 6.5 Buttons and HUD

-   Use oval, pill-shaped, or slightly irregular rounded rectangles.
-   Avoid sharp, perfectly square 90-degree UI boxes.
-   Buttons should feel like chitinous plates or hardened organic
    membranes.
-   Default button: dark maroon `#3E0C17` toward dark purple `#2D142C`,
    muted teal border `#005555`, pale cyan text `#E0FFFF`.
-   Hover: slightly lighter background, turquoise `#40E0D0` border glow,
    white `#FFFFFF` text.
-   Pressed: significantly darker background with a brief border flash.

Keep controls readable and functional. Visual effects must not interfere
with hit testing or performance.

### 6.6 Theme implementation order

1.  First make layout, hit testing, drawing, and interactions correct.
2.  Apply the supplied node, edge, background, and button assets/styles.
3.  Add selected/visited/compared state treatments.
4.  Add subtle pulse/energy effects.
5.  Profile the application and reduce effects if they harm
    responsiveness.

The theme must not be used as an excuse for incorrect node positions,
unreadable values, broken controls, or unsynchronized views.

------------------------------------------------------------------------

## 7. Testing and verification plan

Testing is required throughout Phase 3, not only at the end.
Data-structure unit tests must remain independent of the GUI.

### 7.1 Build and launch tests

-   Clean build using the repository's documented build process.
-   Launch the application.
-   Resize or use the supported window dimensions.
-   Confirm the window remains responsive.
-   Confirm fonts, textures, and supplied assets load.
-   Confirm no missing-resource errors occur.
-   Confirm the application exits cleanly.

### 7.2 Binary Tree and BST GUI tests

-   Empty tree.
-   Single-node tree.
-   Balanced tree.
-   Skewed tree.
-   Tree with missing left/right children.
-   All four traversal orders.
-   Node selection and hover.
-   Zoom and pan.
-   Search found and not found.
-   Insert into an empty and non-empty BST.
-   Reject duplicate BST keys.
-   Delete a leaf.
-   Delete a node with one child.
-   Delete a node with two children using inorder successor.
-   Range query with skipped subtrees.
-   Verify the final drawing matches the model after every operation.

### 7.3 Animation-controller tests

-   Empty event sequence.
-   One-event sequence.
-   Multi-event sequence.
-   Play to completion.
-   Pause while playing.
-   Resume after pause.
-   Step one event at a time.
-   Adjust speed.
-   Start a second operation after completion.
-   Start a new operation while another is active according to the
    agreed policy.
-   Confirm no stale event is replayed.
-   Confirm the window remains responsive.

### 7.4 AVL tests

-   LL rotation.
-   RR rotation.
-   LR rotation.
-   RL rotation.
-   Height update.
-   Balance-factor display.
-   Subtree-size display.
-   Insert/delete operation animation.
-   Compare displayed values with the AVL model after each operation.
-   Confirm final AVL invariants.

### 7.5 Heap tests

-   Build heap from a non-heap array.
-   Sift up.
-   Sift down.
-   Heapify.
-   Insert.
-   Extract maximum.
-   Heap Sort.
-   Duplicate and negative values.
-   Multiple swaps.
-   Heap size change.
-   Extracted/sorted region.
-   Confirm tree and array views match after every event.

### 7.6 Integration tests

Demonstrate all four acts in the same application:

1.  Act 1: perform and animate a Binary Tree traversal.
2.  Act 2: search, insert, and delete in a BST.
3.  Act 3: demonstrate LL, RR, LR, and RL AVL rotations.
4.  Act 4: build a heap, perform swaps/extraction, and show synchronized
    tree and array views.

For each demonstration, verify:

-   User action reaches the correct model operation.
-   Model produces the expected event sequence.
-   Animation controller consumes events in order.
-   Renderer displays the expected transition.
-   Final GUI state matches the model.
-   No GUI component has modified the data structure independently.

### 7.7 Performance checks

Measure:

-   Frame rate during idle rendering.
-   Frame rate during traversal animation.
-   Frame rate during AVL rotation animation.
-   Frame rate during heap swaps/sorting visualization.
-   Behavior with approximately 200 tree nodes or a large heap.

The SRS target is 60 FPS without significant frame drops. Report actual
measurements and test conditions. Do not state that the target is
achieved without evidence.

------------------------------------------------------------------------

## 8. Phase 3 integration milestone (M3)

The `workdivision.md` defines Integration Milestone 3 as:

> Connect both visualizations to the same application and event
> architecture. Test that GUI operations correctly update the underlying
> models.

M3 is complete only when all of the following are true:

-   Member A's Binary Tree/BST visualization is integrated.
-   Member B's AVL/Heap visualization is integrated.
-   All four structures use the same agreed event architecture.
-   The application uses one coherent update/render flow.
-   The animation controller works with events from the relevant
    structures.
-   Heap tree and array views remain synchronized.
-   All four act screens can be demonstrated.
-   GUI operations correctly update the underlying models.
-   The final visual state agrees with the model state.
-   Both members can build and run the integrated application.
-   No unapproved interface or architecture changes remain.

------------------------------------------------------------------------

## 9. Suggested 4--6 day execution schedule

This schedule follows the Phase 3 estimate and ownership in
`workdivision.md`. It is a practical order, not a reason to omit
acceptance criteria.

  --------------------------------------------------------------------------------------------
  Day               Member A --- Tree Systems Member B --- AVL,     Joint checkpoint
                                              Heap and Game         
                                              Interface             
  ----------------- ------------------------- --------------------- --------------------------
  Day 1             Audit existing tree GUI;  Audit existing        Confirm build, event
                    repair LayoutEngine and   controller/event      contract, and shared
                    static tree rendering.    code; repair          interfaces.
                                              controller            
                                              foundation.           

  Day 2             Complete tree selection,  Complete animation    Run a test event sequence
                    highlighting, zoom/pan,   queue, pause/resume,  through the shared
                    and operation panel.      step, and speed       application.
                                              control.              

  Day 3             Complete traversal        Complete AVL badges   Verify
                    animations and BST        and rotation          model-to-event-to-render
                    search/insert/delete      animations.           behavior.
                    paths.                                          

  Day 4             Complete                  Complete heap         Test AVL rotations and
                    search-path/range-query   tree/array views and  heap swap synchronization.
                    visuals and Acts 1--2     synchronization.      
                    screens.                                        

  Day 5             Integrate and test Acts   Integrate and test    Run all four structures in
                    1--2.                     Acts 3--4.            one application.

  Day 6, if needed  Fix integration issues    Fix integration       Complete M3 demonstration
                    and run tree GUI          issues and run        and record remaining Phase
                    regression tests.         AVL/heap/controller   4 work.
                                              regression tests.     
  --------------------------------------------------------------------------------------------

If the current GUI has major defects, spend the necessary time
diagnosing and repairing them. Do not rush into decorative polish while
the underlying event, rendering, or interaction flow is broken.

------------------------------------------------------------------------

## 10. Required development process for the coding agent

Follow this sequence. Do not jump directly to broad implementation.

### Step 1 --- Repository inventory

Return a short inventory containing:

-   Build system and command.
-   Application entry point.
-   Current directory structure.
-   Existing data-structure headers and classes.
-   Existing GUI/screens.
-   Existing layout/rendering components.
-   Existing event types and payloads.
-   Existing animation controller.
-   Existing assets and how they are loaded.
-   Current compilation and runtime status.

### Step 2 --- Defect report

For every broken GUI feature, provide:

  Field             Required information
  ----------------- --------------------------
  Feature           What is supposed to work
  Actual behavior   What currently happens
  Reproduction      Exact steps
  Root cause        Code-level reason
  Affected files    Exact paths
  Correction        Smallest safe fix
  Verification      Test proving the fix

Do not report a symptom as the root cause.

### Step 3 --- Interface agreement

Before parallel implementation, document:

-   How game logic calls each data structure.
-   How each model operation emits events.
-   Event naming convention and payload.
-   How events are handed to the animation controller.
-   How the renderer reads the model state.
-   How node IDs and heap array indices are represented.
-   Which member owns each shared interface.

The source documents require both members to agree on interfaces and
event format. Do not silently change them later.

### Step 4 --- Repair and implement one vertical slice at a time

For each slice:

1.  State the intended behavior.
2.  Identify the existing files to modify.
3.  Make the smallest necessary change.
4.  Compile.
5.  Run the relevant manual or unit test.
6.  Verify the model state.
7.  Verify the event sequence.
8.  Verify the rendered result.
9.  Record the result.
10. Only then proceed to the next slice.

### Step 5 --- Integrate continuously

-   Keep Member A and Member B working in parallel.
-   Merge small, working modules.
-   Do not wait until the final day to integrate.
-   Do not edit another member's module without coordination.
-   Keep shared interfaces stable.
-   Every integrated module must compile and pass its relevant tests.

### Step 6 --- Final M3 report

At the end, report:

-   Completed Phase 3 features.
-   Repaired GUI defects and their root causes.
-   Files created or modified.
-   Event contract used.
-   Tests run and their results.
-   Performance measurements.
-   M3 acceptance checklist results.
-   Known limitations.
-   Items explicitly deferred to Phase 4.

Do not mark an item complete merely because code was written. It must be
built, run, and verified.

------------------------------------------------------------------------

## 11. Phase 3 acceptance checklist

Mark an item complete only after demonstrating it in the running
application.

### A. Existing GUI repair and architecture

-   [ ] Existing GUI was audited before replacement or redesign.
-   [ ] Build and runtime defects were recorded with root causes.
-   [ ] Working Phase 1/2 implementations were preserved.
-   [ ] SFML remains outside the data-structure layer.
-   [ ] GUI operations pass through game logic.
-   [ ] Model events reach the animation controller.
-   [ ] Renderer reflects model/event state.
-   [ ] Shared interfaces and event names were agreed and centralized.
-   [ ] No enums or STL containers were introduced.

### B. Binary Tree and BST --- Member A

-   [ ] LayoutEngine places nodes correctly.
-   [ ] Parent-child edges are correct.
-   [ ] Node values are readable.
-   [ ] Empty, single-node, balanced, and skewed trees render.
-   [ ] Node selection works.
-   [ ] Hover and highlighting work.
-   [ ] Zoom works.
-   [ ] Background pan works.
-   [ ] Tree operation panel calls game logic.
-   [ ] Preorder animation works.
-   [ ] Inorder animation works.
-   [ ] Postorder animation works.
-   [ ] Level-order animation works.
-   [ ] BST search path is correct.
-   [ ] BST insertion animation is correct.
-   [ ] BST leaf deletion animation is correct.
-   [ ] BST one-child deletion animation is correct.
-   [ ] BST two-child deletion uses the model's inorder-successor
    behavior.
-   [ ] Range-query skipped subtrees are highlighted.
-   [ ] Acts 1 and 2 screens are integrated.

### C. Animation controller --- Member B

-   [ ] Event queue preserves event order.
-   [ ] Empty and completed queues are handled safely.
-   [ ] Play works.
-   [ ] Pause works.
-   [ ] Resume works.
-   [ ] Step works.
-   [ ] Speed adjustment works.
-   [ ] New operations do not replay stale events.
-   [ ] Animation does not freeze the SFML window.
-   [ ] No STL container is used for the queue.
-   [ ] Playback states use the agreed string convention.

### D. AVL --- Member B

-   [ ] Node key/value is visible.
-   [ ] Height is visible.
-   [ ] Balance factor is visible.
-   [ ] Subtree size is visible.
-   [ ] Balance alerts highlight the correct node.
-   [ ] LL rotation animation is correct.
-   [ ] RR rotation animation is correct.
-   [ ] LR rotation animation is correct.
-   [ ] RL rotation animation is correct.
-   [ ] Height and size updates match the model.
-   [ ] Act 3 screen is integrated.

### E. Max Heap --- Member B

-   [ ] Heap tree view is correct.
-   [ ] Heap array view is correct.
-   [ ] Active elements are distinguished from sorted/extracted
    elements.
-   [ ] Build Max Heap animation works.
-   [ ] Sift-up visualization works.
-   [ ] Sift-down visualization works.
-   [ ] Heapify visualization works.
-   [ ] Insert visualization works.
-   [ ] Extract Max visualization works.
-   [ ] Heap Sort visualization works.
-   [ ] Every `HEAP_SWAP` updates both views consistently.
-   [ ] Heap size changes are reflected in both views.
-   [ ] Act 4 screen is integrated.

### F. Shared integration milestone M3

-   [ ] All four structure visualizations run in one application.
-   [ ] All four use the agreed event architecture.
-   [ ] Each GUI operation updates the correct model.
-   [ ] The renderer's final state matches the model.
-   [ ] No visualization independently mutates model data.
-   [ ] The application remains responsive during animation.
-   [ ] Relevant GUI regression tests pass.
-   [ ] Performance has been measured and reported.
-   [ ] Both members can build, run, explain, and demonstrate the
    integrated work.

------------------------------------------------------------------------

## 12. Phase 3 versus Phase 4 handoff

At the end of Phase 3, hand over a stable visualization layer to Phase
4.

### Phase 3 must provide

-   Working tree and heap rendering.
-   Working node interaction.
-   Working traversal/search/insert/delete/rotation/heap visualization.
-   Working animation controller and playback.
-   Working synchronized heap tree and array views.
-   Integrated Act 1--4 visualization screens.
-   Stable event and visualization interfaces.
-   Test evidence for M3.

### Phase 4 will complete

As assigned in `workdivision.md`:

-   Main menu and campaign map.
-   LevelManager.
-   ScoreSystem and alarm meter.
-   Full EventLog.
-   Undo and replay integration.
-   Save and load.
-   Sandbox.
-   Endless mode.
-   Level data and win conditions.
-   Campaign progression and final game integration.

Do not duplicate Phase 4 modules in Phase 3. Do not remove existing
Phase 4 code if it is already present; preserve it and ensure the Phase
3 work does not break it.

------------------------------------------------------------------------

## 13. Final instruction

The objective is to make the **existing Vault Heist GUI function
correctly and complete the agreed Phase 3 visualization scope**, not to
produce a new, unrelated interface.

Follow the source documents, preserve the existing architecture and
visual references, repair before replacing, implement in vertical
slices, and verify each slice from user input through model operation,
event generation, animation, and rendering.

**The Phase 3 milestone is achieved only when all four data structures
are correctly visualized in the same application and GUI operations
demonstrably update the underlying models through the agreed
event-driven architecture.**
