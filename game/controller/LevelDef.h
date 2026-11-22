#ifndef LEVEL_DEF_H
#define LEVEL_DEF_H

#include <cstring>

struct LevelDef {
    static const int MAX_MAP_VALUES = 32;
    static const int MAX_STAGES = 8;
    char id[32];
    int order;
    int actNumber;          // 1, 2, 3, 4
    int levelNumber;        // 1 to 5 (e.g. 1.1 to 1.5)
    char title[64];         // e.g. "Level 1.1: Neural Traversal"
    char actName[64];       // e.g. "The Blueprint Wing"
    char structureType[16]; // "tree", "bst", "avl", "heap"
    char objective[160];    // e.g. "Perform Inorder Traversal to extract neural sequence"
    char hint[160];         // e.g. "Inorder visits left child, root, then right child."
    char requiredAction[32];// "inorder", "search", "insert", "delete", etc.
    char gameplayType[32];  // centralized, guardrail-approved handler identifier
    int targetValue;        // e.g. 45 or key to operate on
    int initialValues[MAX_MAP_VALUES];
    int initialValueCount;
    int requiredValues[MAX_MAP_VALUES];
    int requiredValueCount;
    int stageTargets[MAX_STAGES];
    int stageTargetCount;
    int secondaryTargetValue;
    int optimalSteps;       // calculated by LevelManager, never read from JSON
    int alarmCapacity;      // calculated from optimalSteps
    bool isUnlocked;
    int starsEarned;        // 0 to 3
    int bestScore;
    bool isCompleted;

    LevelDef() {
        id[0] = '\0';
        order = 0;
        actNumber = 1;
        levelNumber = 1;
        title[0] = '\0';
        actName[0] = '\0';
        structureType[0] = '\0';
        objective[0] = '\0';
        hint[0] = '\0';
        requiredAction[0] = '\0';
        gameplayType[0] = '\0';
        targetValue = 0;
        initialValueCount = 0;
        requiredValueCount = 0;
        stageTargetCount = 0;
        secondaryTargetValue = 0;
        for (int i = 0; i < MAX_MAP_VALUES; i++) {
            initialValues[i] = 0;
            requiredValues[i] = 0;
        }
        for (int i = 0; i < MAX_STAGES; i++) stageTargets[i] = 0;
        optimalSteps = 0;
        alarmCapacity = 0;
        isUnlocked = false;
        starsEarned = 0;
        bestScore = 0;
        isCompleted = false;
    }

    LevelDef(int act, int lvl, const char* t, const char* aName, const char* sType,
             const char* obj, const char* h, const char* reqAct, int targetVal, int optSteps) {
        actNumber = act;
        levelNumber = lvl;
        strncpy(title, t, sizeof(title) - 1); title[sizeof(title) - 1] = '\0';
        strncpy(actName, aName, sizeof(actName) - 1); actName[sizeof(actName) - 1] = '\0';
        strncpy(structureType, sType, sizeof(structureType) - 1); structureType[sizeof(structureType) - 1] = '\0';
        strncpy(objective, obj, sizeof(objective) - 1); objective[sizeof(objective) - 1] = '\0';
        strncpy(hint, h, sizeof(hint) - 1); hint[sizeof(hint) - 1] = '\0';
        strncpy(requiredAction, reqAct, sizeof(requiredAction) - 1); requiredAction[sizeof(requiredAction) - 1] = '\0';
        targetValue = targetVal;
        optimalSteps = optSteps;
        alarmCapacity = (3 * optSteps + 1) / 2 + 2;
        isUnlocked = (act == 1 && lvl == 1); // First level unlocked by default
        starsEarned = 0;
        bestScore = 0;
        isCompleted = false;
    }
};

#endif
