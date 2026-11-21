#include <iostream>
#include <cassert>
#include <cstring>
#include "game/logic/bst/bst.h"
#include "game/scoring/ScoreSystem.h"
#include "game/controller/act1-2/Act2State.h"

void testLevel23() {
    std::cout << "Testing Level 2.3 (Steal and Collapse)..." << std::endl;

    BST bst;
    ScoreSystem score;
    Act2Controller act2;

    act2.initLevel(3, &bst, score);

    // Verify initial tree: 50, 30, 70, 20, 40, 60, 80, 65
    assert(bst.isValidBST());
    assert(bst.search(50) && bst.search(30) && bst.search(70));
    assert(bst.search(20) && bst.search(40) && bst.search(60) && bst.search(80) && bst.search(65));

    // STAGE 1: Delete Leaf Node (20)
    assert(act2.lvl23.stage == 0);
    assert(act2.lvl23.deleteKey == 20);
    assert(act2.lvl23.expectedCase == 0); // Leaf
    assert(strcmp(act2.lvl23.phase, "case_select") == 0);

    char fb[160];
    // Wrong case choice (One Child)
    bool wrong = act2.selectDeletionCase(1, fb, sizeof(fb));
    assert(!wrong);
    assert(strcmp(act2.lvl23.phase, "case_select") == 0);
    assert(bst.search(20)); // Tree untouched

    // Correct case choice (Leaf)
    bool ok1 = act2.selectDeletionCase(0, fb, sizeof(fb));
    assert(ok1);
    assert(strcmp(act2.lvl23.phase, "stage_cleared") == 0);
    bst.deleteNode(20);
    assert(!bst.search(20));
    assert(bst.isValidBST());

    // STAGE 2: Delete Node with One Child (60)
    // Advance stage: must discard modified tree and reconstruct fresh BST from scratch
    act2.advanceLevel23Stage();
    assert(act2.lvl23.stage == 1);
    assert(act2.lvl23.deleteKey == 60);
    assert(act2.lvl23.expectedCase == 1); // One Child
    assert(strcmp(act2.lvl23.phase, "case_select") == 0);
    assert(bst.search(20)); // 20 reconstructed in fresh tree
    assert(bst.search(60));
    assert(bst.search(65));

    // Wrong case choice (Leaf)
    wrong = act2.selectDeletionCase(0, fb, sizeof(fb));
    assert(!wrong);
    assert(strcmp(act2.lvl23.phase, "case_select") == 0);

    // Correct case choice (One Child)
    bool ok2 = act2.selectDeletionCase(1, fb, sizeof(fb));
    assert(ok2);
    assert(strcmp(act2.lvl23.phase, "stage_cleared") == 0);
    bst.deleteNode(60);
    assert(!bst.search(60));
    assert(bst.search(65)); // 65 reconnected
    assert(bst.isValidBST());

    // STAGE 3: Delete Node with Two Children (50)
    // Advance stage: reconstruct fresh BST from scratch
    act2.advanceLevel23Stage();
    assert(act2.lvl23.stage == 2);
    assert(act2.lvl23.deleteKey == 50);
    assert(act2.lvl23.expectedCase == 2); // Two Children
    assert(act2.lvl23.expectedSuccessor == 60); // Smallest in right subtree
    assert(strcmp(act2.lvl23.phase, "case_select") == 0);
    assert(bst.search(60)); // Fresh tree has 60

    // Wrong case choice
    wrong = act2.selectDeletionCase(0, fb, sizeof(fb));
    assert(!wrong);

    // Correct case choice (Two Children) -> moves to successor_select
    bool ok3 = act2.selectDeletionCase(2, fb, sizeof(fb));
    assert(ok3);
    assert(strcmp(act2.lvl23.phase, "successor_select") == 0);

    // Wrong successor selection
    wrong = act2.handleLevel23SuccessorClick(70, score, fb, sizeof(fb));
    assert(!wrong);
    assert(!act2.lvl23.isLevelComplete);

    // Correct successor selection (60)
    bool okSucc = act2.handleLevel23SuccessorClick(60, score, fb, sizeof(fb));
    assert(okSucc);
    assert(act2.lvl23.isLevelComplete);
    assert(strcmp(act2.lvl23.phase, "complete") == 0);

    // Model delete 50: value swapped with 60, successor deleted
    bst.deleteNode(50);
    assert(!bst.search(50));
    assert(bst.getRoot()->key == 60); // Root is now 60
    assert(bst.search(65));
    assert(bst.isValidBST());

    std::cout << "Level 2.3 PASS!" << std::endl;
}

void testLevel24() {
    std::cout << "Testing Level 2.4 (Range Raid)..." << std::endl;

    BST bst;
    ScoreSystem score;
    Act2Controller act2;

    act2.initLevel(4, &bst, score);

    // Verify initial tree: 50, 30, 70, 20, 40, 60, 80, 65
    assert(bst.isValidBST());
    assert(act2.lvl24.lo == 30);
    assert(act2.lvl24.hi == 65);
    assert(!act2.lvl24.isExecuted);

    char fb[160];
    bool ok = act2.executeRangeQuery(score, fb, sizeof(fb));
    assert(ok);
    assert(act2.lvl24.isExecuted);
    assert(act2.lvl24.isLevelComplete);

    // Check matching results: 30, 40, 50, 60, 65 (in ascending order)
    assert(act2.lvl24.matchingCount == 5);
    assert(act2.lvl24.matchingKeys[0] == 30);
    assert(act2.lvl24.matchingKeys[1] == 40);
    assert(act2.lvl24.matchingKeys[2] == 50);
    assert(act2.lvl24.matchingKeys[3] == 60);
    assert(act2.lvl24.matchingKeys[4] == 65);

    // Check matching helper
    assert(act2.isKeyMatching(30));
    assert(act2.isKeyMatching(40));
    assert(act2.isKeyMatching(50));
    assert(act2.isKeyMatching(60));
    assert(act2.isKeyMatching(65));
    assert(!act2.isKeyMatching(70));
    assert(!act2.isKeyMatching(20));
    assert(!act2.isKeyMatching(80));

    // Check pruned subtrees: 20 (left of 30) and 80 (right of 70)
    assert(act2.lvl24.prunedCount == 2);
    assert(act2.lvl24.prunedSubtreeRoots[0] == 20);
    assert(act2.lvl24.prunedSubtreeRoots[1] == 80);

    assert(act2.lvl24.prunedNodeCount == 2);
    assert(act2.isKeyPruned(20));
    assert(act2.isKeyPruned(80));
    assert(!act2.isKeyPruned(50));
    assert(!act2.isKeyPruned(70));
    assert(!act2.isKeyPruned(30));

    // Check visited nodes: 50, 30, 40, 70, 60, 65 (6 nodes visited)
    assert(act2.lvl24.visitedCount == 6);
    assert(act2.isKeyVisited(50));
    assert(act2.isKeyVisited(30));
    assert(act2.isKeyVisited(40));
    assert(act2.isKeyVisited(70));
    assert(act2.isKeyVisited(60));
    assert(act2.isKeyVisited(65));
    assert(!act2.isKeyVisited(20)); // Skipped!
    assert(!act2.isKeyVisited(80)); // Skipped!

    // Check alarm and scoring:
    // 6 visited nodes -> 6 alarm steps
    assert(score.alarmSteps == 6);
    // +150 pruning bonus
    assert(score.bonuses == 150);

    std::cout << "Level 2.4 PASS!" << std::endl;
}

int main() {
    testLevel23();
    testLevel24();
    std::cout << "ALL LEVEL 2.3 AND 2.4 TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
