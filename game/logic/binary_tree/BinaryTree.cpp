#include "BinaryTree.h"
#include <iostream>
using namespace std;

int main() {
    BinaryTree tree;

    tree.root = new TreeNode(1);
    tree.root->left = new TreeNode(2);
    tree.root->right = new TreeNode(3);
    tree.root->left->left = new TreeNode(4);
    tree.root->left->right = new TreeNode(5);
    tree.root->right->left = new TreeNode(6);
    tree.root->right->right = new TreeNode(7);

    cout << "Preorder: ";
    tree.printPreorder();

    cout << "Inorder: ";
    tree.printInorder();

    cout << "Postorder: ";
    tree.printPostorder();

    cout << "Level Order: ";
    tree.levelOrder();

    cout << "Height: " << tree.height(tree.root) << endl;
    cout << "Diameter: " << tree.diameter() << endl;
    cout << "LCA(4,5): " << tree.lca(4, 5) << endl;
    cout << "LCA(4,6): " << tree.lca(4, 6) << endl;
    cout << "Path length(4,6): " << tree.pathLength(4, 6) << endl;
    cout << "Is Symmetric: " << (tree.isSymmetric() ? "Yes" : "No") << endl;

    tree.mirror(tree.root);
    cout << "Inorder after mirror: ";
    tree.printInorder();

    int pre[] = {1, 2, 4, 5, 3, 6, 7};
    int in[]  = {4, 2, 5, 1, 6, 3, 7};
    BinaryTree reconstructed;
    reconstructed.reconstructFromPreIn(pre, in, 7);
    cout << "Reconstructed (Pre+In) Inorder: ";
    reconstructed.printInorder();

    int post[] = {4, 5, 2, 6, 7, 3, 1};
    int in2[]  = {4, 2, 5, 1, 6, 3, 7};
    BinaryTree reconstructed2;
    reconstructed2.reconstructFromPostIn(post, in2, 7);
    cout << "Reconstructed (Post+In) Inorder: ";
    reconstructed2.printInorder();

    return 0;
}

