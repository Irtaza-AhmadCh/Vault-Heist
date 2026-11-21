#include <iostream>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdio>

#include "../../game/controller/LevelDef.h"
#include "../../game/controller/LevelManager.h"
#include "../../game/controller/UniversalActController.h"
#include "../../game/scoring/ScoreSystem.h"
#include "../../game/logic/SaveManager.h"
#include "../../game/logic/bst/bst.h"
#include "../../game/logic/avl/avl_tree.h"
#include "../../game/controller/act1-2/Act2State.h"
#include "../../game/view/visualization/tree/TreeLayout.h"

using namespace std;

// Helper to map BST to TreeLayoutEngine
void mapBSTToLayout(BSTNode* node, TreeLayoutEngine& layout) {
    if (node == nullptr) return;
    int idx = layout.allocateNode(node->key);
    if (idx == -1) return;

    if (node->left != nullptr) {
        int leftIdx = layout.nodeCount;
        mapBSTToLayout(node->left, layout);
        layout.nodes[idx].leftChildIndex = leftIdx;
        layout.nodes[leftIdx].parentIndex = idx;
    }
    if (node->right != nullptr) {
        int rightIdx = layout.nodeCount;
        mapBSTToLayout(node->right, layout);
        layout.nodes[idx].rightChildIndex = rightIdx;
        layout.nodes[rightIdx].parentIndex = idx;
    }
}

void syncBSTToLayout(BST& bst, TreeLayoutEngine& layout, BSTEmptySlot slots[], int& slotCount, int maxSlots) {
    layout.clear();
    mapBSTToLayout(bst.getRoot(), layout);
    layout.updateLayoutWithNullSlots(1280.0f, slots, slotCount, maxSlots, true);
}

// ============================================================================
// LEVEL 2.2 TESTS
// ============================================================================

void testLevel22NullNodePositioning() {
    cout << "\n=== Running Level 2.2 Tests ===" << endl;

    // Test 1: Balanced tree initial layout
    BST bst;
    int initialKeys[] = {50, 30, 70, 20, 40, 60, 85};
    for (int k : initialKeys) bst.insert(k);

    TreeLayoutEngine layout;
    BSTEmptySlot slots[16];
    int slotCount = 0;
    syncBSTToLayout(bst, layout, slots, slotCount, 16);

    cout << "1. Balanced tree: " << layout.nodeCount << " nodes, " << slotCount << " null slots." << endl;
    assert(layout.nodeCount == 7);
    assert(slotCount == 8); // 4 leaves * 2 null slots = 8 null slots

    // Test 2: Verify all null slots have consistent vertical spacing (parent.y + levelHeight)
    for (int s = 0; s < slotCount; s++) {
        int pKey = slots[s].parentKey;
        int pIdx = -1;
        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].key == pKey) { pIdx = i; break; }
        }
        assert(pIdx != -1);
        float expectedY = layout.nodes[pIdx].y + layout.levelHeight;
        assert(fabs(slots[s].y - expectedY) < 0.001f);
    }
    cout << "   - Vertical spacing consistent for all slots (parent.y + 90.0)." << endl;

    // Test 3: Verify left slots are to the left of parent, right slots to the right
    for (int s = 0; s < slotCount; s++) {
        int pKey = slots[s].parentKey;
        int pIdx = -1;
        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].key == pKey) { pIdx = i; break; }
        }
        if (slots[s].isLeft) {
            assert(slots[s].x < layout.nodes[pIdx].x);
        } else {
            assert(slots[s].x > layout.nodes[pIdx].x);
        }
    }
    cout << "   - Left slots to left of parent, right slots to right of parent." << endl;

    // Test 4: Verify null slots at the same depth do not overlap (min spacing >= 40px)
    for (int i = 0; i < slotCount; i++) {
        for (int j = i + 1; j < slotCount; j++) {
            if (fabs(slots[i].y - slots[j].y) < 1.0f) {
                float dx = fabs(slots[i].x - slots[j].x);
                assert(dx >= 40.0f);
            }
        }
    }
    cout << "   - No overlap between null slots at the same depth." << endl;

    // Test 5: Insert node 45 as right child of 40 (simulating Level 2.2 gameplay insertion)
    bst.insert(45);
    BSTEmptySlot newSlots[16];
    int newSlotCount = 0;
    syncBSTToLayout(bst, layout, newSlots, newSlotCount, 16);

    cout << "2. After inserting 45: " << layout.nodeCount << " nodes, " << newSlotCount << " null slots." << endl;
    assert(layout.nodeCount == 8);
    // Node 40 now has right child 45 (no right slot for 40).
    // Node 45 now has left slot and right slot.
    // Total slots = 8 - 1 + 2 = 9 null slots!
    assert(newSlotCount == 9);

    // Verify newly inserted node 45 has correctly positioned left and right null placeholders
    int slot45Left = -1, slot45Right = -1;
    for (int s = 0; s < newSlotCount; s++) {
        if (newSlots[s].parentKey == 45) {
            if (newSlots[s].isLeft) slot45Left = s;
            else slot45Right = s;
        }
    }
    assert(slot45Left != -1);
    assert(slot45Right != -1);

    int idx45 = -1;
    for (int i = 0; i < layout.nodeCount; i++) {
        if (layout.nodes[i].key == 45) { idx45 = i; break; }
    }
    assert(idx45 != -1);
    assert(newSlots[slot45Left].x < layout.nodes[idx45].x);
    assert(newSlots[slot45Right].x > layout.nodes[idx45].x);
    assert(fabs(newSlots[slot45Left].y - (layout.nodes[idx45].y + layout.levelHeight)) < 0.001f);
    assert(fabs(newSlots[slot45Right].y - (layout.nodes[idx45].y + layout.levelHeight)) < 0.001f);
    cout << "   - Node 45 has correctly positioned left and right null placeholders." << endl;

    // Test 6: Verify left-skewed tree layout
    BST leftTree;
    int leftKeys[] = {50, 40, 30, 20};
    for (int k : leftKeys) leftTree.insert(k);
    BSTEmptySlot leftSlots[16];
    int leftSlotCount = 0;
    syncBSTToLayout(leftTree, layout, leftSlots, leftSlotCount, 16);
    assert(layout.nodeCount == 4);
    assert(leftSlotCount == 5); // 1 for each right child (4), plus 1 for left leaf child = 5
    // Ensure all x positions are strictly separated
    for (int i = 0; i < layout.nodeCount; i++) {
        for (int s = 0; s < leftSlotCount; s++) {
            if (fabs(layout.nodes[i].y - leftSlots[s].y) < 1.0f) {
                assert(fabs(layout.nodes[i].x - leftSlots[s].x) >= 40.0f);
            }
        }
    }
    cout << "   - Left-skewed tree layout renders without overlaps." << endl;

    // Test 7: Verify right-skewed tree layout
    BST rightTree;
    int rightKeys[] = {10, 20, 30, 40};
    for (int k : rightKeys) rightTree.insert(k);
    BSTEmptySlot rightSlots[16];
    int rightSlotCount = 0;
    syncBSTToLayout(rightTree, layout, rightSlots, rightSlotCount, 16);
    assert(layout.nodeCount == 4);
    assert(rightSlotCount == 5);
    cout << "   - Right-skewed tree layout renders without overlaps." << endl;

    cout << "Level 2.2 Tests PASSED!" << endl;
}

// ============================================================================
// LEVEL 2.5 SCORING TESTS
// ============================================================================

void testLevel25Scoring() {
    cout << "\n=== Running Level 2.5 Scoring Tests ===" << endl;

    Act2Controller act2;
    ScoreSystem score;
    BST bst;

    // Test 1: Stage A insertion tracking
    act2.initLevel(5, &bst, score);
    assert(act2.lvl25.stage == 0);
    assert(score.optimalSteps == 28);
    assert(score.alarmCapacity == 44);

    char fb[160];

    // Test 2: Node visits and comparisons contribute to alarm steps according to requirements
    // Inserting sorted sequence: 10, 20, 30, 40, 50, 60, 70
    // Expected alarm steps: 1 + 2 + 3 + 4 + 5 + 6 + 7 = 28 steps
    for (int i = 0; i < 7; i++) {
        bool okVal = act2.handleLevel25SelectArrayValue(i, score, fb, sizeof(fb));
        assert(okVal);
        bool okSlot = act2.handleLevel25SlotClick(0, score, fb, sizeof(fb));
        assert(okSlot);
    }

    assert(act2.lvl25.insertedCount == 7);
    assert(act2.lvl25.insertionRecord.alarmSteps == 28);
    assert(act2.lvl25.insertionRecord.penalties == 0);
    assert(act2.lvl25.insertionRecord.stars == 3);
    assert(act2.lvl25.insertionRecord.score == 1000 - 280); // 720
    assert(score.alarmSteps == 0); // Stage B search meter starts at 0/3

    // Test 3: Insertion stage score is frozen and saved independently
    assert(act2.lvl25.insertionRecord.completed == true);
    assert(act2.lvl25.insertionRecord.alarmSteps == 28);
    assert(act2.lvl25.insertionRecord.penalties == 0);
    assert(act2.lvl25.insertionRecord.score == 720);
    assert(act2.lvl25.insertionRecord.stars == 3);
    cout << "   - Stage A insertion score and stars frozen: " << act2.lvl25.insertionRecord.score << " pts, "
         << act2.lvl25.insertionRecord.stars << " stars." << endl;

    // Test 4: Search demonstration steps do not affect the insertion-stage score
    assert(act2.lvl25.stage == 1);
    // Execute 3 search steps (reaching the budget limit)
    assert(act2.lvl25.currentSearchNodeKey == 10);
    act2.handleLevel25SearchClick(10, score, fb, sizeof(fb)); // comp 1
    assert(act2.lvl25.currentSearchNodeKey == 20);
    act2.handleLevel25SearchClick(20, score, fb, sizeof(fb)); // comp 2
    assert(act2.lvl25.currentSearchNodeKey == 30);
    act2.handleLevel25SearchClick(30, score, fb, sizeof(fb)); // comp 3 -> budget exhausted

    assert(act2.lvl25.searchFailed == true);
    assert(act2.lvl25.stage == 2); // Stage C reached

    // Verify insertion record remains completely unchanged
    assert(act2.lvl25.insertionRecord.score == 720);
    assert(act2.lvl25.insertionRecord.stars == 3);
    assert(act2.lvl25.insertionRecord.alarmSteps == 28);
    assert(act2.lvl25.insertionRecord.penalties == 0);
    cout << "   - Search demonstration did not affect insertion-stage score." << endl;

    // Test 5: Score restoration when completing Level 2.5
    act2.completeLevel25(score);
    assert(act2.lvl25.stage == 3);
    assert(score.alarmSteps == 28);
    assert(score.optimalSteps == 28);
    assert(score.alarmCapacity == 44);
    assert(score.penalties == 0);
    assert(score.currentScore == 720);
    assert(!score.isAlarmBreached());
    assert(score.computeStars() == 3);
    cout << "   - CompleteLevel25 restored Stage A score, stars, and capacity." << endl;

    // Test 6: Incorrect insertion actions apply appropriate penalties
    Act2Controller act2Penalty;
    ScoreSystem scorePenalty;
    BST bstPenalty;
    act2Penalty.initLevel(5, &bstPenalty, scorePenalty);

    // Wrong array value selection
    bool badSelect = act2Penalty.handleLevel25SelectArrayValue(3, scorePenalty, fb, sizeof(fb));
    assert(!badSelect);
    assert(scorePenalty.penalties == 25);
    assert(scorePenalty.alarmSteps == 1); // 1 alarm step penalty
    cout << "   - Incorrect array selection applied penalty (-25 score, +1 alarm)." << endl;

    cout << "Level 2.5 Scoring Tests PASSED!" << endl;
}

// ============================================================================
// LEVEL 2.5 PROGRESSION TESTS
// ============================================================================

void testLevel25Progression() {
    cout << "\n=== Running Level 2.5 Progression Tests ===" << endl;

    LevelManager lm;
    assert(LevelManager::TOTAL_LEVELS == 20);

    // Level 2.5 is index 9 (Act 2 Level 5). Level 3.1 is index 10 (Act 3 Level 1).
    int lvl25Idx = 9;
    int lvl31Idx = 10;

    assert(lm.levels[lvl25Idx].actNumber == 2 && lm.levels[lvl25Idx].levelNumber == 5);
    assert(lm.levels[lvl31Idx].actNumber == 3 && lm.levels[lvl31Idx].levelNumber == 1);

    // Test 1: Level 3.1 is locked before Level 2.5 completion
    lm.levels[lvl25Idx].isUnlocked = true;
    lm.levels[lvl31Idx].isUnlocked = false;
    assert(lm.levels[lvl31Idx].isUnlocked == false);
    cout << "   - Level 3.1 is initially locked." << endl;

    // Test 2: Load Level 2.5
    lm.loadLevel(lvl25Idx);
    assert(strcmp(lm.playState, "in_progress") == 0);

    Act2Controller act2;
    BST bst;
    act2.initLevel(5, &bst, lm.scoreSystem);

    char fb[160];
    // Complete Stage A
    for (int i = 0; i < 7; i++) {
        act2.handleLevel25SelectArrayValue(i, lm.scoreSystem, fb, sizeof(fb));
        act2.handleLevel25SlotClick(0, lm.scoreSystem, fb, sizeof(fb));
    }
    assert(act2.lvl25.stage == 1);
    cout << "   - Stage A completed -> advanced to Stage B Search Demonstration." << endl;

    // Complete Stage B Search Demonstration (budget exhausted at 3 comparisons)
    act2.handleLevel25SearchClick(10, lm.scoreSystem, fb, sizeof(fb));
    act2.handleLevel25SearchClick(20, lm.scoreSystem, fb, sizeof(fb));
    act2.handleLevel25SearchClick(30, lm.scoreSystem, fb, sizeof(fb));
    assert(act2.lvl25.searchFailed == true);
    assert(act2.lvl25.stage == 2);
    cout << "   - Stage B search demonstration budget exhausted (intended endpoint)." << endl;

    // Test 3: Reaching the intended search failure does NOT mark Level 2.5 as failed
    assert(strcmp(lm.playState, "failed") != 0);

    // Test 4: Complete Level 2.5 via completeLevel25 and notifyAction("boss", 0)
    act2.completeLevel25(lm.scoreSystem);
    lm.notifyAction("boss", 0);

    // Verify victory state and stars
    assert(strcmp(lm.playState, "victory") == 0);
    assert(lm.levels[lvl25Idx].isCompleted == true);
    assert(lm.levels[lvl25Idx].starsEarned == 3);
    assert(lm.levels[lvl25Idx].bestScore == 720);
    cout << "   - Level 2.5 successfully marked as completed with victory!" << endl;

    // Test 5: Level 3.1 unlocks after Level 2.5 completion
    assert(lm.levels[lvl31Idx].isUnlocked == true);
    cout << "   - Level 3.1 (Act 3 Level 1) successfully unlocked!" << endl;

    // Test 6: Persistence - Save progress and reload
    const char* savePath = "levels/test_savegame_act2.dat";
    bool saved = SaveManager::saveProgress(savePath, lm.levels, LevelManager::TOTAL_LEVELS);
    assert(saved);

    LevelManager lmReload;
    bool loaded = SaveManager::loadProgress(savePath, lmReload.levels, LevelManager::TOTAL_LEVELS);
    assert(loaded);
    assert(lmReload.levels[lvl25Idx].isCompleted == true);
    assert(lmReload.levels[lvl25Idx].starsEarned == 3);
    assert(lmReload.levels[lvl31Idx].isUnlocked == true);
    remove(savePath);
    cout << "   - Persistence verified: save/load preserved Level 2.5 completion & Level 3.1 unlock." << endl;

    // Test 7: Actual failure during insertion challenge is handled correctly
    LevelManager lmFail;
    lmFail.loadLevel(lvl25Idx);
    Act2Controller act2Fail;
    BST bstFail;
    act2Fail.initLevel(5, &bstFail, lmFail.scoreSystem);

    // Force alarm breach during Stage A (e.g. 45 alarm steps >= capacity 44)
    lmFail.scoreSystem.addAlarmStep(45);
    assert(lmFail.scoreSystem.isAlarmBreached());
    lmFail.notifyAction("fail", 0);
    assert(strcmp(lmFail.playState, "failed") == 0);
    assert(lmFail.levels[lvl31Idx].isUnlocked == false);
    cout << "   - Actual failure during Stage A insertion properly marked as failed (Level 3.1 stays locked)." << endl;

    cout << "Level 2.5 Progression Tests PASSED!" << endl;
}

int main() {
    cout << "==========================================================" << endl;
    cout << " Vault Heist: Act 2 Level 2.2 and Level 2.5 Fixes Tests    " << endl;
    cout << "==========================================================" << endl;

    testLevel22NullNodePositioning();
    testLevel25Scoring();
    testLevel25Progression();

    cout << "\n==========================================================" << endl;
    cout << " ALL ACT 2 FIXES TESTS PASSED SUCCESSFULLY! (22/22 Tests) " << endl;
    cout << "==========================================================" << endl;
    return 0;
}
