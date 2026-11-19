#ifndef LEVEL_DEF_H
#define LEVEL_DEF_H

#include <cstring>

struct LevelDef {
    int actNumber;          // 1, 2, 3, 4
    int levelNumber;        // 1 to 5 (e.g. 1.1 to 1.5)
    char title[64];         // e.g. "Level 1.1: Neural Traversal"
    char actName[64];       // e.g. "The Blueprint Wing"
    char structureType[16]; // "tree", "bst", "avl", "heap"
    char objective[160];    // e.g. "Perform Inorder Traversal to extract neural sequence"
    char hint[160];         // e.g. "Inorder visits left child, root, then right child."
    char requiredAction[32];// "inorder", "search", "insert", "delete", etc.
    int targetValue;        // e.g. 45 or key to operate on
    int optimalSteps;       // baseline steps for 3 stars
    int alarmCapacity;      // ceil(1.5 * optimalSteps) + 2
    bool isUnlocked;
    int starsEarned;        // 0 to 3
    int bestScore;
    bool isCompleted;

    LevelDef() {
        actNumber = 1;
        levelNumber = 1;
        title[0] = '\0';
        actName[0] = '\0';
        structureType[0] = '\0';
        objective[0] = '\0';
        hint[0] = '\0';
        requiredAction[0] = '\0';
        targetValue = 0;
        optimalSteps = 5;
        alarmCapacity = 10;
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
        alarmCapacity = static_cast<int>(1.5f * optSteps + 0.999f) + 2;
        isUnlocked = (act == 1 && lvl == 1); // First level unlocked by default
        starsEarned = 0;
        bestScore = 0;
        isCompleted = false;
    }
};

#endif
