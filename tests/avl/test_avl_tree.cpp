#include <iostream>
#include <cstring>
#include "../../game/logic/event.h"
#include "../../game/logic/avl/avl_tree.h"

using namespace std;

int testsPassed = 0;
int testsTotal = 0;

void assertCondition(bool condition, const char* testName) {
    testsTotal++;
    if (condition) {
        testsPassed++;
        cout << "[PASS] " << testName << endl;
    } else {
        cout << "[FAIL] " << testName << endl;
    }
}

void testAVLRotations() {
    cout << "--- Testing AVL Rotations (LL, RR, LR, RL) ---" << endl;

    // Test RR rotation (inserting 10, 20, 30 -> right heavy)
    AVLTree treeRR;
    treeRR.insert(10);
    treeRR.insert(20);
    treeRR.insert(30);
    assertCondition(treeRR.isBalanced(), "Tree is balanced after RR rotation");
    assertCondition(treeRR.getRoot()->key == 20, "Root is 20 after RR rotation");
    assertCondition(treeRR.getRoot()->left->key == 10, "Left child is 10");
    assertCondition(treeRR.getRoot()->right->key == 30, "Right child is 30");

    // Test LL rotation (inserting 30, 20, 10 -> left heavy)
    AVLTree treeLL;
    treeLL.insert(30);
    treeLL.insert(20);
    treeLL.insert(10);
    assertCondition(treeLL.isBalanced(), "Tree is balanced after LL rotation");
    assertCondition(treeLL.getRoot()->key == 20, "Root is 20 after LL rotation");
    assertCondition(treeLL.getRoot()->left->key == 10, "Left child is 10");
    assertCondition(treeLL.getRoot()->right->key == 30, "Right child is 30");

    // Test LR rotation (inserting 30, 10, 20)
    AVLTree treeLR;
    treeLR.insert(30);
    treeLR.insert(10);
    treeLR.insert(20);
    assertCondition(treeLR.isBalanced(), "Tree is balanced after LR rotation");
    assertCondition(treeLR.getRoot()->key == 20, "Root is 20 after LR rotation");
    assertCondition(treeLR.getRoot()->left->key == 10, "Left child is 10");
    assertCondition(treeLR.getRoot()->right->key == 30, "Right child is 30");

    // Test RL rotation (inserting 10, 30, 20)
    AVLTree treeRL;
    treeRL.insert(10);
    treeRL.insert(30);
    treeRL.insert(20);
    assertCondition(treeRL.isBalanced(), "Tree is balanced after RL rotation");
    assertCondition(treeRL.getRoot()->key == 20, "Root is 20 after RL rotation");
    assertCondition(treeRL.getRoot()->left->key == 10, "Left child is 10");
    assertCondition(treeRL.getRoot()->right->key == 30, "Right child is 30");
}

void testAVLInsertionAndDeletion() {
    cout << "--- Testing AVL Insertion and Deletion ---" << endl;
    AVLTree tree;
    int keys[] = {50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 55};
    int n = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < n; i++) {
        tree.insert(keys[i]);
        assertCondition(tree.isBalanced(), "Tree remains balanced after inserting key");
    }

    assertCondition(tree.getRootSize() == 11, "Subtree size of root is 11");

    // Duplicate key rejection
    tree.insert(50);
    assertCondition(tree.getRootSize() == 11, "Duplicate key 50 rejected without size increase");

    // Search tests
    assertCondition(tree.search(27) == true, "Search finds 27");
    assertCondition(tree.search(999) == false, "Search correctly returns false for 999");

    // Deletion: leaf
    tree.deleteNode(5);
    assertCondition(tree.isBalanced(), "Tree balanced after deleting leaf (5)");
    assertCondition(!tree.search(5), "5 is deleted");

    // Deletion: one child
    tree.deleteNode(10);
    assertCondition(tree.isBalanced(), "Tree balanced after deleting node with one child (10)");
    assertCondition(!tree.search(10), "10 is deleted");

    // Deletion: two children
    tree.deleteNode(50);
    assertCondition(tree.isBalanced(), "Tree balanced after deleting root with two children (50)");
    assertCondition(!tree.search(50), "50 is deleted");
}

void testOrderStatistics() {
    cout << "--- Testing Order Statistics (kth, rank, countInRange) ---" << endl;
    AVLTree tree;
    int sortedKeys[] = {10, 20, 30, 40, 50, 60, 70};
    int n = sizeof(sortedKeys) / sizeof(sortedKeys[0]);

    for (int i = 0; i < n; i++) {
        tree.insert(sortedKeys[i]);
    }

    assertCondition(tree.kth(1) == 10, "kth(1) is 10 (smallest)");
    assertCondition(tree.kth(4) == 40, "kth(4) is 40 (median)");
    assertCondition(tree.kth(7) == 70, "kth(7) is 70 (largest)");
    assertCondition(tree.kth(0) == -1, "kth(0) is invalid (-1)");
    assertCondition(tree.kth(8) == -1, "kth(8) is invalid (-1)");

    assertCondition(tree.rank(10) == 1, "rank(10) is 1");
    assertCondition(tree.rank(40) == 4, "rank(40) is 4");
    assertCondition(tree.rank(70) == 7, "rank(70) is 7");

    assertCondition(tree.countInRange(20, 50) == 4, "countInRange(20, 50) is 4");
    assertCondition(tree.countInRange(10, 70) == 7, "countInRange(10, 70) is 7");
}

void testAVLEventLogging() {
    cout << "--- Testing AVL Event Logging ---" << endl;
    EventLog log;
    AVLTree tree(&log);

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);

    bool hasRotateEvent = false;
    for (int i = 0; i < log.getCount(); i++) {
        if (strcmp(log.getEvent(i).type, EVENT_ROTATE) == 0) {
            hasRotateEvent = true;
            break;
        }
    }
    assertCondition(hasRotateEvent, "Rotate event logged during AVL rebalance");
}

int main() {
    cout << "========================================" << endl;
    cout << "    RUNNING MEMBER B AVL UNIT TESTS     " << endl;
    cout << "========================================" << endl;

    testAVLRotations();
    testAVLInsertionAndDeletion();
    testOrderStatistics();
    testAVLEventLogging();

    cout << "========================================" << endl;
    cout << "Results: " << testsPassed << "/" << testsTotal << " tests passed." << endl;
    cout << "========================================" << endl;

    return (testsPassed == testsTotal) ? 0 : 1;
}

