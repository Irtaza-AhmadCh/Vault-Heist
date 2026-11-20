#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>

#include "../../game/controller/LevelDef.h"
#include "../../game/controller/LevelManager.h"
#include "../../game/scoring/ScoreSystem.h"
#include "../../game/logic/SaveManager.h"
#include "../../game/controller/UndoManager.h"
#include "../../game/logic/binary_tree/BinaryTree.h"
#include "../../game/logic/heap/max_heap.h"

using namespace std;

void testLevelManagerInit() {
    cout << "Running testLevelManagerInit..." << endl;
    LevelManager lm;

    assert(LevelManager::TOTAL_LEVELS == 20);

    // Verify all 20 levels exist and belong to correct acts
    for (int i = 0; i < 20; i++) {
        assert(lm.levels[i].actNumber == (i / 5) + 1);
        assert(lm.levels[i].levelNumber == (i % 5) + 1);
        assert(strlen(lm.levels[i].title) > 0);
        assert(strlen(lm.levels[i].objective) > 0);
        assert(strlen(lm.levels[i].requiredAction) > 0);
        assert(lm.levels[i].optimalSteps > 0);
        assert(lm.levels[i].alarmCapacity >= lm.levels[i].optimalSteps);
    }

    // Act 1 to 4 structure types
    for (int i = 0; i < 5; i++) assert(strcmp(lm.levels[i].structureType, "tree") == 0);
    for (int i = 5; i < 10; i++) assert(strcmp(lm.levels[i].structureType, "bst") == 0);
    for (int i = 10; i < 15; i++) assert(strcmp(lm.levels[i].structureType, "avl") == 0);
    for (int i = 15; i < 20; i++) assert(strcmp(lm.levels[i].structureType, "heap") == 0);

    // Level 1.1 unlocked by default, others locked
    assert(lm.levels[0].isUnlocked == true);
    for (int i = 1; i < 20; i++) {
        assert(lm.levels[i].isUnlocked == false);
    }

    cout << "testLevelManagerInit passed!" << endl;
}

void testJsonLevelLoading() {
    cout << "Running testJsonLevelLoading..." << endl;
    LevelManager lm;
    bool loaded = lm.loadLevelsFromJson("levels/campaign_levels.json");
    assert(loaded == true);

    // Verify parsed data
    assert(strcmp(lm.levels[0].title, "Level 1.1: Neural Traversal") == 0);
    assert(strcmp(lm.levels[0].requiredAction, "inorder") == 0);
    assert(lm.levels[0].optimalSteps == 28);
    assert(lm.levels[0].alarmCapacity == 44);

    assert(strcmp(lm.levels[5].title, "Level 2.1: The Target Pod") == 0);
    assert(lm.levels[5].targetValue == 60);

    assert(strcmp(lm.levels[19].title, "Level 4.5: Priority Mastery") == 0);
    assert(lm.levels[19].targetValue == 99);

    cout << "testJsonLevelLoading passed!" << endl;
}

void testProgressionAndVictory() {
    cout << "Running testProgressionAndVictory..." << endl;
    LevelManager lm;

    // Load Level 1.1
    assert(lm.loadLevel(0) == true);
    assert(lm.currentLevelIndex == 0);
    assert(strcmp(lm.playState, "in_progress") == 0);
    assert(lm.getCurrentAct() == 1);
    assert(lm.getCurrentLevel() == &lm.levels[0]);

    // Wrong action does not trigger victory
    lm.notifyAction("wrong_action", 0);
    assert(strcmp(lm.playState, "in_progress") == 0);
    assert(lm.levels[1].isUnlocked == false);

    // Correct action triggers victory
    lm.notifyAction("inorder", 0);
    assert(strcmp(lm.playState, "victory") == 0);
    assert(lm.levels[0].isCompleted == true);
    assert(lm.levels[0].starsEarned >= 1);
    assert(lm.levels[1].isUnlocked == true); // Level 1.2 is now unlocked!

    // Test Level 2.1 BST search target check
    assert(lm.loadLevel(5) == true);
    assert(lm.getCurrentAct() == 2);
    lm.notifyAction("search", 40); // wrong key
    assert(strcmp(lm.playState, "in_progress") == 0);
    lm.notifyAction("search", 60); // correct key (targetValue == 60)
    assert(strcmp(lm.playState, "victory") == 0);
    assert(lm.levels[6].isUnlocked == true);

    cout << "testProgressionAndVictory passed!" << endl;
}

void testScoreSystem() {
    cout << "Running testScoreSystem..." << endl;
    ScoreSystem ss;
    ss.reset(6, 11);

    assert(ss.optimalSteps == 6);
    assert(ss.alarmCapacity == 11);
    assert(ss.alarmSteps == 0);
    assert(ss.penalties == 0);
    assert(ss.currentScore == 1000);
    assert(ss.hintsUsed == 0);

    // 3 stars condition: steps <= optimal, 0 penalties
    ss.addAlarmStep(4);
    assert(ss.computeStars() == 3);
    assert(ss.computeFinalScore() == 960);
    assert(!ss.isAlarmBreached());

    // 2 stars condition: steps <= 1.25 * optimal (7)
    ss.addAlarmStep(3); // total steps = 7
    assert(ss.computeStars() == 2);
    assert(ss.computeFinalScore() == 930);

    // 1 star condition: within capacity
    ss.addAlarmStep(3); // total steps = 10 <= 11
    assert(ss.computeStars() == 1);

    // Alarm breached condition
    ss.addAlarmStep(2); // total steps = 12 > 11
    assert(ss.isAlarmBreached());
    assert(ss.computeStars() == 0);

    // Hint usage test
    ss.reset(6, 11);
    ss.addAlarmStep(3);
    assert(ss.computeStars() == 3);
    ss.useHint(); // hintsUsed = 1, alarmSteps = 4
    assert(ss.hintsUsed == 1);
    assert(ss.computeStars() == 2); // 3 - 1 hint = 2 stars

    // Penalty test
    ss.reset(6, 11);
    ss.addPenalty(50, 2);
    assert(ss.penalties == 50);
    assert(ss.alarmSteps == 2);
    assert(ss.computeFinalScore() == 1000 - 20 - 50);

    cout << "testScoreSystem passed!" << endl;
}

void testSaveManagerProgress() {
    cout << "Running testSaveManagerProgress..." << endl;
    LevelManager lm;
    lm.levels[0].isCompleted = true;
    lm.levels[0].starsEarned = 3;
    lm.levels[0].bestScore = 950;
    lm.levels[1].isUnlocked = true;
    lm.levels[1].isCompleted = true;
    lm.levels[1].starsEarned = 2;
    lm.levels[1].bestScore = 880;

    const char* testSaveFile = "levels/test_savegame.dat";
    bool saved = SaveManager::saveProgress(testSaveFile, lm.levels, LevelManager::TOTAL_LEVELS);
    assert(saved == true);

    LevelDef loadedLevels[LevelManager::TOTAL_LEVELS];
    bool loaded = SaveManager::loadProgress(testSaveFile, loadedLevels, LevelManager::TOTAL_LEVELS);
    assert(loaded == true);

    assert(loadedLevels[0].isCompleted == true);
    assert(loadedLevels[0].starsEarned == 3);
    assert(loadedLevels[0].bestScore == 950);
    assert(loadedLevels[1].isUnlocked == true);
    assert(loadedLevels[1].isCompleted == true);
    assert(loadedLevels[1].starsEarned == 2);
    assert(loadedLevels[1].bestScore == 880);

    // Remove temp file
    remove(testSaveFile);
    cout << "testSaveManagerProgress passed!" << endl;
}

void testTreeSerialization() {
    cout << "Running testTreeSerialization..." << endl;

    // Create a binary tree:
    //      50
    //     /  \
    //   25    75
    //   /
    //  10
    TreeNode* root = new TreeNode(50);
    root->left = new TreeNode(25);
    root->right = new TreeNode(75);
    root->left->left = new TreeNode(10);

    char buffer[256];
    SaveManager::serializeTree(root, buffer, sizeof(buffer));

    // Preorder with nulls: 50 25 10 # # # 75 # #
    assert(strstr(buffer, "50 25 10 # # # 75 # #") != nullptr);

    TreeNode* reconstructed = SaveManager::deserializeTree(buffer);
    assert(reconstructed != nullptr);
    assert(reconstructed->data == 50);
    assert(reconstructed->left != nullptr && reconstructed->left->data == 25);
    assert(reconstructed->right != nullptr && reconstructed->right->data == 75);
    assert(reconstructed->left->left != nullptr && reconstructed->left->left->data == 10);
    assert(reconstructed->left->right == nullptr);
    assert(reconstructed->right->left == nullptr);
    assert(reconstructed->right->right == nullptr);

    // Clean up
    BinaryTree bt;
    bt.destroyTree(root);
    bt.destroyTree(reconstructed);

    cout << "testTreeSerialization passed!" << endl;
}

void testHeapSerialization() {
    cout << "Running testHeapSerialization..." << endl;
    int arr[] = {90, 75, 80, 50, 60, 40, 70};
    int n = 7;

    char buffer[128];
    SaveManager::serializeHeap(arr, n, buffer, sizeof(buffer));

    assert(strncmp(buffer, "7 90 75 80 50 60 40 70", 22) == 0);

    int loadedArr[32];
    int loadedSize = 0;
    bool ok = SaveManager::deserializeHeap(buffer, loadedArr, loadedSize, 32);
    assert(ok == true);
    assert(loadedSize == 7);
    for (int i = 0; i < 7; i++) {
        assert(loadedArr[i] == arr[i]);
    }

    cout << "testHeapSerialization passed!" << endl;
}

void testEndlessSaveLoad() {
    cout << "Running testEndlessSaveLoad..." << endl;
    EndlessSaveData data;
    data.wave = 8;
    data.score = 2400;
    data.seed = 98765;
    data.keyCount = 5;
    data.keys[0] = 50;
    data.keys[1] = 25;
    data.keys[2] = 75;
    data.keys[3] = 12;
    data.keys[4] = 88;

    const char* testEndlessFile = "levels/test_endless.dat";
    assert(SaveManager::saveEndless(testEndlessFile, data) == true);

    EndlessSaveData loaded;
    assert(SaveManager::loadEndless(testEndlessFile, loaded) == true);

    assert(loaded.wave == 8);
    assert(loaded.score == 2400);
    assert(loaded.seed == 98765);
    assert(loaded.keyCount == 5);
    for (int i = 0; i < 5; i++) {
        assert(loaded.keys[i] == data.keys[i]);
    }

    remove(testEndlessFile);
    cout << "testEndlessSaveLoad passed!" << endl;
}

void testUndoManager() {
    cout << "Running testUndoManager..." << endl;
    UndoManager um;

    assert(!um.canUndo());

    int keys1[] = {50, 25, 75};
    um.pushSnapshot("tree", keys1, 3, 2, 980);
    assert(um.canUndo());
    assert(um.count == 1);

    int keys2[] = {50, 25, 75, 10};
    um.pushSnapshot("tree", keys2, 4, 3, 970);
    assert(um.count == 2);

    TreeSnapshot s2;
    assert(um.popSnapshot(s2) == true);
    assert(s2.keyCount == 4);
    assert(s2.alarmSteps == 3);
    assert(s2.score == 970);
    assert(um.count == 1);

    TreeSnapshot s1;
    assert(um.popSnapshot(s1) == true);
    assert(s1.keyCount == 3);
    assert(s1.alarmSteps == 2);
    assert(s1.score == 980);
    assert(!um.canUndo());

    cout << "testUndoManager passed!" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  VAULT HEIST PHASE 4 INTEGRATION TESTS" << endl;
    cout << "========================================" << endl;

    testLevelManagerInit();
    testJsonLevelLoading();
    testProgressionAndVictory();
    testScoreSystem();
    testSaveManagerProgress();
    testTreeSerialization();
    testHeapSerialization();
    testEndlessSaveLoad();
    testUndoManager();

    cout << "========================================" << endl;
    cout << "  ALL PHASE 4 TESTS PASSED SUCCESSFULLY! " << endl;
    cout << "========================================" << endl;
    return 0;
}
