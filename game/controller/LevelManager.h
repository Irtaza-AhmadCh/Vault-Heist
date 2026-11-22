#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "LevelDef.h"
#include "../scoring/ScoreSystem.h"
class LevelManager {
public:
    static const int MAX_CAMPAIGN_LEVELS = 32;
    static const int TOTAL_LEVELS = MAX_CAMPAIGN_LEVELS;
    LevelDef levels[MAX_CAMPAIGN_LEVELS];
    int levelCount;
    int currentLevelIndex;
    char playState[16]; // "in_progress", "victory", "failed"
    ScoreSystem scoreSystem;
    LevelManager();

    bool loadLevelsFromJson(const char* filepath);
    bool loadCampaign(const char* filepath, char* error, int errorSize);
    bool loadLevel(int index);
    int findLevelIndexById(const char* id) const;
    void notifyAction(const char* actionName, int value = 0);
    bool checkVictory(const char* actionName, int value = 0);

    LevelDef* getCurrentLevel();
    const LevelDef* getCurrentLevel() const;
    int getCurrentAct() const;

    bool unlockNextLevel();
    void resetCurrentLevel();

private:
    bool validateLevel(const LevelDef& level, char* error, int errorSize) const;
    void calculateOptimalSteps(LevelDef& level) const;
    int calculateBstSearchSteps(const LevelDef& level, bool includeInsert) const;
    int calculateHeapBuildSteps(const LevelDef& level) const;
};

#endif

