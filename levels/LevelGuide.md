# Vault Heist â€” Level Configuration Guide (Phase 4)

This guide documents the data-driven level architecture and provides instructions for creating, configuring, and testing new levels in **Vault Heist: The Living Vault**.

---

## 1. Architecture Overview

Campaign levels in Vault Heist are defined through data models (`LevelDef`) managed by `LevelManager`. Levels are organized across 4 distinct Acts:

- **Act 1: The Blueprint Wing** (`"tree"`): General Binary Tree operations (Traversals, Reconstruction, Height/Diameter, LCA, Mirroring).
- **Act 2: The Sorted Wing** (`"bst"`): Binary Search Tree operations (Search, Insertion, Deletion cases 1/2/3, Range Query, Degenerate Analysis).
- **Act 3: The Architect's Wing** (`"avl"`): AVL Self-Balancing Tree operations (Balance Factor detection, LL/RR single rotations, LR/RL double rotations, autonomous balancing, order statistics $k$-th and rank).
- **Act 4: The Heap Fortress** (`"heap"`): Max Heap and Heap Sort operations (Floyd's bottom-up construction, Sift Down repair, Extract Max, Heap Sort, priority insertion).

---

## 2. Level Definition Structure (`LevelDef`)

Each level contains the following data fields:

```cpp
struct LevelDef {
    int actNumber;          // 1 to 4
    int levelNumber;        // 1 to 5 (e.g. 1.1 to 1.5)
    char title[64];         // e.g. "Level 1.1: Neural Traversal"
    char actName[64];       // e.g. "The Blueprint Wing"
    char structureType[16]; // "tree", "bst", "avl", or "heap"
    char objective[160];    // Player objective description
    char hint[160];         // Pedagogical guidance / hint
    char requiredAction[32];// Victory action identifier (e.g. "inorder", "search", "insert")
    int targetValue;        // Key/element value required for victory (0 if any or action-based)
    int optimalSteps;       // Calculated by LevelManager from the configured map
    int alarmCapacity;      // Calculated from optimalSteps; never authored in JSON
    bool isUnlocked;        // Whether the chamber is accessible
    int starsEarned;        // 0 to 3 stars earned by player
    int bestScore;          // Highest score recorded
    bool isCompleted;       // Completion status
};
```

---

## 3. JSON Configuration Schema

Levels are configured in `levels/campaign_levels.json` and split act files (`levels/act1-2/`, `levels/act3-4/`).

Example JSON level block:
```json
{
  "act": 2,
  "level": 1,
  "title": "Level 2.1: The Target Pod",
  "actName": "The Sorted Wing",
  "structureType": "bst",
  "objective": "Locate key 60 in the BST using binary search comparisons.",
  "hint": "Keys smaller go left; keys larger go right.",
  "requiredAction": "search",
  "targetValue": 60,
  "gameplayType": "guided_search",
  "initialValues": [50, 30, 70, 20, 40, 60, 85],
  "requiredValues": [50, 70, 60]
}
```

---

## 4. Victory Action Identifiers

When adding or modifying a level, set `"requiredAction"` to one of the following recognized strings:

| Structure | `"requiredAction"` | Description |
|---|---|---|
| Binary Tree | `"preorder"` | Preorder traversal (Root $\to$ Left $\to$ Right) |
| Binary Tree | `"inorder"` | Inorder traversal (Left $\to$ Root $\to$ Right) |
| Binary Tree | `"postorder"` | Postorder traversal (Left $\to$ Right $\to$ Root) |
| Binary Tree | `"level_order"` | Breadth-First traversal across depth layers |
| Binary Tree | `"reconstruct"` | Tree reconstruction from traversal blueprints |
| Binary Tree | `"height_diam"` | Tree height and diameter analysis |
| Binary Tree | `"lca"` | Lowest Common Ancestor calculation |
| Binary Tree | `"mirror"` | Tree inversion / child swapping |
| BST | `"search"` | Search for key (`targetValue`) |
| BST | `"insert"` | Insert key (`targetValue`) |
| BST | `"delete"` | Delete key (`targetValue`) |
| BST | `"range"` | Execute range query [30, 70] |
| AVL Tree | `"demo_ll"` | Execute LL right rotation |
| AVL Tree | `"demo_rr"` | Execute RR left rotation |
| AVL Tree | `"demo_lr"` | Execute LR double rotation |
| AVL Tree | `"demo_rl"` | Execute RL double rotation |
| AVL Tree | `"kth"` | Extract $k$-th smallest element (`targetValue` = $k$) |
| AVL Tree | `"rank"` | Calculate rank of key (`targetValue`) |
| Max Heap | `"build_heap"` | Build heap via Floyd's bottom-up $O(N)$ algorithm |
| Max Heap | `"sift_down"` | Sift down root pod |
| Max Heap | `"sift_up"` | Sift up last pod |
| Max Heap | `"extract_max"`| Sever root and restore heap |
| Max Heap | `"heap_sort"` | Complete full ascending Heap Sort |

---

## 5. Scoring & Alarm Formula Rules

- **Alarm Capacity**:
  $$\text{Capacity} = \lfloor 1.5 \times \text{optimalSteps} \rfloor + 2$$
- **Star Thresholds**:
  - **3 Stars**: Steps $\le$ `optimalSteps`, 0 penalties, 0 hints.
  - **2 Stars**: Steps $\le 1.25 \times \text{optimalSteps}$, $\le 1$ penalty.
  - **1 Star**: Finished within `alarmCapacity`.
  - **0 Stars**: Alarm threshold exceeded (breached).
- **Hint Penalty**: Each hint used deducts 1 from maximum possible stars earned and adds 1 alarm step.
- **Score**:
  $$\text{Score} = \max(0, 1000 - 10 \times \text{alarmSteps} - \text{penalties})$$

---

## 6. How to Add a New Level

1. Open `levels/campaign_levels.json`.
2. Add a new object with a unique `id`, contiguous `order`, supported `gameplayType`, and the logical map fields required by that type.
3. Keep the campaign at or below `MAX_CAMPAIGN_LEVELS` (32).
4. Do not enter `optimalSteps` or `alarmCapacity`; `LevelManager` computes both from the map.
5. In VS Code, use the Vault Heist launch configuration (F5) to build and start the game from `game/main.cpp`.

