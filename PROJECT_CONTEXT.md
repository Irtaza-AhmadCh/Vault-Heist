# Vault Heist: The Living Vault - Project Context & State

**Document Purpose:** Living reference of the project architecture, constraints, completed features, and implementation roadmap. Updated as work progresses so full codebase re-analysis is never needed.

---

## 1. Project Overview & Guardrails
- **Language / Standard:** C++17, MinGW 32-bit (`i686-w64-mingw32`), SFML 2.6.2.
- **Active Codebase Directory:** `game/` (do not touch or recreate legacy `source/` folder).
- **Core Constraints:**
  - Beginner-friendly C++17, manual data structures.
  - No STL containers (`std::vector`, `std::map`, `std::list` prohibited). Fixed arrays / manual buffers only.
  - No enums; lowercase `snake_case` string literals (`const char* const`).
  - Architecture: GUI collects input -> Controller validates & calls model -> Model mutates & emits events -> Renderer animates events.
  - Model must NOT depend on SFML.
  - Alarm capacity formula: $\text{AlarmCapacity} = \lceil 1.5 \times \text{optimalSteps} \rceil + 2$.
  - Score formula: $\text{Score} = \max(0, 1000 - 10 \times \text{alarmSteps} - \text{penalties} + \text{bonuses})$.

---

## 2. Acts Status
- **Act 1 (Blueprint Wing - Binary Tree):** ✅ Completed & Verified.
- **Act 2 (Sorted Wing - BST):** ✅ Completed & Verified (Levels 2.1 to 2.5).
- **Act 3 (Architect's Wing - AVL Tree):** ✅ **FULLY IMPLEMENTED & BUILDS CLEAN**
  - Specification source: `act3-plan.md`.
  - `game/logic/avl/avl_tree.cpp` — Added: `findNode`, `insertRaw`, `manualRotateLeft`, `manualRotateRight`, `manualRotateLR`, `manualRotateRL`, `predictRotationOnInsert`.
  - `game/controller/act3-4/Act3State.h` — All 5 level state structs + `Act3Controller` class.
  - `game/controller/act3-4/Act3State.cpp` — Full implementation of all 5 level handlers.
  - `build.bat` — Added `Act3State.cpp` to compile sources.
  - `game/controller/LevelManager.cpp` — Updated Act 3 level definitions (correct titles, objectives, optimal steps, `requiredAction`).
  - `levels/campaign_levels.json` — Synced with LevelManager.
  - `game/view/GameScreen.h` — Wired: `#include Act3State.h`, `Act3Controller act3` member, `loadCampaignLevel()` Act 3 block, `setupButtons()` Act 3 campaign buttons (IDs 80–89), button click handlers (IDs 80–89), node click handlers for Levels 3.1 and 3.5.
  - Input box suppressed for Act 3 campaign levels.
  - Node properties: `key`, `value`, `height` (leaves = 1), `size` (subtreeSize), `left`, `right`, `parent`.
  - Balance Factor: BF = height(left) - height(right). Balanced if BF ∈ {-1, 0, 1}.
  - Single rotations: LL (Right rotation), RR (Left rotation).
  - Double rotations: LR (Left on left child then Right on pivot), RL (Right on right child then Left on pivot).
  - Order statistics: `kth(k)` (k-th smallest), `convertedRank = nodeCount - k + 1` for k-th largest.
- **Act 4 (Heap Fortress - Max Heap):** ✅ **FULLY IMPLEMENTED & BUILDS CLEAN** (Levels 4.1 to 4.5)
  - Specification source: `tests/act4.md`.
  - **Level 4.1: Activate the Vault** — Bottom-up Max Heap construction on `[4, 10, 3, 5, 1]`. Optimal 3 steps, Alarm cap 7.
  - **Level 4.2: Repair the Security Locks** — Round A (sift-down from disturbed root on `[4,10,3,5,1]`) + Round B (sift-up of inserted treasure 12 at index 5). Optimal 4 steps, Alarm cap 8.
  - **Level 4.3: Extract the Greatest Treasure** — Repeated Extract-Max with player-guided sift-down heapify until all 5 elements sorted. Final: `[1,3,4,5,10]`. Optimal 8 steps, Alarm cap 14.
  - **Level 4.4: Race Against the Guards** — 4 extraction rounds under alarm pressure on 6-element heap `[15,12,8,10,6,4]`. Shows complexity summary on completion. Optimal 7 steps, Alarm cap 13.
  - **Level 4.5: Final Vault Boss** — Full independent Heap Sort on `[12,3,17,8,15,6,1]`. Phase A: Build Max Heap bottom-up. Phase B: Repeated extractions. Phase C: Validate ascending `[1,3,6,8,12,15,17]`. Emits `HEAP_SORT_COMPLETE`. Optimal 12 steps, Alarm cap 20.
  - No -50 point penalty in Act 4. Invalid actions add +1 alarm step only.
  - Act 4 button IDs: 95 (Confirm Swap), 96 (No Swap Needed), 97 (Cancel), 110 (4.2 Confirm Round A), 111 (4.2 Move Up), 112 (4.3 Extract Max), 113 (4.3 Confirm Swap), 114 (4.4 Extract Max), 115 (4.4 Confirm Swap), 116 (4.5 Phase A Confirm), 117 (4.5 No Swap Needed), 118 (4.5 Phase B Extract), 119 (4.5 Phase B Confirm).
  - `setSize()`, `appendRaw()`, `isSortedAscending()` added to MaxHeap.
  - All 5 level state structs: `Level41State`...`Level45State` in `Act4State.h/.cpp`.
  - Full test suite: `tests/test_act4_complete.cpp` — ALL PASSED.

---

## 3. Act 3 Level Specifications & Rules

| Level | Name | Objective & Setup | Optimal Steps | Alarm Cap | Penalties |
|---|---|---|:---:|:---:|---|
| **3.1** | Read the Badges | Raw insert `{50, 30, 70, 20, 80, 10, 90}`. Direct click on 2 unbalanced nodes (30 [BF=+2], 70 [BF=-2]). | 2 | 5 | Selecting balanced or repeated node: +1 alarm step. |
| **3.2** | Single Rotations | Two stages: Stage 0 = LL `{30, 20, 10}` -> Right rotate 30; Stage 1 = RR `{10, 20, 30}` -> Left rotate 10. | 2 | 5 | Wrong rotation: +2 alarm steps, -50 score. |
| **3.3** | Double Rotations | Two stages: Stage 0 = LR `{30, 10, 20}` -> LR rotation; Stage 1 = RL `{10, 30, 20}` -> RL rotation. | 2 | 5 | Wrong case choice: +2 alarm steps, -50 score. |
| **3.4** | Auto & Predict | Sequence `{30, 20, 10, 25, 28}`. Predict rotation before each insert (`no_rotation`, `ll`, `rr`, `lr`, `rl`), then model auto-rebalances. | 5 | 10 | Wrong prediction: +1 alarm step (no score penalty). |
| **3.5** | Boss: Order-Statistic | Phase A: Raw BST `{40, 20, 50, 10, 30, 60, 70}`. RR imbalance at 50 -> Left rotate 50. Phase B: Find 3rd largest ($k=3$, converted rank 5 -> pod 50) using `subtreeSize`. Path: 40 -> 60 -> 50. | 4 | 8 | Phase A wrong rot: +2 alarm, -50 score. Path visits: +1 alarm. Subtree size calculations: 0 alarm. |

---

## 4. Key Source Files
- `game/logic/avl/avl_tree.h` / `avl_tree.cpp`: Core AVL tree model with manual and automatic rotations, order statistics, invariant validation.
- `game/controller/act3-4/Act3State.h` / `Act3State.cpp`: Act 3 level controllers and state machines.
- `game/view/GameScreen.h`: Main game screen coordinating UI, buttons, visual trees, click handling, and level progression.
- `game/controller/LevelManager.cpp` / `levels/campaign_levels.json`: Campaign level definitions and progression.
- `build.bat`: Project build script. Output binary: `main.exe`.
