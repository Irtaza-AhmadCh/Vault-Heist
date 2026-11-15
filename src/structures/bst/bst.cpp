#include "bst.h"
#include <iostream>

using namespace std;

BSTNode::BSTNode() {
    id = -1;
    key = 0;
    value = 0;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

BSTNode::BSTNode(int k, int val, int nodeId) {
    id = nodeId;
    key = k;
    value = val;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

BST::BST() {
    root = nullptr;
    nextId = 1;
}

BST::~BST() {
    clearRecursive(root);
    root = nullptr;
}

void BST::clearRecursive(BSTNode* node) {
    if (node == nullptr) {
        return;
    }
    clearRecursive(node->left);
    clearRecursive(node->right);
    delete node;
}

BSTNode* BST::findMin(BSTNode* node) const {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

BSTNode* BST::insertRecursive(BSTNode* node, int key, BSTNode* parentNode) {
    if (node == nullptr) {
        BSTNode* newNode = new BSTNode(key, 0, nextId++);
        newNode->parent = parentNode;
        return newNode;
    }

    if (key < node->key) {
        node->left = insertRecursive(node->left, key, node);
    } else if (key > node->key) {
        node->right = insertRecursive(node->right, key, node);
    } else {
        cout << "Duplicate key rejected: " << key << endl;
    }

    return node;
}

void BST::insert(int key) {
    root = insertRecursive(root, key, nullptr);
}

bool BST::search(int key) const {
    BSTNode* curr = root;
    while (curr != nullptr) {
        if (key == curr->key) {
            return true;
        } else if (key < curr->key) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    return false;
}

BSTNode* BST::deleteRecursive(BSTNode* node, int key) {
    if (node == nullptr) {
        return nullptr;
    }

    if (key < node->key) {
        node->left = deleteRecursive(node->left, key);
    } else if (key > node->key) {
        node->right = deleteRecursive(node->right, key);
    } else {
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        } else if (node->left == nullptr) {
            BSTNode* temp = node->right;
            temp->parent = node->parent;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            BSTNode* temp = node->left;
            temp->parent = node->parent;
            delete node;
            return temp;
        } else {
            BSTNode* successor = findMin(node->right);
            node->key = successor->key;
            node->value = successor->value;
            node->right = deleteRecursive(node->right, successor->key);
        }
    }

    return node;
}

void BST::deleteNode(int key) {
    root = deleteRecursive(root, key);
}

BSTNode* BST::getRoot() const {
    return root;
}

bool BST::isEmpty() const {
    return root == nullptr;
}

