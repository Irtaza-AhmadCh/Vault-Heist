#include <iostream>
#include <cassert>
#include <cstring>

#include "../../src/structures/binary_tree/BinaryTree.h"
#include "../../src/game/scoring/ScoreSystem.h"
#include "../../src/game/levels/act1-2/Act1State.h"

using namespace std;

void testLevel11Patrol() {
    cout << "Running testLevel11Patrol..." << endl;
    BinaryTree tree;
    ScoreSystem score;
    Act1Controller act1;
    char feedback[128];

    act1.initLevel(1, &tree, score);
    assert(act1.lvl11.currentStage == 0);
    assert(score.alarmSteps == 0);

    // Wrong click on stage 0 (Preorder expected 50, clicked 25)
    bool ok = act1.handleLevel11Click(25, score, feedback, sizeof(feedback));
    assert(!ok);
    assert(score.alarmSteps == 1);
    assert(act1.lvl11.stages[0].currentIndex == 0);

    // Correct clicks for stage 0: 50, 25, 12, 37, 75, 62, 88
    int s0[7] = {50, 25, 12, 37, 75, 62, 88};
    for (int i = 0; i < 7; i++) {
        ok = act1.handleLevel11Click(s0[i], score, feedback, sizeof(feedback));
        assert(ok);
    }
    // Advanced to stage 1 (Postorder)
    assert(act1.lvl11.currentStage == 1);
    assert(score.alarmSteps == 8); // 1 penalty + 7 correct visits

    // Correct clicks for stage 1: 10, 30, 20, 50, 70, 60, 40
    int s1[7] = {10, 30, 20, 50, 70, 60, 40};
    for (int i = 0; i < 7; i++) {
        ok = act1.handleLevel11Click(s1[i], score, feedback, sizeof(feedback));
        assert(ok);
    }
    assert(act1.lvl11.currentStage == 2);

    // Correct clicks for stage 2: 11, 22, 33, 45, 55, 68, 77
    int s2[7] = {11, 22, 33, 45, 55, 68, 77};
    for (int i = 0; i < 7; i++) {
        ok = act1.handleLevel11Click(s2[i], score, feedback, sizeof(feedback));
        assert(ok);
    }
    assert(act1.lvl11.currentStage == 3);

    // Correct clicks for stage 3: 52, 26, 78, 13, 39, 65, 91
    int s3[7] = {52, 26, 78, 13, 39, 65, 91};
    for (int i = 0; i < 7; i++) {
        ok = act1.handleLevel11Click(s3[i], score, feedback, sizeof(feedback));
        assert(ok);
    }
    assert(act1.lvl11.isLevelComplete == true);
    assert(score.alarmSteps == 29); // 1 penalty + 28 visits
    assert(!score.isAlarmBreached());
    assert(score.computeStars() == 2); // 1 penalty within 1.25 * optimal (35) -> 2 stars

    cout << "testLevel11Patrol passed!" << endl;
}

void testLevel12Reconstruction() {
    cout << "Running testLevel12Reconstruction..." << endl;
    BinaryTree tree;
    ScoreSystem score;
    Act1Controller act1;
    char feedback[128];

    act1.initLevel(2, &tree, score);
    assert(act1.lvl12.currentStage == 0);

    // Select token 50 (index 3 in inSeq: {12, 25, 37, 50, 62, 75, 88})
    int token50Idx = -1;
    for (int i = 0; i < 7; i++) {
        if (act1.lvl12.tokens[i].value == 50) token50Idx = i;
    }
    assert(token50Idx != -1);
    assert(act1.selectToken(token50Idx));

    // Try placing in unavailable slot 1 (left of root, but root not placed yet)
    bool ok = act1.placeTokenInSlot(1, score, feedback, sizeof(feedback));
    assert(!ok);

    // Place in slot 0 (root)
    assert(act1.selectToken(token50Idx));
    ok = act1.placeTokenInSlot(0, score, feedback, sizeof(feedback));
    assert(ok);
    assert(tree.root != nullptr && tree.root->data == 50);
    assert(act1.lvl12.slots[1].isAvailable);
    assert(act1.lvl12.slots[2].isAvailable);

    // Place token 25 in slot 1
    int token25Idx = -1;
    for (int i = 0; i < 7; i++) if (act1.lvl12.tokens[i].value == 25) token25Idx = i;
    assert(act1.selectToken(token25Idx));
    assert(act1.placeTokenInSlot(1, score, feedback, sizeof(feedback)));

    // Place remaining tokens for stage 0: 75, 12, 37, 62, 88
    int remainingSlots[5] = {2, 3, 4, 5, 6};
    int remainingVals[5]  = {75, 12, 37, 62, 88};
    for (int k = 0; k < 5; k++) {
        int idx = -1;
        for (int i = 0; i < 7; i++) if (!act1.lvl12.tokens[i].placed && act1.lvl12.tokens[i].value == remainingVals[k]) idx = i;
        assert(act1.selectToken(idx));
        assert(act1.placeTokenInSlot(remainingSlots[k], score, feedback, sizeof(feedback)));
    }

    // Now should have advanced to stage 1 (Post+In)
    assert(act1.lvl12.currentStage == 1);

    // Complete stage 1: target values for slots 0..6: 40, 20, 60, 10, 30, 50, 70
    int st1Slots[7] = {0, 1, 2, 3, 4, 5, 6};
    int st1Vals[7]  = {40, 20, 60, 10, 30, 50, 70};
    for (int k = 0; k < 7; k++) {
        int idx = -1;
        for (int i = 0; i < 7; i++) if (!act1.lvl12.tokens[i].placed && act1.lvl12.tokens[i].value == st1Vals[k]) idx = i;
        assert(act1.selectToken(idx));
        assert(act1.placeTokenInSlot(st1Slots[k], score, feedback, sizeof(feedback)));
    }

    assert(act1.lvl12.isLevelComplete == true);
    assert(score.alarmSteps == 14);
    assert(score.penaltyCount == 0); // No penalties incurred
    assert(score.computeStars() == 3); // 14 steps, 0 penalties -> 3 stars!

    cout << "testLevel12Reconstruction passed!" << endl;
}

void testLevel13Metrics() {
    cout << "Running testLevel13Metrics..." << endl;
    BinaryTree tree;
    ScoreSystem score;
    Act1Controller act1;
    char feedback[128];

    act1.initLevel(3, &tree, score);

    // Postorder order: 12, 37, 25, 62, 88, 75, 50
    // Try clicking 50 first (out of order)
    bool ok = act1.handleLevel13NodeClick(50, score, feedback, sizeof(feedback));
    assert(!ok);
    assert(score.alarmSteps == 1); // penalty

    // Click 12 (leaf, correct)
    ok = act1.handleLevel13NodeClick(12, score, feedback, sizeof(feedback));
    assert(ok);
    assert(strcmp(act1.lvl13.subState, "enter_height") == 0);

    // Submit wrong height 2 for leaf
    ok = act1.submitLevel13Input(2, score, feedback, sizeof(feedback));
    assert(!ok);
    assert(score.alarmSteps == 2);

    // Submit correct height 0 for leaf 12
    ok = act1.submitLevel13Input(0, score, feedback, sizeof(feedback));
    assert(ok);

    // Process nodes 37(0), 25(1), 62(0), 88(0), 75(1), 50(2)
    int poVals[6] = {37, 25, 62, 88, 75, 50};
    int poH[6]    = {0,  1,  0,  0,  1,  2};
    for (int i = 0; i < 6; i++) {
        assert(act1.handleLevel13NodeClick(poVals[i], score, feedback, sizeof(feedback)));
        assert(act1.submitLevel13Input(poH[i], score, feedback, sizeof(feedback)));
    }

    // Now state is enter_diameter
    assert(strcmp(act1.lvl13.subState, "enter_diameter") == 0);

    // Wrong diameter 6
    assert(!act1.submitLevel13Input(6, score, feedback, sizeof(feedback)));

    // Correct diameter 4
    assert(act1.submitLevel13Input(4, score, feedback, sizeof(feedback)));
    assert(act1.lvl13.isLevelComplete == true);
    assert(score.computeStars() == 1); // 3 penalties incurred -> 1 star

    cout << "testLevel13Metrics passed!" << endl;
}

void testLevel14LCAAndPath() {
    cout << "Running testLevel14LCAAndPath..." << endl;
    BinaryTree tree;
    ScoreSystem score;
    Act1Controller act1;
    char feedback[128];

    act1.initLevel(4, &tree, score);
    assert(strcmp(act1.lvl14.phase, "select_lca") == 0);

    // Wrong LCA (25)
    assert(!act1.handleLevel14Click(25, score, feedback, sizeof(feedback)));

    // Correct LCA (50)
    assert(act1.handleLevel14Click(50, score, feedback, sizeof(feedback)));
    assert(strcmp(act1.lvl14.phase, "trace_path") == 0);

    // Tracing path: expected next is 25, 50, 75, 62
    assert(!act1.handleLevel14Click(75, score, feedback, sizeof(feedback))); // wrong next
    assert(act1.handleLevel14Click(25, score, feedback, sizeof(feedback)));  // correct
    assert(act1.handleLevel14Click(50, score, feedback, sizeof(feedback)));  // correct
    assert(act1.handleLevel14Click(75, score, feedback, sizeof(feedback)));  // correct
    assert(act1.handleLevel14Click(62, score, feedback, sizeof(feedback)));  // destination reached

    assert(act1.lvl14.isLevelComplete == true);
    assert(score.computeStars() == 1); // 2 penalties incurred -> 1 star
    cout << "testLevel14LCAAndPath passed!" << endl;
}

void testLevel15Mirror() {
    cout << "Running testLevel15Mirror..." << endl;
    BinaryTree tree;
    ScoreSystem score;
    Act1Controller act1;
    char feedback[128];

    act1.initLevel(5, &tree, score);

    // Bottom-up rule violation: try swapping root 50 first
    bool ok = act1.handleLevel15Swap(50, score, feedback, sizeof(feedback));
    assert(!ok);
    assert(score.alarmSteps == 1);

    // Try swapping 25 (its children 12, 37 not swapped yet)
    ok = act1.handleLevel15Swap(25, score, feedback, sizeof(feedback));
    assert(!ok);

    // Swap leaves first: 12, 37, 62, 88
    assert(act1.handleLevel15Swap(12, score, feedback, sizeof(feedback)));
    assert(act1.handleLevel15Swap(37, score, feedback, sizeof(feedback)));
    assert(act1.handleLevel15Swap(62, score, feedback, sizeof(feedback)));
    assert(act1.handleLevel15Swap(88, score, feedback, sizeof(feedback)));

    // Now 25 and 75 are eligible
    assert(act1.handleLevel15Swap(25, score, feedback, sizeof(feedback)));
    assert(act1.handleLevel15Swap(75, score, feedback, sizeof(feedback)));

    // Now root 50 is eligible
    assert(act1.handleLevel15Swap(50, score, feedback, sizeof(feedback)));

    assert(act1.lvl15.isLevelComplete == true);
    assert(tree.isSameTree(tree.root, act1.targetTree.root));

    // Also test Auto Mirror on a fresh level 5
    Act1Controller autoAct1;
    BinaryTree autoTree;
    ScoreSystem autoScore;
    autoAct1.initLevel(5, &autoTree, autoScore);
    assert(autoAct1.handleAutoMirror(autoScore, feedback, sizeof(feedback)));
    assert(autoAct1.lvl15.isLevelComplete == true);
    assert(autoTree.isSameTree(autoTree.root, autoAct1.targetTree.root));
    assert(autoScore.alarmSteps == 7);
    assert(autoScore.penaltyCount == 0);
    assert(autoScore.computeStars() == 3); // 7 steps, 0 penalties -> 3 stars!

    cout << "testLevel15Mirror passed!" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  ACT 1 GAMEPLAY LOGIC UNIT TEST SUITE " << endl;
    cout << "========================================" << endl;

    testLevel11Patrol();
    testLevel12Reconstruction();
    testLevel13Metrics();
    testLevel14LCAAndPath();
    testLevel15Mirror();

    cout << "========================================" << endl;
    cout << "  ALL ACT 1 TESTS PASSED SUCCESSFULLY! " << endl;
    cout << "========================================" << endl;
    return 0;
}
