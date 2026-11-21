 #ifndef ACT2_STATE_H
#define ACT2_STATE_H

#include "../../logic/bst/bst.h"
#include "../../logic/avl/avl_tree.h"
#include "../../scoring/ScoreSystem.h"
#include "../LevelDef.h"
#include <cstring>
#include <cstdio>

// ============================================================================
// LEVEL 2.1: SEARCH HEIST
// ============================================================================
struct Level21State {
    int targetKey;              // The target key to find (e.g. 60)
    int currentNodeKey;         // Current node on the search path
    int path[16];               // Visited keys in order
    int pathCount;
    bool revealed[32];          // By layout visual node index
    bool isNullReached;         // True when search points to an empty child
    int nullDirection;          // -1 = left is NULL, 1 = right is NULL
    bool isLevelComplete;
    char statusMsg[128];
};

// ============================================================================
// LEVEL 2.2: STASH LOOT
// ============================================================================
struct BSTEmptySlot {
    int parentKey;
    bool isLeft;
    float x, y;
    bool isTarget;              // True if this is the correct insertion slot
};

struct Level22State {
    int insertKey;              // New key to insert (e.g. 45)
    int currentNodeKey;         // Current node on traversal path
    int path[16];
    int pathCount;
    BSTEmptySlot slots[16];     // Visual NULL placeholder slots
    int slotCount;
    int selectedSlotIndex;
    bool isLevelComplete;
    char statusMsg[128];
};

// ============================================================================
// LEVEL 2.3: STEAL AND COLLAPSE
// ============================================================================
struct Level23State {
    int stage;                  // 0 = Leaf (20), 1 = One Child (60), 2 = Two Children (50), 3 = Complete
    int totalStages;            // 3
    int deleteKey;              // Target key to delete in current stage (20, 60, 50)
    char phase[24];             // "case_select", "stage_cleared", "successor_select", "complete"
    int currentNodeKey;         // Current target node key
    int expectedCase;           // 0 = leaf, 1 = one_child, 2 = two_children
    int selectedCase;
    int expectedSuccessor;      // Inorder successor key (60 for stage 2)
    int selectedSuccessor;
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// LEVEL 2.4: RANGE RAID
// ============================================================================
struct Level24State {
    int lo;                     // Lower bound (30)
    int hi;                     // Upper bound (65)
    int matchingKeys[32];
    int matchingCount;
    int prunedSubtreeRoots[16]; // Root keys of skipped subtrees
    int prunedCount;
    int prunedNodeKeys[16];     // All keys in pruned subtrees (20, 80)
    int prunedNodeCount;
    int visitedKeys[32];        // All keys actually examined (50, 30, 40, 70, 60, 65)
    int visitedCount;
    bool isExecuted;
    bool isLevelComplete;
    int pruningBonus;           // Bonus score awarded for pruning (+150)
    char statusMsg[160];
};

// ============================================================================
// LEVEL 2.5: BOSS: THE LAZY ARCHITECT
// ============================================================================
struct InsertionRecord {
    int alarmSteps;
    int penalties;
    int score;
    int stars;
    bool completed;
};

struct Level25State {
    int stage;                  // 0 = Stage A (Insertion), 1 = Stage B (Search), 2 = Stage C (Explanation/AVL), 3 = Complete
    int sortedValues[7];        // {10, 20, 30, 40, 50, 60, 70}
    int totalCount;             // 7
    int insertedCount;          // Number of values inserted
    int currentArrayIdx;        // Index of value currently being inserted

    // Stage A: Scored insertion record (frozen after Stage A)
    InsertionRecord insertionRecord;

    // Stage B: Separate educational search challenge (unscored)
    int searchTarget;           // Deepest node (70)
    int searchBudget;           // ceil(log2(n+1)) = 3
    int searchComparisonsUsed;
    int currentSearchNodeKey;
    bool searchFailed;

    // Stage C: AVL tree generated from identical values
    AVLTree avlTree;
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// ACT 2 CONTROLLER
// ============================================================================
class Act2Controller {
public:
    int activeLevelNumber;
    Level21State lvl21;
    Level22State lvl22;
    Level23State lvl23;
    Level24State lvl24;
    Level25State lvl25;

    BST* currentBST;

    Act2Controller();

    void initLevel(int levelNum, BST* bstModel, ScoreSystem& score);

    // Level 2.1 methods
    bool handleLevel21Click(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel21TargetNotFound(ScoreSystem& score, char* feedback, int maxLen);

    // Level 2.2 methods
    void computeEmptySlots(BSTNode* node, float x, float y, float xOffset, float yOffset);
    bool handleLevel22NodeClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel22SlotClick(int slotIdx, ScoreSystem& score, char* feedback, int maxLen);

    // Level 2.3 methods
    void initLevel23Stage(int stageNum);
    bool handleLevel23SearchClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);
    bool selectDeletionCase(int caseChoice, char* feedback, int maxLen);
    bool handleLevel23SuccessorClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);
    void advanceLevel23Stage();

    // Level 2.4 methods
    bool executeRangeQuery(ScoreSystem& score, char* feedback, int maxLen);
    void rangeQueryRecursive(BSTNode* node, int lo, int hi);
    bool isKeyPruned(int key) const;
    bool isKeyMatching(int key) const;
    bool isKeyVisited(int key) const;

    // Level 2.5 methods
    bool handleLevel25SelectArrayValue(int idx, char* feedback, int maxLen);
    bool handleLevel25SlotClick(int slotIdx, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel25SearchClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);
    void transitionToStageB(ScoreSystem& score);
    void transitionToStageC(ScoreSystem& score);
    void completeLevel25(ScoreSystem& score);

    void populateInitialBST(int lvlNum);

private:
    void collectPrunedSubtree(BSTNode* node);
    void collectAllSubtreeKeys(BSTNode* node);
};

#endif // ACT2_STATE_H
