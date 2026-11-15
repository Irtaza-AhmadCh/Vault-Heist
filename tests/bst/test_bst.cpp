#include "../../src/structures/bst/BST.h"
#include <iostream>
using namespace std;

int passed = 0;
int failed = 0;

void check(const char* label, bool condition) {
    if (condition) {
        cout << "[PASS] " << label << endl;
        passed++;
    } else {
        cout << "[FAIL] " << label << endl;
        failed++;
    }
}

void testInsertAndSearch() {
    BST bst;
    bst.insert(10);
    bst.insert(5);
    bst.insert(15);
    bst.insert(5);

    check("Insert: search existing key 10",  bst.search(10));
    check("Insert: search existing key 5",   bst.search(5));
    check("Insert: search existing key 15",  bst.search(15));
    check("Insert: search missing key 99",  !bst.search(99));
    check("Insert: duplicate rejected",      bst.isValidBST());
}

void testMinMax() {
    BST bst;
    bst.insert(30);
    bst.insert(10);
    bst.insert(50);
    bst.insert(5);
    bst.insert(20);

    check("MinMax: min is 5",  bst.getMin() == 5);
    check("MinMax: max is 50", bst.getMax() == 50);

    BST empty;
    check("MinMax: empty min returns -1", empty.getMin() == -1);
    check("MinMax: empty max returns -1", empty.getMax() == -1);
}

void testDeleteLeaf() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);

    bst.deleteNode(30);
    check("Delete leaf: 30 removed",      !bst.search(30));
    check("Delete leaf: 50 still exists",  bst.search(50));
    check("Delete leaf: 70 still exists",  bst.search(70));
    check("Delete leaf: still valid BST",  bst.isValidBST());
}

void testDeleteOneChild() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(20);

    bst.deleteNode(30);
    check("Delete one-child: 30 removed",     !bst.search(30));
    check("Delete one-child: 20 still there",  bst.search(20));
    check("Delete one-child: valid BST",       bst.isValidBST());
}

void testDeleteTwoChildren() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);

    bst.deleteNode(30);
    check("Delete two-children: 30 removed",     !bst.search(30));
    check("Delete two-children: 20 still there",  bst.search(20));
    check("Delete two-children: 40 still there",  bst.search(40));
    check("Delete two-children: valid BST",        bst.isValidBST());
}

void testDeleteRoot() {
    BST bst;
    bst.insert(10);
    bst.insert(5);
    bst.insert(15);

    bst.deleteNode(10);
    check("Delete root: root removed",   !bst.search(10));
    check("Delete root: 5 still there",   bst.search(5));
    check("Delete root: 15 still there",  bst.search(15));
    check("Delete root: valid BST",       bst.isValidBST());
}

void testDeleteMissingKey() {
    BST bst;
    bst.insert(10);

    bst.deleteNode(99);
    check("Delete missing key: tree intact", bst.search(10));
    check("Delete missing key: still valid", bst.isValidBST());
}

void testSuccessorPredecessor() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);

    check("Successor: successor of 30 is 40",  bst.successor(30) == 40);
    check("Successor: successor of 50 is 60",  bst.successor(50) == 60);
    check("Successor: successor of 80 is -1",  bst.successor(80) == -1);

    check("Predecessor: predecessor of 70 is 60",  bst.predecessor(70) == 60);
    check("Predecessor: predecessor of 50 is 40",  bst.predecessor(50) == 40);
    check("Predecessor: predecessor of 20 is -1",  bst.predecessor(20) == -1);
}

void testRangeQuery() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);

    check("Range: count [25, 65] is 4",  bst.countInRange(25, 65) == 4);
    check("Range: count [20, 80] is 7",  bst.countInRange(20, 80) == 7);
    check("Range: count [1, 10] is 0",   bst.countInRange(1, 10)  == 0);
    check("Range: count [50, 50] is 1",  bst.countInRange(50, 50) == 1);
}

void testValidation() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);

    check("Validation: valid BST is valid", bst.isValidBST());

    bst.getRoot()->left->key = 999;
    check("Validation: corrupted BST is invalid", !bst.isValidBST());
}

void testLCABST() {
    BST bst;
    bst.insert(20);
    bst.insert(10);
    bst.insert(30);
    bst.insert(5);
    bst.insert(15);

    check("LCA BST: lca(5,15) is 10",  bst.lcaBST(5, 15) == 10);
    check("LCA BST: lca(5,30) is 20",  bst.lcaBST(5, 30) == 20);
    check("LCA BST: lca(10,30) is 20", bst.lcaBST(10, 30) == 20);
}

void testDegenerate() {
    BST sorted;
    sorted.insert(10);
    sorted.insert(20);
    sorted.insert(30);
    sorted.insert(40);

    check("Degenerate: sorted insertions are degenerate", sorted.isDegenerate());
    check("Degenerate: height equals node count",         sorted.getHeight() == 4);

    BST balanced;
    balanced.insert(20);
    balanced.insert(10);
    balanced.insert(30);

    check("Degenerate: balanced BST is not degenerate", !balanced.isDegenerate());
}

void testNodeFields() {
    BST bst;
    bst.insert(42);

    BSTNode* r = bst.getRoot();
    check("Node fields: key is 42",        r->key == 42);
    check("Node fields: id assigned",      r->id >= 1);
    check("Node fields: parent is null",   r->parent == nullptr);
    check("Node fields: left is null",     r->left == nullptr);
    check("Node fields: right is null",    r->right == nullptr);
}

int main() {
    cout << "=== BST Unit Tests ===" << endl << endl;

    testInsertAndSearch();
    testMinMax();
    testDeleteLeaf();
    testDeleteOneChild();
    testDeleteTwoChildren();
    testDeleteRoot();
    testDeleteMissingKey();
    testSuccessorPredecessor();
    testRangeQuery();
    testValidation();
    testLCABST();
    testDegenerate();
    testNodeFields();

    cout << endl;
    cout << "Results: " << passed << " passed, " << failed << " failed." << endl;

    return failed == 0 ? 0 : 1;
}
