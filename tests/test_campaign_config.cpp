#include <cassert>
#include <cstring>
#include <cstdio>
#include "../game/controller/LevelManager.h"
#include "../game/logic/SaveManager.h"
#include "../game/controller/act3-4/Act4State.h"

int main() {
    LevelManager manager;
    char error[192] = {0};
    bool ok = manager.loadCampaign("levels/campaign_levels.json", error, sizeof(error));
    if (!ok) {
        printf("Error: %s\n", error);
    }
    printf("levelCount: %d\n", manager.levelCount);
    assert(ok);
    assert(manager.levelCount == 26);
    assert(manager.findLevelIndexById("act1.level1") == 0);
    assert(manager.findLevelIndexById("act4.level5") == 20);

    const LevelDef& traversal = manager.levels[0];
    assert(traversal.optimalSteps == 28);
    assert(traversal.alarmCapacity == 44);
    assert(strcmp(traversal.gameplayType, "traversal_stages") == 0);

    const LevelDef& heapBuild = manager.levels[16];
    assert(heapBuild.optimalSteps == 3);
    assert(heapBuild.alarmCapacity == 7);

    // Test dynamically added 4.7 (repair) and 4.8 (extract_max)
    int idx7 = manager.findLevelIndexById("act4.level7");
    assert(idx7 == 22);
    const LevelDef& lvl7 = manager.levels[idx7];
    assert(strcmp(lvl7.gameplayType, "heap_repair") == 0);
    assert(strcmp(lvl7.requiredAction, "repair") == 0);
    assert(lvl7.initialValueCount == 5);

    int idx8 = manager.findLevelIndexById("act4.level8");
    assert(idx8 == 23);
    const LevelDef& lvl8 = manager.levels[idx8];
    assert(strcmp(lvl8.gameplayType, "extract_max") == 0);
    assert(strcmp(lvl8.requiredAction, "extract_max") == 0);
    assert(lvl8.initialValueCount == 5);

    // Test Act4Controller initializes heap from lvl7 and lvl8
    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;

    act4.initLevel(lvl7.levelNumber, &heap, score, &lvl7);
    assert(heap.getSize() == 5);
    assert(heap.get(0) == 4);
    assert(act4.lvl42.heapSize == 5);
    assert(act4.lvl42.round == 1);

    act4.initLevel(lvl8.levelNumber, &heap, score, &lvl8);
    assert(heap.getSize() == 5);
    assert(heap.get(0) == 10);
    assert(act4.lvl43.activeHeapSize == 5);

    assert(manager.loadLevel(10));
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
