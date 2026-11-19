#include "Act1State.h"
#include <cstdio>
#include <cstring>

Act1Controller::Act1Controller() {
    activeLevelNumber = 1;
    currentTree = nullptr;
}

void Act1Controller::initLevel(int levelNum, BinaryTree* tree, ScoreSystem& score) {
    activeLevelNumber = levelNum;
    currentTree = tree;

    if (levelNum == 1) {
        lvl11.currentStage = 0;
        lvl11.visitedCount = 0;
        lvl11.isLevelComplete = false;

        // Stage 0: Preorder
        strncpy(lvl11.stages[0].name, "preorder", sizeof(lvl11.stages[0].name));
        strncpy(lvl11.stages[0].displayName, "Stage 1/4: Preorder (Root -> Left -> Right)", sizeof(lvl11.stages[0].displayName));
        int s0[7] = {50, 25, 12, 37, 75, 62, 88};
        for (int i = 0; i < 7; i++) lvl11.stages[0].expectedValues[i] = s0[i];
        lvl11.stages[0].expectedCount = 7;
        lvl11.stages[0].currentIndex = 0;
        lvl11.stages[0].completed = false;

        // Stage 1: Postorder
        strncpy(lvl11.stages[1].name, "postorder", sizeof(lvl11.stages[1].name));
        strncpy(lvl11.stages[1].displayName, "Stage 2/4: Postorder (Left -> Right -> Root)", sizeof(lvl11.stages[1].displayName));
        int s1[7] = {10, 30, 20, 50, 70, 60, 40};
        for (int i = 0; i < 7; i++) lvl11.stages[1].expectedValues[i] = s1[i];
        lvl11.stages[1].expectedCount = 7;
        lvl11.stages[1].currentIndex = 0;
        lvl11.stages[1].completed = false;

        // Stage 2: Inorder
        strncpy(lvl11.stages[2].name, "inorder", sizeof(lvl11.stages[2].name));
        strncpy(lvl11.stages[2].displayName, "Stage 3/4: Inorder (Left -> Root -> Right)", sizeof(lvl11.stages[2].displayName));
        int s2[7] = {11, 22, 33, 45, 55, 68, 77};
        for (int i = 0; i < 7; i++) lvl11.stages[2].expectedValues[i] = s2[i];
        lvl11.stages[2].expectedCount = 7;
        lvl11.stages[2].currentIndex = 0;
        lvl11.stages[2].completed = false;

        // Stage 3: Level-order
        strncpy(lvl11.stages[3].name, "level_order", sizeof(lvl11.stages[3].name));
        strncpy(lvl11.stages[3].displayName, "Stage 4/4: Level-Order (Breadth-First)", sizeof(lvl11.stages[3].displayName));
        int s3[7] = {52, 26, 78, 13, 39, 65, 91};
        for (int i = 0; i < 7; i++) lvl11.stages[3].expectedValues[i] = s3[i];
        lvl11.stages[3].expectedCount = 7;
        lvl11.stages[3].currentIndex = 0;
        lvl11.stages[3].completed = false;

        score.reset(28, 44);
        loadStageTree(0);
    } else if (levelNum == 2) {
        lvl12.currentStage = 0;
        lvl12.selectedTokenIndex = -1;
        lvl12.isLevelComplete = false;

        strncpy(lvl12.stageName, "Stage 1/2: Preorder + Inorder", sizeof(lvl12.stageName));
        int pre[7] = {50, 25, 12, 37, 75, 62, 88};
        int in[7]  = {12, 25, 37, 50, 62, 75, 88};
        lvl12.seqLen = 7;
        for (int i = 0; i < 7; i++) {
            lvl12.preSeq[i] = pre[i];
            lvl12.inSeq[i]  = in[i];
        }

        targetTree.reconstructFromPreIn(pre, in, 7);

        lvl12.tokenCount = 7;
        for (int i = 0; i < 7; i++) {
            lvl12.tokens[i].value = in[i];
            lvl12.tokens[i].placed = false;
            lvl12.tokens[i].trayX = 260.0f + i * 110.0f;
            lvl12.tokens[i].trayY = 620.0f;
        }

        lvl12.slotCount = 7;
        lvl12.slots[0] = {0, -1, false, true, false, -1, 50, 640.0f, 190.0f};
        lvl12.slots[1] = {1,  0, true,  false, false, -1, 25, 490.0f, 280.0f};
        lvl12.slots[2] = {2,  0, false, false, false, -1, 75, 790.0f, 280.0f};
        lvl12.slots[3] = {3,  1, true,  false, false, -1, 12, 415.0f, 370.0f};
        lvl12.slots[4] = {4,  1, false, false, false, -1, 37, 565.0f, 370.0f};
        lvl12.slots[5] = {5,  2, true,  false, false, -1, 62, 715.0f, 370.0f};
        lvl12.slots[6] = {6,  2, false, false, false, -1, 88, 865.0f, 370.0f};

        if (currentTree != nullptr) {
            currentTree->destroyTree(currentTree->root);
            currentTree->root = nullptr;
        }

        score.reset(14, 23);
    } else if (levelNum == 3) {
        strncpy(lvl13.subState, "select_node", sizeof(lvl13.subState));
        int po[7] = {12, 37, 25, 62, 88, 75, 50};
        int eh[7] = {0, 0, 1, 0, 0, 1, 2};
        lvl13.nodeCount = 7;
        lvl13.currentPostorderIdx = 0;
        lvl13.activeValue = -1;
        lvl13.expectedDiameter = 4;
        lvl13.enteredDiameter = -1;
        lvl13.isLevelComplete = false;

        for (int i = 0; i < 7; i++) {
            lvl13.postorderValues[i] = po[i];
            lvl13.expectedHeights[i] = eh[i];
            lvl13.enteredHeights[i] = -1;
        }

        if (currentTree != nullptr) {
            currentTree->destroyTree(currentTree->root);
            currentTree->root = new TreeNode(50);
            currentTree->root->left = new TreeNode(25);
            currentTree->root->right = new TreeNode(75);
            currentTree->root->left->left = new TreeNode(12);
            currentTree->root->left->right = new TreeNode(37);
            currentTree->root->right->left = new TreeNode(62);
            currentTree->root->right->right = new TreeNode(88);
        }

        score.reset(7, 13);
    } else if (levelNum == 4) {
        strncpy(lvl14.phase, "select_lca", sizeof(lvl14.phase));
        lvl14.nodeA = 12;
        lvl14.nodeB = 62;
        lvl14.expectedLCA = 50;
        lvl14.selectedLCA = -1;
        lvl14.pathLength = 4;
        int ep[5] = {12, 25, 50, 75, 62};
        for (int i = 0; i < 5; i++) lvl14.expectedPath[i] = ep[i];
        lvl14.tracedCount = 1;
        lvl14.tracedPath[0] = 12;
        lvl14.currentTraceIdx = 1;
        lvl14.isLevelComplete = false;

        if (currentTree != nullptr) {
            currentTree->destroyTree(currentTree->root);
            currentTree->root = new TreeNode(50);
            currentTree->root->left = new TreeNode(25);
            currentTree->root->right = new TreeNode(75);
            currentTree->root->left->left = new TreeNode(12);
            currentTree->root->left->right = new TreeNode(37);
            currentTree->root->right->left = new TreeNode(62);
            currentTree->root->right->right = new TreeNode(88);
        }

        score.reset(5, 10);
    } else if (levelNum == 5) {
        lvl15.nodeCount = 7;
        lvl15.isLevelComplete = false;
        int vals[7] = {12, 37, 25, 62, 88, 75, 50};
        for (int i = 0; i < 7; i++) {
            lvl15.nodes[i].value = vals[i];
            lvl15.nodes[i].swapped = false;
        }

        if (currentTree != nullptr) {
            currentTree->destroyTree(currentTree->root);
            currentTree->root = new TreeNode(50);
            currentTree->root->left = new TreeNode(25);
            currentTree->root->right = new TreeNode(75);
            currentTree->root->left->left = new TreeNode(12);
            currentTree->root->left->right = new TreeNode(37);
            currentTree->root->right->left = new TreeNode(62);
            currentTree->root->right->right = new TreeNode(88);
        }

        targetTree.destroyTree(targetTree.root);
        targetTree.root = currentTree->cloneTree(currentTree->root);
        targetTree.mirror(targetTree.root);

        score.reset(7, 13);
    }
}

void Act1Controller::loadStageTree(int stageIdx) {
    if (currentTree == nullptr) return;
    currentTree->destroyTree(currentTree->root);

    if (stageIdx == 0) {
        currentTree->root = new TreeNode(50);
        currentTree->root->left = new TreeNode(25);
        currentTree->root->right = new TreeNode(75);
        currentTree->root->left->left = new TreeNode(12);
        currentTree->root->left->right = new TreeNode(37);
        currentTree->root->right->left = new TreeNode(62);
        currentTree->root->right->right = new TreeNode(88);
    } else if (stageIdx == 1) {
        currentTree->root = new TreeNode(40);
        currentTree->root->left = new TreeNode(20);
        currentTree->root->right = new TreeNode(60);
        currentTree->root->left->left = new TreeNode(10);
        currentTree->root->left->right = new TreeNode(30);
        currentTree->root->right->left = new TreeNode(50);
        currentTree->root->right->right = new TreeNode(70);
    } else if (stageIdx == 2) {
        currentTree->root = new TreeNode(45);
        currentTree->root->left = new TreeNode(22);
        currentTree->root->right = new TreeNode(68);
        currentTree->root->left->left = new TreeNode(11);
        currentTree->root->left->right = new TreeNode(33);
        currentTree->root->right->left = new TreeNode(55);
        currentTree->root->right->right = new TreeNode(77);
    } else if (stageIdx == 3) {
        currentTree->root = new TreeNode(52);
        currentTree->root->left = new TreeNode(26);
        currentTree->root->right = new TreeNode(78);
        currentTree->root->left->left = new TreeNode(13);
        currentTree->root->left->right = new TreeNode(39);
        currentTree->root->right->left = new TreeNode(65);
        currentTree->root->right->right = new TreeNode(91);
    }
}

bool Act1Controller::handleLevel11Click(int clickedValue, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl11.isLevelComplete) return true;

    TraversalStage& cur = lvl11.stages[lvl11.currentStage];
    int expected = cur.expectedValues[cur.currentIndex];

    if (clickedValue == expected) {
        lvl11.visitedValues[lvl11.visitedCount++] = clickedValue;
        cur.currentIndex++;
        score.addAlarmStep(1);

        if (cur.currentIndex >= cur.expectedCount) {
            cur.completed = true;
            if (lvl11.currentStage < 3) {
                lvl11.currentStage++;
                lvl11.visitedCount = 0;
                loadStageTree(lvl11.currentStage);
                snprintf(feedback, maxLen, "Stage complete! Progressing to %s.", lvl11.stages[lvl11.currentStage].displayName);
            } else {
                lvl11.isLevelComplete = true;
                snprintf(feedback, maxLen, "PATROL COMPLETE: All 4 traversals mapped perfectly!");
            }
        } else {
            snprintf(feedback, maxLen, "Correct pod %d! Next impulse expected in patrol order.", clickedValue);
        }
        return true;
    } else {
        score.addPenalty(0, 1);
        snprintf(feedback, maxLen, "PATROL ALARM: Expected pod %d, but pod %d was clicked! +1 Alarm penalty.", expected, clickedValue);
        return false;
    }
}

bool Act1Controller::selectToken(int tokenIdx) {
    if (tokenIdx < 0 || tokenIdx >= lvl12.tokenCount) return false;
    if (lvl12.tokens[tokenIdx].placed) return false;
    lvl12.selectedTokenIndex = tokenIdx;
    return true;
}

bool Act1Controller::placeTokenInSlot(int slotIdx, ScoreSystem& score, char* feedback, int maxLen) {
    if (slotIdx < 0 || slotIdx >= lvl12.slotCount) return false;
    if (lvl12.selectedTokenIndex < 0 || lvl12.selectedTokenIndex >= lvl12.tokenCount) {
        snprintf(feedback, maxLen, "Select a token from the tray first!");
        return false;
    }

    ReconSlot& slot = lvl12.slots[slotIdx];
    ReconToken& tok = lvl12.tokens[lvl12.selectedTokenIndex];

    if (!slot.isAvailable || slot.isOccupied) {
        snprintf(feedback, maxLen, "That tree slot is not available for placement.");
        return false;
    }

    if (tok.value == slot.targetValue) {
        slot.isOccupied = true;
        slot.placedValue = tok.value;
        tok.placed = true;
        lvl12.selectedTokenIndex = -1;
        score.addAlarmStep(1);

        for (int i = 0; i < lvl12.slotCount; i++) {
            if (lvl12.slots[i].parentSlot == slotIdx) {
                lvl12.slots[i].isAvailable = true;
            }
        }

        // Rebuild current tree from placed slots
        if (slotIdx == 0) {
            currentTree->destroyTree(currentTree->root);
            currentTree->root = new TreeNode(tok.value);
        } else {
            TreeNode* parent = currentTree->findNode(currentTree->root, lvl12.slots[slot.parentSlot].placedValue);
            if (parent != nullptr) {
                if (slot.isLeft) parent->left = new TreeNode(tok.value);
                else parent->right = new TreeNode(tok.value);
            }
        }

        checkReconstructionComplete(score);
        snprintf(feedback, maxLen, "Pod %d grafted into blueprint position!", tok.value);
        return true;
    } else {
        score.addPenalty(0, 1);
        lvl12.selectedTokenIndex = -1;
        snprintf(feedback, maxLen, "RECONSTRUCTION MISMATCH: Pod %d violates traversal constraints! +1 Alarm.", tok.value);
        return false;
    }
}

bool Act1Controller::checkReconstructionComplete(ScoreSystem& score) {
    for (int i = 0; i < lvl12.slotCount; i++) {
        if (!lvl12.slots[i].isOccupied) return false;
    }

    if (lvl12.currentStage == 0) {
        lvl12.currentStage = 1;
        strncpy(lvl12.stageName, "Stage 2/2: Postorder + Inorder", sizeof(lvl12.stageName));

        int post[7] = {10, 30, 20, 50, 70, 60, 40};
        int in[7]   = {10, 20, 30, 40, 50, 60, 70};
        for (int i = 0; i < 7; i++) {
            lvl12.postSeq[i] = post[i];
            lvl12.inSeq[i]   = in[i];
        }

        targetTree.reconstructFromPostIn(post, in, 7);

        for (int i = 0; i < 7; i++) {
            lvl12.tokens[i].value = in[i];
            lvl12.tokens[i].placed = false;
        }

        lvl12.slots[0] = {0, -1, false, true, false, -1, 40, 640.0f, 190.0f};
        lvl12.slots[1] = {1,  0, true,  false, false, -1, 20, 490.0f, 280.0f};
        lvl12.slots[2] = {2,  0, false, false, false, -1, 60, 790.0f, 280.0f};
        lvl12.slots[3] = {3,  1, true,  false, false, -1, 10, 415.0f, 370.0f};
        lvl12.slots[4] = {4,  1, false, false, false, -1, 30, 565.0f, 370.0f};
        lvl12.slots[5] = {5,  2, true,  false, false, -1, 50, 715.0f, 370.0f};
        lvl12.slots[6] = {6,  2, false, false, false, -1, 70, 865.0f, 370.0f};

        currentTree->destroyTree(currentTree->root);
        currentTree->root = nullptr;
        return true;
    } else {
        lvl12.isLevelComplete = true;
        return true;
    }
}

bool Act1Controller::handleLevel13NodeClick(int clickedValue, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl13.isLevelComplete) return true;

    if (strcmp(lvl13.subState, "select_node") != 0 && strcmp(lvl13.subState, "enter_height") != 0) {
        snprintf(feedback, maxLen, "All node heights calculated. Enter the tree diameter in the box.");
        return false;
    }

    int expected = lvl13.postorderValues[lvl13.currentPostorderIdx];
    if (clickedValue == expected) {
        lvl13.activeValue = clickedValue;
        strncpy(lvl13.subState, "enter_height", sizeof(lvl13.subState));
        snprintf(feedback, maxLen, "Selected Pod %d. Enter height (edges to deepest leaf) in input box and submit.", clickedValue);
        return true;
    } else {
        score.addPenalty(0, 1);
        snprintf(feedback, maxLen, "WRONG NODE: Traversal must be Postorder (Left -> Right -> Root). Expected pod %d.", expected);
        return false;
    }
}

bool Act1Controller::submitLevel13Input(int inputVal, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl13.isLevelComplete) return true;

    if (strcmp(lvl13.subState, "enter_height") == 0) {
        int expectedH = lvl13.expectedHeights[lvl13.currentPostorderIdx];
        if (inputVal == expectedH) {
            lvl13.enteredHeights[lvl13.currentPostorderIdx] = inputVal;
            score.addAlarmStep(1);
            lvl13.currentPostorderIdx++;

            if (lvl13.currentPostorderIdx >= lvl13.nodeCount) {
                strncpy(lvl13.subState, "enter_diameter", sizeof(lvl13.subState));
                snprintf(feedback, maxLen, "All heights verified! Now enter Tree Diameter (longest path in edges):");
            } else {
                strncpy(lvl13.subState, "select_node", sizeof(lvl13.subState));
                snprintf(feedback, maxLen, "Height %d verified! Click the next pod in Postorder.", inputVal);
            }
            return true;
        } else {
            score.addPenalty(0, 1);
            snprintf(feedback, maxLen, "INCORRECT HEIGHT: Pod %d has height %d, not %d! (leaf=0, parent=1+max).",
                     lvl13.activeValue, expectedH, inputVal);
            return false;
        }
    } else if (strcmp(lvl13.subState, "enter_diameter") == 0) {
        if (inputVal == lvl13.expectedDiameter) {
            lvl13.enteredDiameter = inputVal;
            strncpy(lvl13.subState, "complete", sizeof(lvl13.subState));
            lvl13.isLevelComplete = true;
            snprintf(feedback, maxLen, "DIAMETER VERIFIED: Longest path = %d edges. Hive metrics cleared!", inputVal);
            return true;
        } else {
            score.addPenalty(0, 1);
            snprintf(feedback, maxLen, "INCORRECT DIAMETER: Tree diameter is %d edges, not %d.",
                     lvl13.expectedDiameter, inputVal);
            return false;
        }
    }

    return false;
}

bool Act1Controller::handleLevel14Click(int clickedValue, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl14.isLevelComplete) return true;

    if (strcmp(lvl14.phase, "select_lca") == 0) {
        if (clickedValue == lvl14.expectedLCA) {
            lvl14.selectedLCA = clickedValue;
            strncpy(lvl14.phase, "trace_path", sizeof(lvl14.phase));
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "LCA %d IDENTIFIED! Now trace the path from 12 to 62: click next pod (%d).",
                     clickedValue, lvl14.expectedPath[lvl14.currentTraceIdx]);
            return true;
        } else {
            score.addPenalty(0, 1);
            snprintf(feedback, maxLen, "INCORRECT LCA: Pod %d is not the lowest common ancestor of 12 and 62! +1 Alarm.", clickedValue);
            return false;
        }
    } else if (strcmp(lvl14.phase, "trace_path") == 0) {
        int expectedNext = lvl14.expectedPath[lvl14.currentTraceIdx];
        if (clickedValue == expectedNext) {
            lvl14.tracedPath[lvl14.tracedCount++] = clickedValue;
            lvl14.currentTraceIdx++;
            score.addAlarmStep(1);

            if (lvl14.currentTraceIdx >= 5) {
                strncpy(lvl14.phase, "complete", sizeof(lvl14.phase));
                lvl14.isLevelComplete = true;
                snprintf(feedback, maxLen, "ESCAPE ROUTE ESTABLISHED: Path length = 4 edges. Extraction complete!");
            } else {
                snprintf(feedback, maxLen, "Path pod %d illuminated. Click next pod (%d).",
                         clickedValue, lvl14.expectedPath[lvl14.currentTraceIdx]);
            }
            return true;
        } else {
            score.addPenalty(0, 1);
            snprintf(feedback, maxLen, "ROUTE DEVIATION: Pod %d is not next on path! Expected pod %d. +1 Alarm.",
                     clickedValue, expectedNext);
            return false;
        }
    }

    return false;
}

bool Act1Controller::isNodeSwapped(int val) {
    for (int i = 0; i < lvl15.nodeCount; i++) {
        if (lvl15.nodes[i].value == val) return lvl15.nodes[i].swapped;
    }
    return false;
}

void Act1Controller::markNodeSwapped(int val) {
    for (int i = 0; i < lvl15.nodeCount; i++) {
        if (lvl15.nodes[i].value == val) {
            lvl15.nodes[i].swapped = true;
            return;
        }
    }
}

bool Act1Controller::isNodeEligibleForSwap(int nodeVal) {
    if (isNodeSwapped(nodeVal)) return false;
    TreeNode* node = currentTree->findNode(currentTree->root, nodeVal);
    if (node == nullptr) return false;

    // Bottom-up rule: children must already be swapped
    if (node->left != nullptr && !isNodeSwapped(node->left->data)) return false;
    if (node->right != nullptr && !isNodeSwapped(node->right->data)) return false;

    return true;
}

bool Act1Controller::handleLevel15Swap(int nodeVal, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl15.isLevelComplete) return true;

    if (isNodeSwapped(nodeVal)) {
        snprintf(feedback, maxLen, "Pod %d has already been inverted.", nodeVal);
        return false;
    }

    if (!isNodeEligibleForSwap(nodeVal)) {
        score.addPenalty(0, 1);
        snprintf(feedback, maxLen, "BOTTOM-UP VIOLATION: Children of pod %d must be mirrored before the parent! +1 Alarm.", nodeVal);
        return false;
    }

    TreeNode* node = currentTree->findNode(currentTree->root, nodeVal);
    if (node == nullptr) return false;

    TreeNode* temp = node->left;
    node->left = node->right;
    node->right = temp;

    markNodeSwapped(nodeVal);
    score.addAlarmStep(1);

    if (currentTree->isSameTree(currentTree->root, targetTree.root)) {
        lvl15.isLevelComplete = true;
        snprintf(feedback, maxLen, "BILATERAL REFLECTION ACHIEVED: Tree matches expected mirror!");
    } else {
        snprintf(feedback, maxLen, "Swapped branches of pod %d. Continue bottom-up inversion.", nodeVal);
    }
    return true;
}

bool Act1Controller::handleAutoMirror(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl15.isLevelComplete) return true;

    currentTree->mirror(currentTree->root);
    for (int i = 0; i < lvl15.nodeCount; i++) {
        lvl15.nodes[i].swapped = true;
    }
    score.addAlarmStep(7);

    if (currentTree->isSameTree(currentTree->root, targetTree.root)) {
        lvl15.isLevelComplete = true;
        snprintf(feedback, maxLen, "AUTO-MIRROR EXECUTED: Vault structure completely inverted!");
        return true;
    }
    return false;
}
