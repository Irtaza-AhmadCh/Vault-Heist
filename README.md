# Vault Heist: The Living Vault

**Vault Heist: The Living Vault** is a GUI-based puzzle-strategy game where players infiltrate an organic, bioluminescent living vault represented as hierarchical data structures. Players solve algorithmic puzzles, trigger biological operations, and manage an alarm capacity threshold to extract critical neural codes.

---

## 1. Project Overview & Features (Phase 4 Integration)

- **Act 1: The Blueprint Wing (General Binary Tree)**
  - Levels 1.1 to 1.5: Preorder, Inorder, Postorder, and Level-Order traversals, Blueprint Reconstruction (Pre+In, Post+In), Height and Diameter analysis, Lowest Common Ancestor (LCA) & Path Length, Bilateral Mirroring & Symmetry.
- **Act 2: The Sorted Wing (Binary Search Tree - BST)**
  - Levels 2.1 to 2.5: Search, Grafting Insertion, Deletion (Leaf, One-Child, Two-Child via Inorder Successor), Range Query filtering [30, 70] with inactive subtree dimming, Degeneration analysis.
- **Act 3: The Architect's Wing (AVL Self-Balancing Tree)**
  - Levels 3.1 to 3.5: Balance Factor detection badges ($BF = \text{height}(L) - \text{height}(R)$), Single Rotations (LL Right rotation, RR Left rotation), Double Rotations (LR and RL double pivots), Autonomous Rebalancing, $O(\log N)$ Order Statistics ($k$-th element and rank).
- **Act 4: The Heap Fortress (Max Heap & Heap Sort)**
  - Levels 4.1 to 4.5: Floyd's $O(N)$ bottom-up buildMaxHeap, Sift Down and Sift Up priority repair, Extract Max, Full Ascending Heap Sort, Priority insertion, Max-Heap validation. Synchronized dual-view (Tree + 1D Array).
- **Game Systems:**
  - **Level System:** 20 complete data-driven levels loaded from `levels/campaign_levels.json` with progression, unlocking, objectives, hints, and optimal step baselines.
  - **Alarm & Scoring:**
    - Alarm Capacity: $\lfloor 1.5 \times \text{optimalSteps} \rfloor + 2$.
    - 3 Stars: Steps $\le \text{optimalSteps}$ without penalties.
    - 2 Stars: Steps $\le 1.25 \times \text{optimalSteps}$ with $\le 1$ penalty.
    - 1 Star: Completed within alarm capacity.
    - Score formula: $\max(0, 1000 - 10 \times \text{alarmSteps} - \text{penalties})$.
  - **Save & Load:** Persistent campaign save file (`levels/savegame.dat`) preserving unlock progression, completion status, star ratings, and best scores.
  - **Undo & Replay:** Snapshot buffer (`UndoManager`) supporting undo via `Z` key, event stepping (`Left`/`Right` arrow keys), and animation pause/play (`Space`).
  - **Game Modes:** Campaign Mode, Sandbox Mode (free-form laboratory), Endless Survival Mode (continuous swarm defense), Leaderboard screen, and System Settings.

---

## 2. Strict Coding Guardrails Compliance

- **Zero STL Containers:** No `std::vector`, `std::map`, `std::queue`, or STL algorithms. All data structures, dynamic buffers, and snapshot stacks are implemented from scratch using pointers, structs, and arrays.
- **Zero Enums:** Standardized lowercase `snake_case` string literals (`"menu"`, `"campaign_map"`, `"level_play"`, `"sandbox"`, `"endless"`, `"victory"`, `"failed"`, `"in_progress"`, `"tree"`, `"bst"`, `"avl"`, `"heap"`).
- **Strict Decoupling:** Data structure engines are completely decoupled from SFML and GUI rendering, communicating purely via the standardized event system (`event.h`).

---

## 3. Controls & Hotkeys

| Input | Action |
|---|---|
| **Left Click** | Select pods / array cells, click buttons, enter values |
| **Right Click + Drag** | Pan camera view across the living chamber |
| **Mouse Wheel** | Zoom in and zoom out |
| **Space** | Play / Pause event animation |
| **Right Arrow ($\to$)** | Step forward one event |
| **Left Arrow ($\leftarrow$)** | Step backward one event |
| **Z** | Undo previous operation and restore snapshot |
| **H** | Request pedagogical hint (applies -1 max star penalty) |
| **Esc** | Return to Campaign Map or Main Menu |
| **R** | Reset camera zoom and position to default center |

---

## 4. Compilation & Execution

### Prerequisites
- GCC / MinGW-w64 with C++17 support.
- SFML 2.6.2 (x64) located in `docs/SFML-2.6.2-x64/SFML-2.6.2/`.

### Building the Game
Run the provided build batch script:
```bat
.\build.bat
```
This compiles `main.exe` linking SFML graphics, window, and system libraries.

### Running the Game
Run the provided run script or executable:
```bat
.\run.bat
```
or
```bat
.\main.exe
```

---

## 5. Automated Unit & System Test Suites

The project includes independent verification suites covering each data structure and the complete Phase 4 game system:

1. **Phase 4 Game Systems Test Suite:**
   ```bat
   g++ -std=c++17 tests/game/test_phase4_game_systems.cpp src/game/levels/LevelManager.cpp src/game/scoring/ScoreSystem.cpp src/game/save/SaveManager.cpp src/structures/heap/max_heap.cpp src/core/events/event.cpp -I"docs/SFML-2.6.2-x64/SFML-2.6.2/include" -o tests/game/test_phase4.exe
   .\tests\game\test_phase4.exe
   ```
2. **Binary Tree Unit Tests:**
   ```bat
   g++ -std=c++17 tests/binary_tree/test_binary_tree.cpp -o tests/binary_tree/test_binary_tree.exe
   .\tests\binary_tree\test_binary_tree.exe
   ```
3. **BST Unit Tests (49/49 passed):**
   ```bat
   g++ -std=c++17 tests/bst/test_bst.cpp src/structures/bst/bst.cpp src/core/events/event.cpp -I"docs/SFML-2.6.2-x64/SFML-2.6.2/include" -o tests/bst/test_bst.exe
   .\tests\bst\test_bst.exe
   ```
4. **AVL Tree Unit Tests (48/48 passed):**
   ```bat
   g++ -std=c++17 tests/avl/test_avl_tree.cpp src/structures/avl/avl_tree.cpp src/core/events/event.cpp -I"docs/SFML-2.6.2-x64/SFML-2.6.2/include" -o tests/avl/test_avl.exe
   .\tests\avl\test_avl.exe
   ```
5. **Max Heap Unit Tests (38/38 passed):**
   ```bat
   g++ -std=c++17 tests/heap/test_max_heap.cpp src/structures/heap/max_heap.cpp src/core/events/event.cpp -I"docs/SFML-2.6.2-x64/SFML-2.6.2/include" -o tests/heap/test_heap.exe
   .\tests\heap\test_heap.exe
   ```
6. **GUI Integration Verification Suite (Acts 1 to 4):**
   ```bat
   g++ -std=c++17 tests/gui/test_phase3_gui.cpp src/structures/bst/bst.cpp src/structures/avl/avl_tree.cpp src/structures/heap/max_heap.cpp src/core/events/event.cpp src/game/levels/LevelManager.cpp src/game/scoring/ScoreSystem.cpp src/game/save/SaveManager.cpp -I"docs/SFML-2.6.2-x64/SFML-2.6.2/include" -L"docs/SFML-2.6.2-x64/SFML-2.6.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -o tests/gui/test_gui.exe
   .\tests\gui\test_gui.exe
   ```
