#include "Act3State.h"

Act3Controller::Act3Controller() {
    activeLevelNumber = 1;
    currentAVL = nullptr;
}

void Act3Controller::initLevel(int levelNum, AVLTree* avlModel, ScoreSystem& score) {
    activeLevelNumber = levelNum;
    currentAVL = avlModel;

    if (currentAVL == nullptr) return;

    // Clear any existing tree nodes
    while (!currentAVL->isEmpty()) {
        currentAVL->deleteNode(currentAVL->getRoot()->key);
    }

    if (levelNum == 1) {
        // Level 3.1: Read the Badges
        // Insert raw keys {50, 30, 70, 20, 80, 10, 90}
        int initialKeys[] = {50, 30, 70, 20, 80, 10, 90};
        for (int i = 0; i < 7; i++) {
            currentAVL->insertRaw(initialKeys[i]);
        }
        lvl31.unbalancedKeys[0] = 30;
        lvl31.unbalancedKeys[1] = 70;
        lvl31.unbalancedCount = 2;
        lvl31.foundKeys[0] = false;
        lvl31.foundKeys[1] = false;
        lvl31.foundCount = 0;
        lvl31.isLevelComplete = false;
        snprintf(lvl31.statusMsg, sizeof(lvl31.statusMsg),
                 "READ THE BADGES: Click all rooms violating AVL balance (|BF| > 1). [0/2 found]");
        score.reset(score.optimalSteps, score.alarmCapacity);

    } else if (levelNum == 2) {
        // Level 3.2: Single Rotations
        lvl32.stage = 0;
        lvl32.totalStages = 2;
        lvl32.isLevelComplete = false;
        setupLevel32Stage(0);
        score.reset(score.optimalSteps, score.alarmCapacity);

    } else if (levelNum == 3) {
        // Level 3.3: Double Rotations
        lvl33.stage = 0;
        lvl33.totalStages = 2;
        lvl33.isLevelComplete = false;
        setupLevel33Stage(0);
        score.reset(score.optimalSteps, score.alarmCapacity);

    } else if (levelNum == 4) {
        // Level 3.4: Auto Mode and Prediction
        int seq[] = {30, 20, 10, 25, 28};
        for (int i = 0; i < 5; i++) {
            lvl34.sequence[i] = seq[i];
        }
        lvl34.totalCount = 5;
        lvl34.currentIdx = 0;
        lvl34.lastPrediction[0] = '\0';
        lvl34.actualRotation[0] = '\0';
        lvl34.lastWasCorrect = false;
        lvl34.isLevelComplete = false;
        snprintf(lvl34.statusMsg, sizeof(lvl34.statusMsg),
                 "PREDICTION: Next key to insert is %d. Predict rotation before insertion!", lvl34.sequence[0]);
        score.reset(score.optimalSteps, score.alarmCapacity);

    } else if (levelNum == 5) {
        // Level 3.5: Boss: Order-Statistic Heist
        // Raw BST {40, 20, 50, 10, 30, 60, 70}
        int bossKeys[] = {40, 20, 50, 10, 30, 60, 70};
        for (int i = 0; i < 7; i++) {
            currentAVL->insertRaw(bossKeys[i]);
        }
        lvl35.phase = 0; // Phase A: Repair
        lvl35.k = 3;     // 3rd largest
        lvl35.convertedRank = 5; // 7 - 3 + 1 = 5
        lvl35.targetKey = 50;
        lvl35.currentPathNode = 40;
        lvl35.pathCount = 0;
        lvl35.targetFound = false;
        lvl35.isLevelComplete = false;
        snprintf(lvl35.statusMsg, sizeof(lvl35.statusMsg),
                 "BOSS PHASE A: Pod 50 has an RR imbalance. Drag pod 50 to the left to repair the tree.");
        score.reset(score.optimalSteps, score.alarmCapacity);
    }
}

// ============================================================================
// LEVEL 3.1 METHODS
// ============================================================================
bool Act3Controller::handleLevel31NodeClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl31.isLevelComplete) return false;

    if (clickedKey == 30) {
        if (!lvl31.foundKeys[0]) {
            lvl31.foundKeys[0] = true;
            lvl31.foundCount++;
            score.addAlarmStep(1);
            if (lvl31.foundCount == 2) {
                lvl31.isLevelComplete = true;
                snprintf(feedback, maxLen, "LEVEL COMPLETE: All unbalanced nodes identified! Vault integrity verified.");
            } else {
                snprintf(feedback, maxLen, "CORRECT: Pod 30 is unbalanced (BF = +2). [%d/2 found]", lvl31.foundCount);
            }
            return true;
        } else {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "MISTAKE: Pod 30 was already identified! (+1 Alarm)");
            return false;
        }
    } else if (clickedKey == 70) {
        if (!lvl31.foundKeys[1]) {
            lvl31.foundKeys[1] = true;
            lvl31.foundCount++;
            score.addAlarmStep(1);
            if (lvl31.foundCount == 2) {
                lvl31.isLevelComplete = true;
                snprintf(feedback, maxLen, "LEVEL COMPLETE: All unbalanced nodes identified! Vault integrity verified.");
            } else {
                snprintf(feedback, maxLen, "CORRECT: Pod 70 is unbalanced (BF = -2). [%d/2 found]", lvl31.foundCount);
            }
            return true;
        } else {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "MISTAKE: Pod 70 was already identified! (+1 Alarm)");
            return false;
        }
    } else {
        // Balanced node clicked: mistake penalty
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "MISTAKE: Pod %d is balanced (|BF| <= 1)! (+1 Alarm)", clickedKey);
        return false;
    }
}

// ============================================================================
// LEVEL 3.2 METHODS
// ============================================================================
void Act3Controller::setupLevel32Stage(int stageNum) {
    if (currentAVL == nullptr) return;
    while (!currentAVL->isEmpty()) {
        currentAVL->deleteNode(currentAVL->getRoot()->key);
    }

    if (stageNum == 0) {
        // Stage 0: LL case
        currentAVL->insertRaw(30);
        currentAVL->insertRaw(20);
        currentAVL->insertRaw(10);
        lvl32.pivotKey = 30;
        snprintf(lvl32.statusMsg, sizeof(lvl32.statusMsg),
                 "STAGE 1/2 (LL Case): Pod 30 has BF = +2. Drag pod 30 to the right to balance.");
    } else if (stageNum == 1) {
        // Stage 1: RR case
        currentAVL->insertRaw(10);
        currentAVL->insertRaw(20);
        currentAVL->insertRaw(30);
        lvl32.pivotKey = 10;
        snprintf(lvl32.statusMsg, sizeof(lvl32.statusMsg),
                 "STAGE 2/2 (RR Case): Pod 10 has BF = -2. Drag pod 10 to the left to balance.");
    }
}

bool Act3Controller::handleLevel32Rotation(const char* rotType, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl32.isLevelComplete || currentAVL == nullptr) return false;

    if (lvl32.stage == 0) {
        // Stage 0: LL -> requires right rotation
        if (strcmp(rotType, "right") == 0 || strcmp(rotType, "ll") == 0) {
            currentAVL->manualRotateRight(30);
            score.addAlarmStep(1);
            lvl32.stage = 1;
            snprintf(lvl32.statusMsg, sizeof(lvl32.statusMsg),
                     "Right rotation balanced the LL case. The RR case is next.");
            snprintf(feedback, maxLen, "CORRECT: Dragging Pod 30 right applied a right rotation. Showing the balanced tree.");
            return true;
        } else {
            score.addPenalty(50, 2);
            snprintf(feedback, maxLen, "WRONG ROTATION: Pod 30 has LL imbalance. Requires Right Rotation! (+2 Alarm, -50 pts)");
            return false;
        }
    } else if (lvl32.stage == 1) {
        // Stage 1: RR -> requires left rotation
        if (strcmp(rotType, "left") == 0 || strcmp(rotType, "rr") == 0) {
            currentAVL->manualRotateLeft(10);
            score.addAlarmStep(1);
            lvl32.stage = 2;
            lvl32.isLevelComplete = true;
            snprintf(feedback, maxLen, "VICTORY: Left rotation applied to Pod 10. Single rotations mastered!");
            return true;
        } else {
            score.addPenalty(50, 2);
            snprintf(feedback, maxLen, "WRONG ROTATION: Pod 10 has RR imbalance. Requires Left Rotation! (+2 Alarm, -50 pts)");
            return false;
        }
    }
    return false;
}

// ============================================================================
// LEVEL 3.3 METHODS
// ============================================================================
void Act3Controller::setupLevel33Stage(int stageNum) {
    if (currentAVL == nullptr) return;
    while (!currentAVL->isEmpty()) {
        currentAVL->deleteNode(currentAVL->getRoot()->key);
    }

    if (stageNum == 0) {
        // Stage 0: LR case
        currentAVL->insertRaw(30);
        currentAVL->insertRaw(10);
        currentAVL->insertRaw(20);
        lvl33.pivotKey = 30;
        snprintf(lvl33.statusMsg, sizeof(lvl33.statusMsg),
                 "STAGE 1/2 (LR Case): Drag pivot pod 30 to the right to perform the double rotation.");
    } else if (stageNum == 1) {
        // Stage 1: RL case
        currentAVL->insertRaw(10);
        currentAVL->insertRaw(30);
        currentAVL->insertRaw(20);
        lvl33.pivotKey = 10;
        snprintf(lvl33.statusMsg, sizeof(lvl33.statusMsg),
                 "STAGE 2/2 (RL Case): Drag pivot pod 10 to the left to perform the double rotation.");
    }
}

bool Act3Controller::handleLevel33CaseSelect(const char* caseType, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl33.isLevelComplete || currentAVL == nullptr) return false;

    if (lvl33.stage == 0) {
        // Stage 0 expects LR
        if (strcmp(caseType, "lr") == 0) {
            currentAVL->manualRotateLR(30);
            score.addAlarmStep(1);
            lvl33.stage = 1;
            snprintf(lvl33.statusMsg, sizeof(lvl33.statusMsg),
                     "LR double rotation balanced this tree. The RL case is next.");
            snprintf(feedback, maxLen, "CORRECT: Dragging pivot Pod 30 right performed the LR double rotation. Showing the balanced tree.");
            return true;
        } else {
            score.addPenalty(50, 2);
            snprintf(feedback, maxLen, "WRONG CASE: Pod 30 has LR imbalance (left child is right-heavy). (+2 Alarm, -50 pts)");
            return false;
        }
    } else if (lvl33.stage == 1) {
        // Stage 1 expects RL
        if (strcmp(caseType, "rl") == 0) {
            currentAVL->manualRotateRL(10);
            score.addAlarmStep(1);
            lvl33.stage = 2;
            lvl33.isLevelComplete = true;
            snprintf(feedback, maxLen, "VICTORY: RL Double Rotation executed (Right on 30, Left on 10). Double rotations mastered!");
            return true;
        } else {
            score.addPenalty(50, 2);
            snprintf(feedback, maxLen, "WRONG CASE: Pod 10 has RL imbalance (right child is left-heavy). (+2 Alarm, -50 pts)");
            return false;
        }
    }
    return false;
}

// ============================================================================
// LEVEL 3.4 METHODS
// ============================================================================
bool Act3Controller::handleLevel34Prediction(const char* prediction, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl34.isLevelComplete || currentAVL == nullptr) return false;

    int val = lvl34.sequence[lvl34.currentIdx];
    const char* expected = currentAVL->predictRotationOnInsert(val);

    strncpy(lvl34.lastPrediction, prediction, sizeof(lvl34.lastPrediction) - 1);
    strncpy(lvl34.actualRotation, expected, sizeof(lvl34.actualRotation) - 1);

    bool matches = (strcmp(prediction, expected) == 0);
    lvl34.lastWasCorrect = matches;

    if (!matches) {
        score.addAlarmStep(1); // Incorrect prediction: +1 alarm step, no score deduction
    }
    score.addAlarmStep(1); // 1 player step for the insertion action

    // AVL model executes the insertion with autonomous rebalancing
    currentAVL->insert(val);
    lvl34.currentIdx++;

    if (lvl34.currentIdx >= lvl34.totalCount) {
        lvl34.isLevelComplete = true;
        if (matches) {
            snprintf(feedback, maxLen, "VICTORY: Final prediction %s was correct! Inserted %d. Sequence complete!", prediction, val);
        } else {
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Inserted %d (actual: %s). All 5 insertions completed!", val, expected);
        }
    } else {
        int nextVal = lvl34.sequence[lvl34.currentIdx];
        if (matches) {
            snprintf(feedback, maxLen, "CORRECT: Predicted %s on inserting %d. Next key to predict: %d", prediction, val, nextVal);
        } else {
            snprintf(feedback, maxLen, "INCORRECT: Predicted %s, but inserting %d triggered %s (+1 Alarm). Next key: %d",
                     prediction, val, expected, nextVal);
        }
    }
    return matches;
}

// ============================================================================
// LEVEL 3.5 METHODS
// ============================================================================
bool Act3Controller::handleLevel35Repair(const char* rotType, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl35.phase != 0 || currentAVL == nullptr) return false;

    // Pod 50 has RR imbalance (right child 60, right child 70) -> Left rotation
    if (strcmp(rotType, "left") == 0 || strcmp(rotType, "rr") == 0) {
        currentAVL->manualRotateLeft(50);
        score.addAlarmStep(1);
        lvl35.phase = 1; // Advance to Phase B: Order-Statistic Heist
        lvl35.currentPathNode = 40; // start at root
        lvl35.pathCount = 0;
        lvl35.path[lvl35.pathCount++] = 40;
        snprintf(feedback, maxLen,
                 "PHASE A COMPLETE: Vault balanced! Phase B: Find 3rd largest treasure. At root 40 (Left size=3, Rank=4). Go RIGHT!");
        return true;
    } else {
        score.addAlarmStep(2);
        score.addPenalty(50);
        snprintf(feedback, maxLen, "WRONG ROTATION: Imbalance at Pod 50 is RR. Requires Left Rotation! (+2 Alarm, -50 pts)");
        return false;
    }
}

bool Act3Controller::handleLevel35SearchClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl35.phase != 1 || currentAVL == nullptr) return false;

    if (lvl35.targetFound) {
        snprintf(feedback, maxLen, "Pod 50 is already reached! Click Claim Treasure.");
        return true;
    }

    if (lvl35.currentPathNode == 40) {
        // Root 40: left size is 3 -> rank 4. Converted rank 5 > 4 -> must go right to 60.
        if (clickedKey == 60) {
            lvl35.currentPathNode = 60;
            lvl35.path[lvl35.pathCount++] = 60;
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "At Pod 60: Left subtree size is 1. Remaining rank 1 <= 1 -> Go LEFT to Pod 50!");
            return true;
        } else {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "MISTAKE: Rank 5 > 4 (root rank = left size + 1 = 4). Must follow path RIGHT to 60!");
            return false;
        }
    } else if (lvl35.currentPathNode == 60) {
        // Node 60: left child 50 has size 1. Remaining rank 1 <= 1 -> must go left to 50.
        if (clickedKey == 50) {
            lvl35.currentPathNode = 50;
            lvl35.path[lvl35.pathCount++] = 50;
            lvl35.targetFound = true;
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "TARGET LOCATED: Pod 50 is the 3rd largest treasure! Click 'Claim Treasure'.");
            return true;
        } else {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "MISTAKE: Remaining rank 1 <= 1 (left size). Must follow path LEFT to 50!");
            return false;
        }
    }
    return false;
}

bool Act3Controller::handleLevel35Claim(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl35.phase != 1 || !lvl35.targetFound) return false;

    lvl35.phase = 2;
    lvl35.isLevelComplete = true;
    score.addAlarmStep(1);
    snprintf(feedback, maxLen, "VICTORY: 3rd largest treasure (Key 50) extracted in O(log N)! Act 3 Architect's Wing Conquered!");
    return true;
}

