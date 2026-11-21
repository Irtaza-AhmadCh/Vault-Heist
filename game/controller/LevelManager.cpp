#include "LevelManager.h"
#include "UniversalActController.h"
#include <cstring>
#include <iostream>

LevelManager::LevelManager() {
    currentLevelIndex = -1;
    strncpy(playState, "in_progress", sizeof(playState) - 1);
    initLevels();
}

void LevelManager::initLevels() {
    int idx = 0;

    // ===== ACT 1: The Blueprint Wing (General Binary Tree) =====
    levels[idx++] = LevelDef(1, 1, "Level 1.1: Neural Traversal", "The Blueprint Wing", "tree",
                             "Perform an Inorder Traversal to extract the neural sequence.",
                             "Inorder visits left branch, root pod, then right branch.",
                             "inorder", 0, 28);

    levels[idx++] = LevelDef(1, 2, "Level 1.2: Neural Reconstruction", "The Blueprint Wing", "tree",
                             "Reconstruct the binary tree from traversal blueprints.",
                             "Use the Preorder + Inorder sequences to rebuild the hive.",
                             "reconstruct", 0, 14);

    levels[idx++] = LevelDef(1, 3, "Level 1.3: Hive Metrics", "The Blueprint Wing", "tree",
                             "Calculate and verify Tree Height and Diameter.",
                             "Height measures vertical levels; diameter measures the longest path.",
                             "height_diam", 0, 7);

    levels[idx++] = LevelDef(1, 4, "Level 1.4: Synaptic Convergence (LCA)", "The Blueprint Wing", "tree",
                             "Locate the Lowest Common Ancestor (LCA) and path between pods.",
                             "Find the deepest shared ancestral pod between branches.",
                             "lca", 0, 5);

    levels[idx++] = LevelDef(1, 5, "Level 1.5: Bilateral Reflection", "The Blueprint Wing", "tree",
                             "Invert the hive structure using Tree Mirroring.",
                             "Swaps left and right children recursively across the trunk.",
                             "mirror", 0, 7);

    // ===== ACT 2: The Sorted Wing (BST) =====
    levels[idx++] = LevelDef(2, 1, "Level 2.1: Search Heist", "The Sorted Wing", "bst",
                             "Search for target pod 60 in the BST using binary search comparisons.",
                             "Keys smaller go left; keys larger go right.",
                             "search", 60, 3);

    levels[idx++] = LevelDef(2, 2, "Level 2.2: Stash Loot", "The Sorted Wing", "bst",
                             "Insert key 45 into the BST at the correct leaf position.",
                             "Trace comparisons to find the empty NULL slot.",
                             "insert", 45, 4);

    levels[idx++] = LevelDef(2, 3, "Level 2.3: Steal and Collapse", "The Sorted Wing", "bst",
                             "Delete leaf, one-child, and two-child nodes by identifying each case.",
                             "For a two-child node, identify and swap with the inorder successor.",
                             "delete", 0, 7);

    levels[idx++] = LevelDef(2, 4, "Level 2.4: Range Raid", "The Sorted Wing", "bst",
                             "Execute Range Query [30, 70] and prune non-overlapping subtrees.",
                             "Branches outside the range boundaries are bypassed.",
                             "range", 0, 5);

    levels[idx++] = LevelDef(2, 5, "Level 2.5: Boss: The Lazy Architect", "The Sorted Wing", "bst",
                             "Insert sorted sequence into BST and observe degeneration into a corridor.",
                             "Sorted insertions form a chain, destroying logarithmic efficiency.",
                             "boss", 0, 28);

    // ===== ACT 3: The Architect's Wing (AVL Tree) =====
    levels[idx++] = LevelDef(3, 1, "Level 3.1: Read the Badges", "The Architect's Wing", "avl",
                             "Identify all unbalanced nodes (|BF| > 1) in the vault.",
                             "Inspect badges: green is balanced (-1, 0, +1), red violates AVL invariant.",
                             "search", 0, 2);

    levels[idx++] = LevelDef(3, 2, "Level 3.2: Single Rotations (LL & RR)", "The Architect's Wing", "avl",
                             "Repair LL and RR imbalances using single rotations.",
                             "LL requires Right rotation; RR requires Left rotation.",
                             "rotate", 0, 2);

    levels[idx++] = LevelDef(3, 3, "Level 3.3: Double Rotations (LR & RL)", "The Architect's Wing", "avl",
                             "Identify and repair LR and RL imbalances using double rotations.",
                             "LR is Left-Right double rotation; RL is Right-Left double rotation.",
                             "rotate", 0, 2);

    levels[idx++] = LevelDef(3, 4, "Level 3.4: Auto Mode and Prediction", "The Architect's Wing", "avl",
                             "Predict which rotation will occur before each insertion, then watch auto-rebalance.",
                             "Trace insertion path to determine if BF reaches +2 or -2.",
                             "insert", 0, 5);

    levels[idx++] = LevelDef(3, 5, "Level 3.5: Boss: Order-Statistic Heist", "The Architect's Wing", "avl",
                             "Repair the unbalanced BST, then steal the 3rd largest treasure in O(log N).",
                             "Balance RR at 50, then use subtreeSize to navigate to the 3rd largest node.",
                             "boss", 50, 4);

    // ===== ACT 4: The Heap Fortress (Max Heap) =====
    levels[idx++] = LevelDef(4, 1, "Level 4.1: Activate the Vault", "The Heap Fortress", "heap",
                             "Build a valid Max Heap from raw elements using bottom-up construction.",
                             "Inspect each parent bottom-up; compare with children and swap with the larger child.",
                             "build_heap", 0, 3);

    levels[idx++] = LevelDef(4, 2, "Level 4.2: Repair the Security Locks", "The Heap Fortress", "heap",
                             "Execute Sift-down (Round A) and Sift-up (Round B) to restore heap mechanism.",
                             "Sift down root 4 with larger child; then sift up new treasure 12 to root.",
                             "repair", 0, 4);

    levels[idx++] = LevelDef(4, 3, "Level 4.3: Extract the Greatest Treasure", "The Heap Fortress", "heap",
                             "Repeatedly extract the root maximum and repair remaining heap with sift-down.",
                             "Swap root with last active element, shrink heap size, and sift down new root.",
                             "extract_max", 0, 8);

    levels[idx++] = LevelDef(4, 4, "Level 4.4: Race Against the Guards", "The Heap Fortress", "heap",
                             "Complete 4 extraction/heapify iterations before the alarm threshold triggers lockdown.",
                             "Work efficiently: each extraction requires root/last swap and sift-down.",
                             "race", 0, 7);

    levels[idx++] = LevelDef(4, 5, "Level 4.5: Final Vault Boss", "The Heap Fortress", "heap",
                             "Execute independent full Heap Sort: Phase A build, Phase B extract/sort, Phase C validate.",
                             "Build max heap bottom-up, then perform repeated extractions until array is sorted.",
                             "boss", 0, 12);
}

bool LevelManager::loadLevelsFromJson(const char* filepath) {
    if (filepath == nullptr) return false;
    FILE* f = fopen(filepath, "r");
    if (f == nullptr) return false;

    char buffer[16384];
    size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, f);
    fclose(f);
    buffer[bytesRead] = '\0';

    const char* ptr = buffer;
    int count = 0;
    while (count < TOTAL_LEVELS) {
        const char* lvlToken = strstr(ptr, "\"act\":");
        if (lvlToken == nullptr) break;

        int act = 1, lvlNum = 1, optSteps = 5, targetVal = 0;
        char title[64] = {0};
        char actName[64] = {0};
        char sType[16] = {0};
        char obj[160] = {0};
        char hint[160] = {0};
        char reqAct[32] = {0};

        sscanf(lvlToken, "\"act\": %d", &act);

        const char* lnum = strstr(lvlToken, "\"level\":");
        if (lnum) sscanf(lnum, "\"level\": %d", &lvlNum);

        const char* t = strstr(lvlToken, "\"title\": \"");
        if (t) {
            t += 10;
            const char* end = strchr(t, '\"');
            if (end) {
                int len = static_cast<int>(end - t);
                if (len > 63) len = 63;
                strncpy(title, t, len);
                title[len] = '\0';
            }
        }

        const char* an = strstr(lvlToken, "\"actName\": \"");
        if (an) {
            an += 12;
            const char* end = strchr(an, '\"');
            if (end) {
                int len = static_cast<int>(end - an);
                if (len > 63) len = 63;
                strncpy(actName, an, len);
                actName[len] = '\0';
            }
        }

        const char* st = strstr(lvlToken, "\"structureType\": \"");
        if (st) {
            st += 18;
            const char* end = strchr(st, '\"');
            if (end) {
                int len = static_cast<int>(end - st);
                if (len > 15) len = 15;
                strncpy(sType, st, len);
                sType[len] = '\0';
            }
        }

        const char* ob = strstr(lvlToken, "\"objective\": \"");
        if (ob) {
            ob += 14;
            const char* end = strchr(ob, '\"');
            if (end) {
                int len = static_cast<int>(end - ob);
                if (len > 159) len = 159;
                strncpy(obj, ob, len);
                obj[len] = '\0';
            }
        }

        const char* hn = strstr(lvlToken, "\"hint\": \"");
        if (hn) {
            hn += 9;
            const char* end = strchr(hn, '\"');
            if (end) {
                int len = static_cast<int>(end - hn);
                if (len > 159) len = 159;
                strncpy(hint, hn, len);
                hint[len] = '\0';
            }
        }

        const char* ra = strstr(lvlToken, "\"requiredAction\": \"");
        if (ra) {
            ra += 19;
            const char* end = strchr(ra, '\"');
            if (end) {
                int len = static_cast<int>(end - ra);
                if (len > 31) len = 31;
                strncpy(reqAct, ra, len);
                reqAct[len] = '\0';
            }
        }

        const char* tv = strstr(lvlToken, "\"targetValue\":");
        if (tv) sscanf(tv, "\"targetValue\": %d", &targetVal);

        const char* os = strstr(lvlToken, "\"optimalSteps\":");
        if (os) sscanf(os, "\"optimalSteps\": %d", &optSteps);

        levels[count] = LevelDef(act, lvlNum, title, actName, sType, obj, hint, reqAct, targetVal, optSteps);
        count++;
        ptr = lvlToken + 6;
    }

    return count == TOTAL_LEVELS;
}

bool LevelManager::loadLevel(int index) {
    if (index < 0 || index >= TOTAL_LEVELS) {
        currentLevelIndex = -1;
        return false;
    }
    currentLevelIndex = index;
    strncpy(playState, "in_progress", sizeof(playState) - 1);
    // Reset scoring for this level
    scoreSystem.reset(levels[index].optimalSteps, levels[index].alarmCapacity);
    // Initialise the universal controller for this level
    universalCtrl.initLevel(levels[index], scoreSystem);
    return true;
}

bool LevelManager::checkVictory(const char* actionName, int value) {
    if (currentLevelIndex < 0 || currentLevelIndex >= TOTAL_LEVELS) return false;
    const LevelDef& lvl = levels[currentLevelIndex];

    if (strcmp(lvl.requiredAction, actionName) == 0) {
        if (lvl.targetValue == 0 || lvl.targetValue == value) {
            return true;
        }
    }

    // Secondary matches (e.g. LL vs RR or LR vs RL or kth vs rank)
    if (strcmp(lvl.requiredAction, "boss") == 0 && (strcmp(actionName, "boss") == 0 || strcmp(actionName, "heap_sort") == 0)) return true;
    if (strcmp(lvl.requiredAction, "repair") == 0 && (strcmp(actionName, "repair") == 0 || strcmp(actionName, "sift_down") == 0 || strcmp(actionName, "sift_up") == 0)) return true;
    if (strcmp(lvl.requiredAction, "race") == 0 && (strcmp(actionName, "race") == 0 || strcmp(actionName, "extract_max") == 0)) return true;
    if (strcmp(lvl.requiredAction, "build_heap") == 0 && strcmp(actionName, "build_heap") == 0) return true;
    if (strcmp(lvl.requiredAction, "extract_max") == 0 && strcmp(actionName, "extract_max") == 0) return true;
    if (strcmp(lvl.requiredAction, "rotate") == 0 && strcmp(actionName, "rotate") == 0) return true;
    if (strcmp(lvl.requiredAction, "demo_ll") == 0 && strcmp(actionName, "demo_rr") == 0) return true;
    if (strcmp(lvl.requiredAction, "demo_lr") == 0 && strcmp(actionName, "demo_rl") == 0) return true;
    if (strcmp(lvl.requiredAction, "sift_down") == 0 && strcmp(actionName, "sift_up") == 0) return true;
    if (strcmp(lvl.requiredAction, "kth") == 0 && strcmp(actionName, "rank") == 0) return true;

    return false;
}

void LevelManager::notifyAction(const char* actionName, int value) {
    if (currentLevelIndex < 0 || currentLevelIndex >= TOTAL_LEVELS) return;
    if (strcmp(playState, "in_progress") != 0) return;

    // Forward action to the universal controller (updates alarm, penalties, feedback)
    char feedback[256] = {0};
    universalCtrl.handleAction(actionName, value, feedback, sizeof(feedback));
    // Feedback could be logged or sent to UI; for now we ignore it.

    if (scoreSystem.isAlarmBreached()) {
        strncpy(playState, "failed", sizeof(playState) - 1);
        return;
    }

    if (checkVictory(actionName, value)) {
        strncpy(playState, "victory", sizeof(playState) - 1);
        int stars = scoreSystem.computeStars();
        int score = scoreSystem.computeFinalScore();
        levels[currentLevelIndex].isCompleted = true;
        if (stars > levels[currentLevelIndex].starsEarned) {
            levels[currentLevelIndex].starsEarned = stars;
        }
        if (score > levels[currentLevelIndex].bestScore) {
            levels[currentLevelIndex].bestScore = score;
        }
        unlockNextLevel();
    }
}




LevelDef* LevelManager::getCurrentLevel() {
    if (currentLevelIndex >= 0 && currentLevelIndex < TOTAL_LEVELS) {
        return &levels[currentLevelIndex];
    }
    return nullptr;
}

const LevelDef* LevelManager::getCurrentLevel() const {
    if (currentLevelIndex >= 0 && currentLevelIndex < TOTAL_LEVELS) {
        return &levels[currentLevelIndex];
    }
    return nullptr;
}

int LevelManager::getCurrentAct() const {
    if (currentLevelIndex >= 0 && currentLevelIndex < TOTAL_LEVELS) {
        return levels[currentLevelIndex].actNumber;
    }
    return 1;
}

bool LevelManager::unlockNextLevel() {
    int nextIdx = currentLevelIndex + 1;
    if (nextIdx < TOTAL_LEVELS) {
        levels[nextIdx].isUnlocked = true;
        return true;
    }
    return false;
}

void LevelManager::resetCurrentLevel() {
    if (currentLevelIndex >= 0 && currentLevelIndex < TOTAL_LEVELS) {
        loadLevel(currentLevelIndex);
    }
}

