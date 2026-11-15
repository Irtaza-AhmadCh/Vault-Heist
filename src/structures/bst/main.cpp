#include "BST.h"
#include <iostream>
using namespace std;

int main() {
    BST bst;

    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);
    bst.insert(50);

    cout << "Inorder (sorted): ";
    bst.printInorder();

    cout << "Level Order: ";
    bst.levelOrder();

    cout << "Search 40: " << (bst.search(40) ? "Found" : "Not found") << endl;
    cout << "Search 99: " << (bst.search(99) ? "Found" : "Not found") << endl;

    cout << "Min: " << bst.getMin() << endl;
    cout << "Max: " << bst.getMax() << endl;

    cout << "Successor of 30: "   << bst.successor(30)   << endl;
    cout << "Predecessor of 60: " << bst.predecessor(60) << endl;
    cout << "Successor of 80: "   << bst.successor(80)   << endl;

    cout << "Range [25, 65]: ";
    bst.rangeQuery(25, 65);
    cout << "Count in [25, 65]: " << bst.countInRange(25, 65) << endl;

    cout << "LCA(20,40): " << bst.lcaBST(20, 40) << endl;
    cout << "LCA(60,80): " << bst.lcaBST(60, 80) << endl;

    cout << "Is valid BST: "  << (bst.isValidBST()   ? "Yes" : "No") << endl;
    cout << "Height: "        << bst.getHeight()                      << endl;
    cout << "Is degenerate: " << (bst.isDegenerate() ? "Yes" : "No") << endl;

    bst.deleteNode(20);
    cout << "After deleting 20 (leaf): ";
    bst.printInorder();

    bst.deleteNode(30);
    cout << "After deleting 30 (one child): ";
    bst.printInorder();

    bst.deleteNode(50);
    cout << "After deleting 50 (two children): ";
    bst.printInorder();

    cout << "Still valid BST: " << (bst.isValidBST() ? "Yes" : "No") << endl;

    return 0;
}

