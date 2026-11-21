#include <cassert>
#include <cstring>
#include <cstdio>
#include "../game/controller/LevelManager.h"
#include "../game/logic/SaveManager.h"

int main() {
    LevelManager manager;
    char error[192] = {0};
    assert(manager.loadCampaign("levels/campaign_levels.json", error, sizeof(error)));
    assert(manager.levelCount == 20);
    assert(manager.findLevelIndexById("act1.level1") == 0);
    assert(manager.findLevelIndexById("act4.level5") == 19);

    const LevelDef& traversal = manager.levels[0];
    assert(traversal.optimalSteps == 28);
    assert(traversal.alarmCapacity == 44);
    assert(strcmp(traversal.gameplayType, "traversal_stages") == 0);

    const LevelDef& heapBuild = manager.levels[15];
    assert(heapBuild.optimalSteps == 3);
    assert(heapBuild.alarmCapacity == 7);

    assert(manager.loadLevel(9));
    assert(manager.scoreSystem.optimalSteps == 28);
    assert(manager.scoreSystem.alarmCapacity == 44);

    // Test SaveManager with V1 save format
    assert(SaveManager::loadProgress("levels/savegame.txt", manager.levels, manager.levelCount));
    assert(manager.levels[0].isUnlocked);
    assert(manager.levels[0].isCompleted);
    assert(manager.levels[0].starsEarned == 3);

    // Test saving in V2 format
    const char* testV2Path = "levels/test_v2_save.txt";
    assert(SaveManager::saveProgress(testV2Path, manager.levels, manager.levelCount));

    // Reset levels in memory
    for (int i = 0; i < manager.levelCount; i++) {
        manager.levels[i].isUnlocked = (i == 0);
        manager.levels[i].isCompleted = false;
        manager.levels[i].starsEarned = 0;
        manager.levels[i].bestScore = 0;
    }

    // Load back V2 format
    assert(SaveManager::loadProgress(testV2Path, manager.levels, manager.levelCount));
    assert(manager.levels[0].isUnlocked);
    assert(manager.levels[0].isCompleted);
    assert(manager.levels[0].starsEarned == 3);
    std::remove(testV2Path);

    // Test unlocking next level
    manager.currentLevelIndex = 0;
    manager.unlockNextLevel();
    assert(manager.levels[1].isUnlocked);

    return 0;
}
