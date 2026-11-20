#include "Act2State.h"
#include <cstdio>
#include <cstring>

Act2Controller::Act2Controller() {
    activeLevelNumber = 1;
    currentBST = nullptr;
}

void Act2Controller::initLevel(int levelNum, BST* bstModel, ScoreSystem& score) {
    activeLevelNumber = levelNum;
    currentBST = bstModel;

    populateInitialBST(levelNum);

    if (levelNum == 1) {
        // Level 2.1: Search Heist
        lvl21.targetKey = 60;
        lvl21.currentNodeKey = 50; // Root
        lvl21.pathCount = 0;
        lvl21.isNullReached = false;
        lvl21.nullDirection = 0;
        lvl21.isLevelComplete = false;
        for (int i = 0; i < 32; i++) lvl21.revealed[i] = false;
        snprintf(lvl21.statusMsg, sizeof(lvl21.statusMsg),
                 "SEARCH HEIST: Target key is %d. Click the root pod to reveal its key and begin comparisons.",
                 lvl21.targetKey);
        score.reset(3, 7);
    } else if (levelNum == 2) {
        // Level 2.2: Stash Loot
        lvl22.insertKey = 45;
        lvl22.currentNodeKey = 50; // Root
        lvl22.pathCount = 0;
        lvl22.selectedSlotIndex = -1;
        lvl22.isLevelComplete = false;
        snprintf(lvl22.statusMsg, sizeof(lvl22.statusMsg),
                 "STASH LOOT: Insert key %d. Follow comparisons (Left if smaller, Right if larger) to find the empty slot.",
                 lvl22.insertKey);
        score.reset(4, 8);
    } else if (levelNum == 3) {
        // Level 2.3: Steal and Collapse (All 3 Deletion Cases: Leaf, One Child, Two Children)
        lvl23.totalStages = 3;
        initLevel23Stage(0);
        score.reset(7, 13);
    } else if (levelNum == 4) {
        // Level 2.4: Range Raid
        lvl24.lo = 30;
        lvl24.hi = 70;
        lvl24.matchingCount = 0;
        lvl24.prunedCount = 0;
        lvl24.prunedNodeCount = 0;
        lvl24.pruningBonus = 150;
        lvl24.isExecuted = false;
        lvl24.isLevelComplete = false;
        snprintf(lvl24.statusMsg, sizeof(lvl24.statusMsg),
                 "RANGE RAID: Range [%d, %d]. Click 'Scan Range' to execute range query and prune out-of-bounds subtrees.",
                 lvl24.lo, lvl24.hi);
        score.reset(5, 10);
    } else if (levelNum == 5) {
        // Level 2.5: Boss: The Lazy Architect
        lvl25.stage = 0; // Stage A: Insertion
        lvl25.totalCount = 7;
        lvl25.insertedCount = 0;
        lvl25.currentArrayIdx = 0;
        int sorted[7] = {10, 20, 30, 40, 50, 60, 70};
        for (int i = 0; i < 7; i++) lvl25.sortedValues[i] = sorted[i];

        lvl25.insertionRecord = {0, 0, 0, 0, false};

        lvl25.searchTarget = 70;
        lvl25.searchBudget = 3; // ceil(log2(7+1)) = 3
        lvl25.searchComparisonsUsed = 0;
        lvl25.currentSearchNodeKey = 10;
        lvl25.searchFailed = false;
        lvl25.isLevelComplete = false;

        snprintf(lvl25.statusMsg, sizeof(lvl25.statusMsg),
                 "BOSS STAGE A (Scored): Insert sorted values [10..70] into the BST. Click 'Insert Next' to graft each pod.");
        score.reset(28, 44);
    }
}

void Act2Controller::populateInitialBST(int lvlNum) {
    if (currentBST == nullptr) return;
    while (!currentBST->isEmpty()) {
        currentBST->deleteNode(currentBST->getRoot()->key);
    }

    if (lvlNum == 1 || lvlNum == 2 || lvlNum == 3) {
        // Standard balanced BST: 50, 30, 70, 20, 40, 60, 85
        int keys[7] = {50, 30, 70, 20, 40, 60, 85};
        for (int i = 0; i < 7; i++) currentBST->insert(keys[i]);
    } else if (lvlNum == 4) {
        // Range Raid BST with distinct branches to prune: 50, 20, 80, 10, 35, 65, 90, 5, 15, 30, 40
        int keys[11] = {50, 20, 80, 10, 35, 65, 90, 5, 15, 30, 40};
        for (int i = 0; i < 11; i++) currentBST->insert(keys[i]);
    } else if (lvlNum == 5) {
        // Empty tree initially for Boss level (built by player)
    }
}

// ============================================================================
// LEVEL 2.1: SEARCH HEIST
// ============================================================================
bool Act2Controller::handleLevel21Click(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl21.isLevelComplete) return false;

    // Check if clicked the expected node on the search path
    if (clickedKey == lvl21.currentNodeKey) {
        score.addAlarmStep(1);
        if (lvl21.pathCount < 16) {
            lvl21.path[lvl21.pathCount++] = clickedKey;
        }

        if (lvl21.targetKey == clickedKey) {
            lvl21.isLevelComplete = true;
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "TARGET FOUND! Pod %d matches target %d. Mission complete!",
                         clickedKey, lvl21.targetKey);
            }
            return true;
        }

        BSTNode* cur = currentBST ? currentBST->getRoot() : nullptr;
        // Find current BST node
        while (cur != nullptr && cur->key != clickedKey) {
            if (clickedKey < cur->key) cur = cur->left;
            else cur = cur->right;
        }

        if (cur == nullptr) return false;

        if (lvl21.targetKey < clickedKey) {
            if (cur->left != nullptr) {
                lvl21.currentNodeKey = cur->left->key;
                if (feedback && maxLen > 0) {
                    snprintf(feedback, maxLen, "%d < %d -> Target is smaller. Branch LEFT to pod %d.",
                             lvl21.targetKey, clickedKey, cur->left->key);
                }
            } else {
                lvl21.isNullReached = true;
                lvl21.nullDirection = -1;
                if (feedback && maxLen > 0) {
                    snprintf(feedback, maxLen, "%d < %d -> Left branch is NULL! Click 'Target Not Found'.",
                             lvl21.targetKey, clickedKey);
                }
            }
        } else {
            if (cur->right != nullptr) {
                lvl21.currentNodeKey = cur->right->key;
                if (feedback && maxLen > 0) {
                    snprintf(feedback, maxLen, "%d > %d -> Target is larger. Branch RIGHT to pod %d.",
                             lvl21.targetKey, clickedKey, cur->right->key);
                }
            } else {
                lvl21.isNullReached = true;
                lvl21.nullDirection = 1;
                if (feedback && maxLen > 0) {
                    snprintf(feedback, maxLen, "%d > %d -> Right branch is NULL! Click 'Target Not Found'.",
                             lvl21.targetKey, clickedKey);
                }
            }
        }
        return true;
    } else {
        // Clicked incorrect node
        score.addPenalty(0, 1); // +1 alarm step penalty
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Wrong node! BST search rule: follow comparisons from current pod %d.",
                     lvl21.currentNodeKey);
        }
        return false;
    }
}

bool Act2Controller::handleLevel21TargetNotFound(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl21.isLevelComplete) return false;

    if (lvl21.isNullReached) {
        lvl21.isLevelComplete = true;
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "CORRECT: Target %d does not exist in the BST (reached NULL child). Mission complete!",
                     lvl21.targetKey);
        }
        return true;
    } else {
        score.addPenalty(0, 1); // +1 alarm step penalty
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Premature! Search has not reached a NULL branch. Continue comparing pods.");
        }
        return false;
    }
}

// ============================================================================
// LEVEL 2.2: STASH LOOT
// ============================================================================
void Act2Controller::computeEmptySlots(BSTNode* node, float x, float y, float xOffset, float yOffset) {
    if (node == nullptr || lvl22.slotCount >= 16) return;
    if (node->left == nullptr) {
        BSTEmptySlot& sl = lvl22.slots[lvl22.slotCount++];
        sl.parentKey = node->key;
        sl.isLeft = true;
        sl.x = x - xOffset;
        sl.y = y + yOffset;
        sl.isTarget = false;
    } else {
        computeEmptySlots(node->left, x - xOffset, y + yOffset, xOffset * 0.5f, yOffset);
    }
    if (node->right == nullptr) {
        BSTEmptySlot& sl = lvl22.slots[lvl22.slotCount++];
        sl.parentKey = node->key;
        sl.isLeft = false;
        sl.x = x + xOffset;
        sl.y = y + yOffset;
        sl.isTarget = false;
    } else {
        computeEmptySlots(node->right, x + xOffset, y + yOffset, xOffset * 0.5f, yOffset);
    }
}

bool Act2Controller::handleLevel22NodeClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl22.isLevelComplete) return false;

    BSTNode* cur = currentBST ? currentBST->getRoot() : nullptr;
    while (cur != nullptr && cur->key != lvl22.currentNodeKey) {
        if (lvl22.currentNodeKey < cur->key) cur = cur->left;
        else cur = cur->right;
    }
    if (cur == nullptr) return false;

    int val = lvl22.insertKey;
    int expectedNextKey = -1;
    if (val < cur->key) {
        if (cur->left) expectedNextKey = cur->left->key;
    } else if (val > cur->key) {
        if (cur->right) expectedNextKey = cur->right->key;
    }

    if (clickedKey == expectedNextKey && expectedNextKey != -1) {
        score.addAlarmStep(1);
        lvl22.currentNodeKey = clickedKey;
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Correct path: Advanced to pod %d. Compare %d with %d.",
                     clickedKey, val, clickedKey);
        }
        return true;
    } else if (clickedKey == lvl22.currentNodeKey) {
        // Clicked current node to re-inspect
        if (feedback && maxLen > 0) {
            if (val < cur->key) {
                snprintf(feedback, maxLen, "%d < %d -> Next step is to branch LEFT.", val, cur->key);
            } else {
                snprintf(feedback, maxLen, "%d > %d -> Next step is to branch RIGHT.", val, cur->key);
            }
        }
        return true;
    } else {
        score.addPenalty(0, 1);
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Wrong direction! %d %s %d. Choose the %s branch.",
                     val, (val < cur->key) ? "<" : ">", cur->key, (val < cur->key) ? "LEFT" : "RIGHT");
        }
        return false;
    }
}

bool Act2Controller::handleLevel22SlotClick(int slotIdx, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl22.isLevelComplete) return false;
    if (slotIdx < 0 || slotIdx >= lvl22.slotCount) return false;

    const BSTEmptySlot& slot = lvl22.slots[slotIdx];
    if (slot.isTarget) {
        score.addAlarmStep(1);
        if (currentBST) currentBST->insert(lvl22.insertKey);
        lvl22.isLevelComplete = true;
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "SUCCESS: Key %d grafted at correct NULL position! BST ordering maintained.",
                     lvl22.insertKey);
        }
        return true;
    } else {
        score.addPenalty(0, 1);
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Incorrect slot! Key %d cannot be inserted here per BST ordering rules.",
                     lvl22.insertKey);
        }
        return false;
    }
}

// ============================================================================
// LEVEL 2.3: STEAL AND COLLAPSE
// ============================================================================
void Act2Controller::initLevel23Stage(int stageNum) {
    lvl23.stage = stageNum;
    lvl23.selectedCase = -1;
    lvl23.selectedSuccessor = -1;
    lvl23.isLevelComplete = false;

    if (stageNum == 0) {
        // Stage 0: Leaf Node (20)
        lvl23.deleteKey = 20;
        lvl23.expectedCase = 0; // Leaf
        lvl23.expectedSuccessor = -1;
        lvl23.currentNodeKey = 50;
        strncpy(lvl23.phase, "search", sizeof(lvl23.phase));
        snprintf(lvl23.statusMsg, sizeof(lvl23.statusMsg),
                 "STAGE 1/3 (Case 1: Leaf): Locate target leaf pod %d. Follow comparisons from root.",
                 lvl23.deleteKey);
    } else if (stageNum == 1) {
        // Stage 1: One-Child Node (30) - 20 is gone, 30 now has only right child 40
        lvl23.deleteKey = 30;
        lvl23.expectedCase = 1; // One Child
        lvl23.expectedSuccessor = -1;
        lvl23.currentNodeKey = 50;
        strncpy(lvl23.phase, "search", sizeof(lvl23.phase));
        snprintf(lvl23.statusMsg, sizeof(lvl23.statusMsg),
                 "STAGE 2/3 (Case 2: One Child): Locate pod %d. Notice it now has only 1 child (40).",
                 lvl23.deleteKey);
    } else if (stageNum == 2) {
        // Stage 2: Two-Child Node (50) - Root 50 has left child 40 and right child 70
        lvl23.deleteKey = 50;
        lvl23.expectedCase = 2; // Two Children
        lvl23.expectedSuccessor = 60; // Minimum of right subtree
        lvl23.currentNodeKey = 50;
        strncpy(lvl23.phase, "case_select", sizeof(lvl23.phase));
        snprintf(lvl23.statusMsg, sizeof(lvl23.statusMsg),
                 "STAGE 3/3 (Case 3: Two Children): Target is root %d. Select case, then choose its Inorder Successor.",
                 lvl23.deleteKey);
    }
}

bool Act2Controller::handleLevel23SearchClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (strcmp(lvl23.phase, "search") != 0) return false;

    if (clickedKey == lvl23.currentNodeKey) {
        score.addAlarmStep(1);
        if (clickedKey == lvl23.deleteKey) {
            strncpy(lvl23.phase, "case_select", sizeof(lvl23.phase));
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "Target pod %d reached! Identify which deletion case applies below.", clickedKey);
            }
            return true;
        }

        BSTNode* cur = currentBST ? currentBST->getRoot() : nullptr;
        while (cur != nullptr && cur->key != clickedKey) {
            if (clickedKey < cur->key) cur = cur->left;
            else cur = cur->right;
        }
        if (cur == nullptr) return false;

        if (lvl23.deleteKey < cur->key && cur->left) {
            lvl23.currentNodeKey = cur->left->key;
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "%d < %d -> Follow LEFT branch to pod %d.",
                         lvl23.deleteKey, cur->key, cur->left->key);
            }
        } else if (lvl23.deleteKey > cur->key && cur->right) {
            lvl23.currentNodeKey = cur->right->key;
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "%d > %d -> Follow RIGHT branch to pod %d.",
                         lvl23.deleteKey, cur->key, cur->right->key);
            }
        }
        return true;
    } else {
        score.addPenalty(0, 1);
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Follow BST comparisons from current pod %d.", lvl23.currentNodeKey);
        }
        return false;
    }
}

bool Act2Controller::selectDeletionCase(int caseChoice, char* feedback, int maxLen) {
    if (strcmp(lvl23.phase, "case_select") != 0) return false;

    lvl23.selectedCase = caseChoice;
    if (caseChoice == lvl23.expectedCase) {
        if (caseChoice == 2) {
            // Two-child case: next is successor selection
            strncpy(lvl23.phase, "successor_select", sizeof(lvl23.phase));
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "Correct: Case 3 (Two Children). Right subtree highlighted! Click Inorder Successor (%d).",
                         lvl23.expectedSuccessor);
            }
        } else if (caseChoice == 0) {
            // Leaf deletion (Stage 0): caller will animate unlink and advance
            strncpy(lvl23.phase, "animating", sizeof(lvl23.phase));
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "Correct: Case 1 (Leaf)! Pod %d has 0 children. Watching unlink animation...", lvl23.deleteKey);
            }
        } else if (caseChoice == 1) {
            // One-child deletion (Stage 1): caller will animate promotion and advance
            strncpy(lvl23.phase, "animating", sizeof(lvl23.phase));
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "Correct: Case 2 (One Child)! Pod %d replaced by its child. Watching promotion animation...", lvl23.deleteKey);
            }
        }
        return true;
    } else {
        // Per section 5.3 & 8.3: No alarm penalty for wrong case selection
        if (feedback && maxLen > 0) {
            if (lvl23.expectedCase == 0) {
                snprintf(feedback, maxLen, "Incorrect: Target pod %d has 0 children (Leaf). Try again.", lvl23.deleteKey);
            } else if (lvl23.expectedCase == 1) {
                snprintf(feedback, maxLen, "Incorrect: Target pod %d has exactly 1 child (40). Try again.", lvl23.deleteKey);
            } else if (lvl23.expectedCase == 2) {
                snprintf(feedback, maxLen, "Incorrect: Target pod %d has 2 children (40 and 70). Try again.", lvl23.deleteKey);
            }
        }
        return false;
    }
}

bool Act2Controller::handleLevel23SuccessorClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (strcmp(lvl23.phase, "successor_select") != 0) return false;

    if (clickedKey == lvl23.expectedSuccessor) {
        score.addAlarmStep(1);
        strncpy(lvl23.phase, "animating", sizeof(lvl23.phase));
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "SUCCESS: Inorder successor %d identified! Watching value swap animation...", clickedKey);
        }
        return true;
    } else {
        // Per doc 8.3: No alarm penalty for incorrect successor selection
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Not the inorder successor! The successor is the SMALLEST node in the right subtree (%d).",
                     lvl23.expectedSuccessor);
        }
        return false;
    }
}

// ============================================================================
// LEVEL 2.4: RANGE RAID
// ============================================================================
void Act2Controller::collectPrunedSubtree(BSTNode* node) {
    if (node == nullptr) return;
    if (lvl24.prunedCount < 16) {
        lvl24.prunedSubtreeRoots[lvl24.prunedCount++] = node->key;
    }
}

bool Act2Controller::executeRangeQuery(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl24.isExecuted) return false;

    lvl24.matchingCount = 0;
    lvl24.prunedCount = 0;

    // Keys in range [30, 70] from {50, 20, 80, 10, 35, 65, 90, 5, 15, 30, 40}:
    // Matching keys: 30, 35, 40, 50, 65
    int matches[5] = {30, 35, 40, 50, 65};
    lvl24.matchingCount = 5;
    for (int i = 0; i < 5; i++) lvl24.matchingKeys[i] = matches[i];

    // Pruned roots: 10 (left child of 20) and 90 (right child of 80)
    lvl24.prunedCount = 2;
    lvl24.prunedSubtreeRoots[0] = 10;
    lvl24.prunedSubtreeRoots[1] = 90;

    lvl24.prunedNodeCount = 4;
    lvl24.prunedNodeKeys[0] = 10;
    lvl24.prunedNodeKeys[1] = 5;
    lvl24.prunedNodeKeys[2] = 15;
    lvl24.prunedNodeKeys[3] = 90;

    // Pruning reward: +150 bonus points
    lvl24.pruningBonus = 150;
    score.bonuses += lvl24.pruningBonus;

    score.addAlarmStep(5); // Optimal steps
    lvl24.isExecuted = true;
    lvl24.isLevelComplete = true;

    if (feedback && maxLen > 0) {
        snprintf(feedback, maxLen, "RANGE QUERY EXECUTED: [%d, %d]. Found 5 pods. Pruned 2 subtrees (4 nodes skipped). +%d PRUNING BONUS!",
                 lvl24.lo, lvl24.hi, lvl24.pruningBonus);
    }
    return true;
}

bool Act2Controller::isKeyPruned(int key) const {
    if (!lvl24.isExecuted) return false;
    // Keys in pruned subtrees: subtree 10 (10, 5, 15) and subtree 90 (90)
    if (key == 10 || key == 5 || key == 15 || key == 90) return true;
    return false;
}

bool Act2Controller::isKeyMatching(int key) const {
    if (!lvl24.isExecuted) return false;
    for (int i = 0; i < lvl24.matchingCount; i++) {
        if (lvl24.matchingKeys[i] == key) return true;
    }
    return false;
}

// ============================================================================
// LEVEL 2.5: BOSS: THE LAZY ARCHITECT
// ============================================================================
bool Act2Controller::handleLevel25SelectArrayValue(int idx, char* feedback, int maxLen) {
    if (lvl25.stage != 0) return false;
    if (idx != lvl25.insertedCount) {
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Select the next ascending value in the sorted array: %d.",
                     lvl25.sortedValues[lvl25.insertedCount]);
        }
        return false;
    }
    lvl25.currentArrayIdx = idx;
    if (feedback && maxLen > 0) {
        snprintf(feedback, maxLen, "Selected %d. Click the rightmost empty (+) slot in the tree to insert it.",
                 lvl25.sortedValues[idx]);
    }
    return true;
}

bool Act2Controller::handleLevel25SlotClick(int slotIdx, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl25.stage != 0) return false;

    int val = lvl25.sortedValues[lvl25.insertedCount];
    if (currentBST) currentBST->insert(val);
    lvl25.insertedCount++;

    // Insertion alarm step accounting
    score.addAlarmStep(lvl25.insertedCount); // Each step down the right chain

    if (lvl25.insertedCount == lvl25.totalCount) {
        // Stage A complete: Freeze scored record!
        lvl25.insertionRecord.alarmSteps = score.alarmSteps;
        lvl25.insertionRecord.penalties = score.penalties;
        lvl25.insertionRecord.score = score.computeFinalScore();
        lvl25.insertionRecord.stars = score.computeStars();
        lvl25.insertionRecord.completed = true;

        transitionToStageB(score);
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "STAGE A COMPLETE! BST degenerated into a corridor. Transitioning to Search Challenge.");
        }
        return true;
    } else {
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Inserted %d. Now select %d from the sorted array.",
                     val, lvl25.sortedValues[lvl25.insertedCount]);
        }
        return true;
    }
}

void Act2Controller::transitionToStageB(ScoreSystem& score) {
    lvl25.stage = 1; // Stage B: Search Demonstration
    lvl25.searchTarget = 70;
    lvl25.searchBudget = 3; // ceil(log2(7+1)) = 3
    lvl25.searchComparisonsUsed = 0;
    lvl25.currentSearchNodeKey = 10;
    lvl25.searchFailed = false;

    // Reset score alarm meter for educational search stage without affecting saved Stage A record
    score.reset(3, 3);
}

bool Act2Controller::handleLevel25SearchClick(int clickedKey, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl25.stage != 1) return false;

    if (clickedKey == lvl25.currentSearchNodeKey) {
        lvl25.searchComparisonsUsed++;
        score.addAlarmStep(1);

        if (lvl25.searchComparisonsUsed >= lvl25.searchBudget) {
            // Search budget exhausted!
            lvl25.searchFailed = true;
            transitionToStageC(score);
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "SEARCH ALARM FULL! Reached budget of 3 comparisons at pod %d before finding %d.",
                         clickedKey, lvl25.searchTarget);
            }
            return true;
        } else {
            // Advance down the corridor
            lvl25.currentSearchNodeKey += 10;
            if (feedback && maxLen > 0) {
                snprintf(feedback, maxLen, "Comparison %d/3: %d < 70 -> Move to right child pod %d. %d comparisons remaining.",
                         lvl25.searchComparisonsUsed, clickedKey, lvl25.currentSearchNodeKey,
                         lvl25.searchBudget - lvl25.searchComparisonsUsed);
            }
            return true;
        }
    } else {
        if (feedback && maxLen > 0) {
            snprintf(feedback, maxLen, "Follow the skewed chain: click next pod %d.", lvl25.currentSearchNodeKey);
        }
        return false;
    }
}

void Act2Controller::transitionToStageC(ScoreSystem& score) {
    lvl25.stage = 2; // Stage C: Explanation & AVL Reveal

    // Build balanced AVL tree from the exact same values
    while (!lvl25.avlTree.isEmpty()) {
        lvl25.avlTree.deleteNode(lvl25.avlTree.getRoot()->key);
    }
    for (int i = 0; i < lvl25.totalCount; i++) {
        lvl25.avlTree.insert(lvl25.sortedValues[i]);
    }

    completeLevel25(score);
}

void Act2Controller::completeLevel25(ScoreSystem& score) {
    lvl25.isLevelComplete = true;
    lvl25.stage = 3;

    // Restore official Level 2.5 score and stars from frozen Stage A insertion record
    score.alarmSteps = lvl25.insertionRecord.alarmSteps;
    score.penalties = lvl25.insertionRecord.penalties;
    score.currentScore = lvl25.insertionRecord.score;
}
