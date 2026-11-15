#include "BST.h"
#include <iostream>
using namespace std;

int main() {
    BST bst;

    // --- Insert ---
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);
    bst.insert(50); // duplicate — rejected

    cout << "Inorder (sorted): ";
    bst.printInorder();

    cout << "Level Order: ";
    bst.levelOrder();

    // --- Search ---
    cout << "Search 40: " << (bst.search(40) ? "Found" : "Not found") << endl;
    cout << "Search 99: " << (bst.search(99) ? "Found" : "Not found") << endl;

    // --- Min / Max ---
    cout << "Min: " << bst.getMin() << endl;
    cout << "Max: " << bst.getMax() << endl;

    // --- Successor / Predecessor ---
    cout << "Successor of 30: " << bst.successor(30) << endl;
    cout << "Predecessor of 60: " << bst.predecessor(60) << endl;
    cout << "Successor of 80 (none): " << bst.successor(80) << endl;

    // --- Range Query ---
    cout << "Range [25, 65]: ";
    bst.rangeQuery(25, 65);
    cout << "Count in [25, 65]: " << bst.countInRange(25, 65) << endl;

    // --- BST LCA ---
    cout << "LCA(20,40): " << bst.lcaBST(20, 40) << endl;
    cout << "LCA(60,80): " << bst.lcaBST(60, 80) << endl;

    // --- BST Validation ---
    cout << "Is valid BST: " << (bst.isValidBST() ? "Yes" : "No") << endl;

    // --- Height and Degeneration ---
    cout << "Height: " << bst.getHeight() << endl;
    cout << "Is degenerate: " << (bst.isDegenerate() ? "Yes" : "No") << endl;

    // --- Delete: Case 1 (leaf) ---
    bst.deleteNode(20);
    cout << "After deleting 20 (leaf) - Inorder: ";
    bst.printInorder();

    // --- Delete: Case 2 (one child) ---
    bst.deleteNode(30);
    cout << "After deleting 30 (one child) - Inorder: ";
    bst.printInorder();

    // --- Delete: Case 3 (two children) ---
    bst.deleteNode(50);
    cout << "After deleting 50 (two children) - Inorder: ";
    bst.printInorder();

    cout << "Is valid BST after deletions: " << (bst.isValidBST() ? "Yes" : "No") << endl;

    // --- Degenerate BST test ---
    BST skewed;
    skewed.insert(10);
    skewed.insert(20);
    skewed.insert(30);
    skewed.insert(40);
    cout << "Skewed BST is degenerate: " << (skewed.isDegenerate() ? "Yes" : "No") << endl;
    cout << "Skewed BST height: " << skewed.getHeight() << endl;

    return 0;
}

