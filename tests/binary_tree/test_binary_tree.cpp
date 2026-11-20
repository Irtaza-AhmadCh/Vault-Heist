#include <iostream>
#include <cassert>
#include <cstring>
#include "../../game/logic/binary_tree/BinaryTree.h"

using namespace std;

void testEmptyAndSingleNode() {
    cout << "Running testEmptyAndSingleNode..." << endl;
    BinaryTree emptyTree;
    assert(emptyTree.root == nullptr);
    assert(emptyTree.height(emptyTree.root) == 0);
    assert(emptyTree.diameter() == 0);
    assert(emptyTree.isSymmetric() == true);

    BinaryTree singleTree;
    singleTree.root = new TreeNode(42);
    assert(singleTree.root->data == 42);
    assert(singleTree.height(singleTree.root) == 1);
    assert(singleTree.diameter() == 0);
    assert(singleTree.isSymmetric() == true);
    assert(singleTree.lca(42, 42) == 42);
    assert(singleTree.pathLength(42, 42) == 0);

    cout << "testEmptyAndSingleNode passed!" << endl;
}

void testSkewedAndFullTrees() {
    cout << "Running testSkewedAndFullTrees..." << endl;

    // Skewed left tree: 3 -> 2 -> 1
    BinaryTree leftSkewed;
    leftSkewed.root = new TreeNode(3);
    leftSkewed.root->left = new TreeNode(2);
    leftSkewed.root->left->left = new TreeNode(1);
    assert(leftSkewed.height(leftSkewed.root) == 3);
    assert(leftSkewed.diameter() == 2);
    assert(leftSkewed.lca(1, 2) == 2);
    assert(leftSkewed.pathLength(1, 3) == 2);

    // Full balanced tree
    //        1
    //      /   \
    //     2     3
    //    / \   / \
    //   4   5 6   7
    BinaryTree fullTree;
    fullTree.root = new TreeNode(1);
    fullTree.root->left = new TreeNode(2);
    fullTree.root->right = new TreeNode(3);
    fullTree.root->left->left = new TreeNode(4);
    fullTree.root->left->right = new TreeNode(5);
    fullTree.root->right->left = new TreeNode(6);
    fullTree.root->right->right = new TreeNode(7);

    assert(fullTree.height(fullTree.root) == 3);
    assert(fullTree.diameter() == 4); // path: 4-2-1-3-7
    assert(fullTree.lca(4, 5) == 2);
    assert(fullTree.lca(4, 6) == 1);
    assert(fullTree.pathLength(4, 5) == 2);
    assert(fullTree.pathLength(4, 7) == 4);

    cout << "testSkewedAndFullTrees passed!" << endl;
}

void testMirrorAndSymmetry() {
    cout << "Running testMirrorAndSymmetry..." << endl;

    // Symmetric tree:
    //        1
    //      /   \
    //     2     2
    //    / \   / \
    //   3   4 4   3
    BinaryTree symTree;
    symTree.root = new TreeNode(1);
    symTree.root->left = new TreeNode(2);
    symTree.root->right = new TreeNode(2);
    symTree.root->left->left = new TreeNode(3);
    symTree.root->left->right = new TreeNode(4);
    symTree.root->right->left = new TreeNode(4);
    symTree.root->right->right = new TreeNode(3);

    assert(symTree.isSymmetric() == true);

    // Asymmetric tree
    BinaryTree asymTree;
    asymTree.root = new TreeNode(1);
    asymTree.root->left = new TreeNode(2);
    asymTree.root->right = new TreeNode(3);
    assert(asymTree.isSymmetric() == false);

    // Mirror operation
    asymTree.mirror(asymTree.root);
    assert(asymTree.root->left->data == 3);
    assert(asymTree.root->right->data == 2);

    cout << "testMirrorAndSymmetry passed!" << endl;
}

void testReconstruction() {
    cout << "Running testReconstruction..." << endl;

    int pre[] = {1, 2, 4, 5, 3, 6, 7};
    int in[]  = {4, 2, 5, 1, 6, 3, 7};
    int post[] = {4, 5, 2, 6, 7, 3, 1};

    BinaryTree rPreIn;
    rPreIn.reconstructFromPreIn(pre, in, 7);
    assert(rPreIn.root != nullptr);
    assert(rPreIn.root->data == 1);
    assert(rPreIn.root->left->data == 2);
    assert(rPreIn.root->right->data == 3);
    assert(rPreIn.root->left->left->data == 4);
    assert(rPreIn.root->left->right->data == 5);
    assert(rPreIn.root->right->left->data == 6);
    assert(rPreIn.root->right->right->data == 7);

    BinaryTree rPostIn;
    rPostIn.reconstructFromPostIn(post, in, 7);
    assert(rPostIn.root != nullptr);
    assert(rPostIn.root->data == 1);
    assert(rPostIn.root->left->data == 2);
    assert(rPostIn.root->right->data == 3);
    assert(rPostIn.root->left->left->data == 4);
    assert(rPostIn.root->left->right->data == 5);
    assert(rPostIn.root->right->left->data == 6);
    assert(rPostIn.root->right->right->data == 7);

    cout << "testReconstruction passed!" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  BINARY TREE UNIT TEST SUITE" << endl;
    cout << "========================================" << endl;

    testEmptyAndSingleNode();
    testSkewedAndFullTrees();
    testMirrorAndSymmetry();
    testReconstruction();

    cout << "========================================" << endl;
    cout << "  ALL BINARY TREE TESTS PASSED! " << endl;
    cout << "========================================" << endl;
    return 0;
}
