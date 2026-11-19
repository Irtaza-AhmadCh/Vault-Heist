#include "UniversalActController.h"
#include <cstring>

UniversalActController::UniversalActController() {}
UniversalActController::~UniversalActController() {
    delete bst;
    delete avl;
    delete heap;
}

void UniversalActController::initLevel(const LevelDef& lvl, ScoreSystem& sc) {
    // Clean up any previous structure
    delete bst; bst = nullptr;
    delete avl; avl = nullptr;
    delete heap; heap = nullptr;

    currentLevel = &lvl;
    scoreSystem = &sc;

    // Instantiate the appropriate data structure based on lvl.structureType
    if (strcmp(lvl.structureType, "bst") == 0) {
        bst = new BST();
    } else if (strcmp(lvl.structureType, "avl") == 0) {
        avl = new AVLTree(); // placeholder – actual class name may differ
    } else if (strcmp(lvl.structureType, "heap") == 0) {
        heap = new MaxHeap(); // placeholder – actual class name may differ
    }
    // For "tree" (Act 1) we could reuse a BinaryTree, but the universal controller
    // currently focuses on Act 2 (BST). Other acts can be added later.
}

bool UniversalActController::handleAction(const char* action, int value, char* feedback, int maxLen) {
    if (!currentLevel || !scoreSystem) return false;
    // Dispatch based on the required action for Act 2 (BST).
    if (strcmp(currentLevel->structureType, "bst") != 0) {
        // For now we only support BST actions.
        return false;
    }
    if (strcmp(action, "search") == 0) {
        return act2Search(value, feedback, maxLen);
    } else if (strcmp(action, "insert") == 0) {
        return act2Insert(value, feedback, maxLen);
    } else if (strcmp(action, "delete") == 0) {
        return act2Delete(value, feedback, maxLen);
    } else if (strcmp(action, "range") == 0) {
        // Expect low in value, high in a second call – for simplicity we assume
        // the caller packs lo in high 16 bits and hi in low 16 bits.
        int lo = (value >> 16) & 0xFFFF;
        int hi = value & 0xFFFF;
        return act2Range(lo, hi, feedback, maxLen);
    } else if (strcmp(action, "boss_insert") == 0) {
        return act2BossInsert(value, feedback, maxLen);
    } else if (strcmp(action, "boss_search") == 0) {
        return act2BossSearch(value, feedback, maxLen);
    }
    return false;
}

// ---------- Act 2 helpers ---------------------------------------------------

bool UniversalActController::act2Search(int target, char* fb, int maxLen) {
    // Perform a BST search, counting visits/comparisons.
    if (!bst) return false;
    // Simple wrapper – BST::search returns bool but we need to emit events.
    // For now we just count a single alarm step per comparison/visit.
    // The actual visual events are emitted elsewhere (side‑car). Here we only
    // update the score.
    recordAlarm(1); // visiting root (first step)
    bool found = bst->search(target);
    if (found) {
        if (fb && maxLen > 0) snprintf(fb, maxLen, "Target %d found.", target);
    } else {
        if (fb && maxLen > 0) snprintf(fb, maxLen, "Target %d not present.", target);
    }
    return true;
}

bool UniversalActController::act2Insert(int key, char* fb, int maxLen) {
    if (!bst) return false;
    // Duplicate detection – BST::insert will silently ignore duplicates; we must
    // enforce the rule.
    if (bst->search(key)) {
        recordPenalty(0, 1);
        if (fb && maxLen > 0) snprintf(fb, maxLen, "Duplicate key %d rejected.", key);
        return false;
    }
    bst->insert(key);
    recordAlarm(1); // insertion counts as one alarm step
    if (fb && maxLen > 0) snprintf(fb, maxLen, "Inserted %d.", key);
    return true;
}

bool UniversalActController::act2Delete(int key, char* fb, int maxLen) {
    if (!bst) return false;
    if (!bst->search(key)) {
        recordPenalty(0, 1);
        if (fb && maxLen > 0) snprintf(fb, maxLen, "Key %d not found for deletion.", key);
        return false;
    }
    bst->deleteNode(key);
    recordAlarm(1); // deletion counts as one alarm step
    if (fb && maxLen > 0) snprintf(fb, maxLen, "Deleted %d.", key);
    return true;
}

bool UniversalActController::act2Range(int lo, int hi, char* fb, int maxLen) {
    if (!bst) return false;
    // Range query – each visited node counts as a step.
    // For simplicity we just call the model method and assume it prints.
    bst->rangeQuery(lo, hi);
    recordAlarm(1); // count the whole operation as one step (could be refined).
    if (fb && maxLen > 0) snprintf(fb, maxLen, "Range [%d,%d] executed.", lo, hi);
    return true;
}

bool UniversalActController::act2BossInsert(int key, char* fb, int maxLen) {
    // Boss Stage A – sorted insertion challenge.
    return act2Insert(key, fb, maxLen); // same logic, but bonus handled later.
}

bool UniversalActController::act2BossSearch(int target, char* fb, int maxLen) {
    // Boss Stage B – search with a limited budget.
    // The budget check is done by the caller (LevelManager) via alarm capacity.
    return act2Search(target, fb, maxLen);
}

void UniversalActController::recordAlarm(int steps) {
    if (scoreSystem) scoreSystem->addAlarmStep(steps);
}

void UniversalActController::recordPenalty(int penaltyScore, int alarmPenalty) {
    if (scoreSystem) scoreSystem->addPenalty(penaltyScore, alarmPenalty);
}
