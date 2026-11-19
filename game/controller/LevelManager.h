#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "LevelDef.h"
#include "../scoring/ScoreSystem.h"
#include "UniversalActController.h"



class LevelManager {
public:
    static const int TOTAL_LEVELS = 20;
    LevelDef levels[TOTAL_LEVELS];
    int currentLevelIndex;
    char playState[16]; // "in_progress", "victory", "failed"
    ScoreSystem scoreSystem;
    // Universal controller for all acts (BST, AVL, Heap, etc.)
    UniversalActController universalCtrl;



    LevelManager();

    void initLevels();
    bool loadLevelsFromJson(const char* filepath);
    bool loadLevel(int index);
    void notifyAction(const char* actionName, int value = 0);
    bool checkVictory(const char* actionName, int value = 0);

    LevelDef* getCurrentLevel();
    const LevelDef* getCurrentLevel() const;
    int getCurrentAct() const;

    bool unlockNextLevel();
    void resetCurrentLevel();
};

#endif

