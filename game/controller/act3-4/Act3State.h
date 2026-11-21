#ifndef ACT3_STATE_H
#define ACT3_STATE_H

#include "../../logic/avl/avl_tree.h"
#include "../../scoring/ScoreSystem.h"
#include "../LevelDef.h"
#include <cstring>
#include <cstdio>

// ============================================================================
// LEVEL 3.1: READ THE BADGES
// ============================================================================
struct Level31State {
    int unbalancedKeys[8];       // {30, 70}
    int unbalancedCount;         // 2
    bool foundKeys[8];           // whether found
    int foundCount;              // 0..2
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// LEVEL 3.2: MANUAL REPAIR: SINGLE ROTATIONS (LL & RR)
// ============================================================================
struct Level32State {
    int stage;                   // 0 = LL (tree {30, 20, 10}), 1 = RR (tree {10, 20, 30}), 2 = Complete
    int totalStages;             // 2
    int pivotKey;                // 30 in stage 0, 10 in stage 1
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// LEVEL 3.3: MANUAL REPAIR: DOUBLE ROTATIONS (LR & RL)
// ============================================================================
struct Level33State {
    int stage;                   // 0 = LR (tree {30, 10, 20}), 1 = RL (tree {10, 30, 20}), 2 = Complete
    int totalStages;             // 2
    int pivotKey;                // 30 in stage 0, 10 in stage 1
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// LEVEL 3.4: AUTO MODE AND PREDICTION
// ============================================================================
struct Level34State {
    int sequence[8];             // {30, 20, 10, 25, 28}
    int totalCount;              // 5
    int currentIdx;              // index of value being predicted & inserted (0..4)
    char lastPrediction[16];     // "no_rotation", "ll", "rr", "lr", "rl"
    char actualRotation[16];
    bool lastWasCorrect;
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// LEVEL 3.5: BOSS: ORDER-STATISTIC HEIST
// ============================================================================
struct Level35State {
    int phase;                   // 0 = Phase A (Balance tree via RR at 50), 1 = Phase B (Search k-th largest), 2 = Complete
    int k;                       // 3 (3rd largest)
    int convertedRank;           // 7 - 3 + 1 = 5 (5th smallest)
    int targetKey;               // 50
    int currentPathNode;         // 40 -> 60 -> 50
    int path[16];
    int pathCount;
    bool targetFound;
    bool isLevelComplete;
    char statusMsg[160];
};

// ============================================================================
// ACT 3 CONTROLLER
// ============================================================================
class Act3Controller {
public:
    int activeLevelNumber;
    Level31State lvl31;
    Level32State lvl32;
    Level33State lvl33;
    Level34State lvl34;
    Level35State lvl35;

    AVLTree* currentAVL;

    Act3Controller();

    void initLevel(int levelNum, AVLTree* avlModel, ScoreSystem& score);

    // Level 3.1
    bool handleLevel31NodeClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);

    // Level 3.2
    bool handleLevel32Rotation(const char* rotType, ScoreSystem& score, char* feedback, int maxLen);
    void setupLevel32Stage(int stageNum);

    // Level 3.3
    bool handleLevel33CaseSelect(const char* caseType, ScoreSystem& score, char* feedback, int maxLen);
    void setupLevel33Stage(int stageNum);

    // Level 3.4
    bool handleLevel34Prediction(const char* prediction, ScoreSystem& score, char* feedback, int maxLen);

    // Level 3.5
    bool handleLevel35Repair(const char* rotType, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel35SearchClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel35Claim(ScoreSystem& score, char* feedback, int maxLen);
};

#endif // ACT3_STATE_H
