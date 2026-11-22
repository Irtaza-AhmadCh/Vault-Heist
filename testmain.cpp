#include <iostream>
#include <fstream>
#include <cstdlib>
#include "game/logic/binary_tree/BinaryTree.h"
#include "game/logic/bst/bst.h"
#include "game/logic/avl/avl_tree.h"
#include "game/logic/heap/max_heap.h"

static int passed = 0;
static int failed = 0;
static std::ofstream report;

static void check(bool ok, const char* name) {
    if (ok) {
        ++passed;
        std::cout << "[PASS] " << name << std::endl;
        report << "[PASS] " << name << std::endl;
    } else {
        ++failed;
        std::cout << "[FAIL] " << name << std::endl;
        report << "[FAIL] " << name << std::endl;
    }
}

static bool same(const int* actual, int count, const int* expected, int expectedCount) {
    if (count != expectedCount) return false;
    for (int i = 0; i < count; ++i) if (actual[i] != expected[i]) return false;
    return true;
}

static void binaryTreeTests() {
    BinaryTree tree;
    tree.root = new TreeNode(4);
    tree.root->left = new TreeNode(2);
    tree.root->right = new TreeNode(6);
    tree.root->left->left = new TreeNode(1);
    tree.root->left->right = new TreeNode(3);
    tree.root->right->left = new TreeNode(5);
    tree.root->right->right = new TreeNode(7);
    int values[16], count = 0;
    const int pre[] = {4, 2, 1, 3, 6, 5, 7};
    const int in[] = {1, 2, 3, 4, 5, 6, 7};
    const int post[] = {1, 3, 2, 5, 7, 6, 4};
    const int level[] = {4, 2, 6, 1, 3, 5, 7};
    tree.getPreorder(values, count); check(same(values, count, pre, 7), "BinaryTree preorder");
    tree.getInorder(values, count); check(same(values, count, in, 7), "BinaryTree inorder");
    tree.getPostorder(values, count); check(same(values, count, post, 7), "BinaryTree postorder");
    tree.getLevelOrder(values, count); check(same(values, count, level, 7), "BinaryTree level order");
    check(tree.height(tree.root) == 3 && tree.heightEdge(tree.root) == 2, "BinaryTree documented node height and edge height");
    check(tree.diameter() == 4 && tree.lca(1, 3) == 2 && tree.pathLength(1, 7) == 4, "BinaryTree diameter, LCA and path length");
    BinaryTree rebuilt;
    rebuilt.reconstructFromPreIn(const_cast<int*>(pre), const_cast<int*>(in), 7);
    check(tree.isSameTree(tree.root, rebuilt.root), "BinaryTree preorder/inorder reconstruction");
    rebuilt.reconstructFromPostIn(const_cast<int*>(post), const_cast<int*>(in), 7);
    check(tree.isSameTree(tree.root, rebuilt.root), "BinaryTree postorder/inorder reconstruction");
}

static void bstTests() {
    BST tree;
    const int keys[] = {50, 30, 70, 20, 40, 60, 80, 35};
    for (int i = 0; i < 8; ++i) tree.insert(keys[i]);
    tree.insert(40);
    check(tree.isValidBST() && tree.search(35) && !tree.search(999), "BST insert, duplicate rejection, search and validator");
    check(tree.getMin() == 20 && tree.getMax() == 80 && tree.successor(40) == 50 && tree.predecessor(40) == 35, "BST extrema and predecessor/successor");
    check(tree.countInRange(30, 60) == 5 && tree.lcaBST(35, 40) == 40, "BST range count and LCA");
    tree.deleteNode(20); check(tree.isValidBST() && !tree.search(20), "BST leaf deletion");
    tree.deleteNode(30); check(tree.isValidBST() && !tree.search(30), "BST one-child deletion");
    tree.deleteNode(50); check(tree.isValidBST() && !tree.search(50), "BST two-child root deletion");
    tree.deleteNode(999); check(tree.isValidBST(), "BST missing-key deletion");
}

static bool avlMetadata(AVLNode* n, int lo, int hi, int& h, int& sz) {
    if (!n) { h = 0; sz = 0; return true; }
    if (n->key <= lo || n->key >= hi) return false;
    int lh, rh, ls, rs;
    if ((n->left && n->left->parent != n) || (n->right && n->right->parent != n)) return false;
    bool ok = avlMetadata(n->left, lo, n->key, lh, ls) && avlMetadata(n->right, n->key, hi, rh, rs);
    h = 1 + (lh > rh ? lh : rh); sz = 1 + ls + rs;
    return ok && n->height == h && n->size == sz && (lh - rh <= 1) && (rh - lh <= 1);
}

static void avlTests() {
    const int samples[4][3] = {{30,20,10},{10,20,30},{30,10,20},{10,30,20}};
    for (int r = 0; r < 4; ++r) {
        AVLTree tree;
        for (int i = 0; i < 3; ++i) tree.insert(samples[r][i]);
        check(tree.getRoot()->key == 20 && tree.isBalanced(), "AVL LL/RR/LR/RL rotation and balance");
        int h, sz; check(avlMetadata(tree.getRoot(), -2147483647, 2147483647, h, sz) && h == 2 && sz == 3, "AVL height, size and parent metadata");
    }
    AVLTree tree;
    const int values[] = {40, 20, 60, 10, 30, 50, 70, 25, 35};
    for (int i = 0; i < 9; ++i) tree.insert(values[i]);
    bool stats = true;
    for (int k = 1; k <= 9; ++k) if (tree.kth(k) != (k == 1 ? 10 : k == 2 ? 20 : k == 3 ? 25 : k == 4 ? 30 : k == 5 ? 35 : k == 6 ? 40 : k == 7 ? 50 : k == 8 ? 60 : 70)) stats = false;
    check(stats && tree.rank(35) == 5 && tree.countInRange(25, 50) == 5, "AVL kth, rank and countInRange");
    tree.deleteNode(40); tree.deleteNode(10); tree.deleteNode(999);
    int h, sz; check(tree.isBalanced() && avlMetadata(tree.getRoot(), -2147483647, 2147483647, h, sz) && sz == 7, "AVL deletion and metadata validation");
}

static void heapTests() {
    MaxHeap empty;
    check(empty.isEmpty() && empty.isMaxHeap(), "MaxHeap empty state");
    const int input[] = {4, 10, 3, 5, 1, 10, -2};
    MaxHeap heap(input, 7);
    check(heap.isMaxHeap() && heap.peekMax() == 10, "MaxHeap construction with duplicates and negative key");
    heap.insert(20); check(heap.isMaxHeap() && heap.peekMax() == 20, "MaxHeap insertion");
    int previous = heap.extractMax(); bool ordered = previous == 20;
    while (!heap.isEmpty()) { int next = heap.extractMax(); if (next > previous || !heap.isMaxHeap()) ordered = false; previous = next; }
    check(ordered, "MaxHeap extraction maintains property and descending maxima");
    const int sorted[] = {-1, 0, 4, 4, 7, 9};
    int copy[] = {9, -1, 4, 4, 0, 7};
    MaxHeap::heapSort(copy, 6, nullptr);
    check(same(copy, 6, sorted, 6), "Heap Sort output including duplicates and negatives");
}

int main() {
    report.open("test_results.txt");
    if (!report) { std::cerr << "Cannot write test_results.txt" << std::endl; return 2; }
    std::cout << "Vault Heist data structure tests (headless)" << std::endl;
    report << "Vault Heist data structure tests (headless)" << std::endl;
    binaryTreeTests(); bstTests(); avlTests(); heapTests();
    std::cout << "Passed: " << passed << " Failed: " << failed << std::endl;
    report << "Passed: " << passed << " Failed: " << failed << std::endl;
    report.close();
    return failed == 0 ? 0 : 1;
}
