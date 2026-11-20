#include "avl_tree.h"
#include <iostream>

using namespace std;

AVLNode::AVLNode() {
    id = -1;
    key = 0;
    value = 0;
    height = 1;
    size = 1;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

AVLNode::AVLNode(int k, int val, int nodeId) {
    id = nodeId;
    key = k;
    value = val;
    height = 1;
    size = 1;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

AVLTree::AVLTree(EventLog* log) {
    root = nullptr;
    nextId = 1;
    eventLog = log;
}

AVLTree::~AVLTree() {
    clearRecursive(root);
    root = nullptr;
}

void AVLTree::clearRecursive(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    clearRecursive(node->left);
    clearRecursive(node->right);
    delete node;
}

void AVLTree::setEventLog(EventLog* log) {
    eventLog = log;
}

EventLog* AVLTree::getEventLog() const {
    return eventLog;
}

int AVLTree::maxInt(int a, int b) const {
    return (a > b) ? a : b;
}

int AVLTree::height(AVLNode* node) const {
    return (node != nullptr) ? node->height : 0;
}

int AVLTree::subtreeSize(AVLNode* node) const {
    return (node != nullptr) ? node->size : 0;
}

int AVLTree::balanceFactor(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

void AVLTree::updateHeightAndSize(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    int oldH = node->height;
    int oldS = node->size;

    node->height = 1 + maxInt(height(node->left), height(node->right));
    node->size = 1 + subtreeSize(node->left) + subtreeSize(node->right);

    if (eventLog != nullptr) {
        if (oldH != node->height) {
            eventLog->logUpdateHeight(node->id, oldH, node->height);
        }
        if (oldS != node->size) {
            eventLog->logUpdateSize(node->id, oldS, node->size);
        }
    }
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }

    y->parent = x->parent;
    y->left = x;
    x->parent = y;

    updateHeightAndSize(x);
    updateHeightAndSize(y);

    if (eventLog != nullptr) {
        eventLog->logRotate(y->id, ROTATION_RR);
    }

    return y;
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    y->left = x->right;
    if (x->right != nullptr) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    x->right = y;
    y->parent = x;

    updateHeightAndSize(y);
    updateHeightAndSize(x);

    if (eventLog != nullptr) {
        eventLog->logRotate(x->id, ROTATION_LL);
    }

    return x;
}

AVLNode* AVLTree::rebalance(AVLNode* node) {
    if (node == nullptr) {
        return nullptr;
    }

    updateHeightAndSize(node);
    int bf = balanceFactor(node);

    if (bf > 1) {
        if (eventLog != nullptr) {
            eventLog->logBalanceAlert(node->id, bf);
        }
        if (balanceFactor(node->left) >= 0) {
            return rotateRight(node);
        } else {
            if (eventLog != nullptr) {
                eventLog->logRotate(node->id, ROTATION_LR);
            }
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }

    if (bf < -1) {
        if (eventLog != nullptr) {
            eventLog->logBalanceAlert(node->id, bf);
        }
        if (balanceFactor(node->right) <= 0) {
            return rotateLeft(node);
        } else {
            if (eventLog != nullptr) {
                eventLog->logRotate(node->id, ROTATION_RL);
            }
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }

    return node;
}

AVLNode* AVLTree::insertRecursive(AVLNode* node, int key, int val, AVLNode* parentNode) {
    if (node == nullptr) {
        AVLNode* newNode = new AVLNode(key, val, nextId++);
        newNode->parent = parentNode;
        return newNode;
    }

    if (key < node->key) {
        node->left = insertRecursive(node->left, key, val, node);
    } else if (key > node->key) {
        node->right = insertRecursive(node->right, key, val, node);
    } else {
        cout << "Duplicate key rejected in AVL: " << key << endl;
        return node;
    }

    return rebalance(node);
}

void AVLTree::insert(int key, int value) {
    root = insertRecursive(root, key, value, nullptr);
    if (root != nullptr) {
        root->parent = nullptr;
    }
}

AVLNode* AVLTree::findMin(AVLNode* node) const {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

AVLNode* AVLTree::deleteRecursive(AVLNode* node, int key) {
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
            AVLNode* temp = node->right;
            temp->parent = node->parent;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            AVLNode* temp = node->left;
            temp->parent = node->parent;
            delete node;
            return temp;
        } else {
            AVLNode* successor = findMin(node->right);
            node->key = successor->key;
            node->value = successor->value;
            node->right = deleteRecursive(node->right, successor->key);
        }
    }

    return rebalance(node);
}

void AVLTree::deleteNode(int key) {
    root = deleteRecursive(root, key);
    if (root != nullptr) {
        root->parent = nullptr;
    }
}

bool AVLTree::search(int key) const {
    AVLNode* curr = root;
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

int AVLTree::kthRecursive(AVLNode* node, int k) const {
    if (node == nullptr) {
        return -1;
    }
    int leftS = subtreeSize(node->left);
    if (k == leftS + 1) {
        return node->key;
    }
    if (k <= leftS) {
        return kthRecursive(node->left, k);
    }
    return kthRecursive(node->right, k - (leftS + 1));
}

int AVLTree::kth(int k) const {
    if (root == nullptr || k <= 0 || k > root->size) {
        return -1;
    }
    return kthRecursive(root, k);
}

int AVLTree::rankRecursive(AVLNode* node, int key) const {
    if (node == nullptr) {
        return 0;
    }
    if (key == node->key) {
        return subtreeSize(node->left) + 1;
    }
    if (key < node->key) {
        return rankRecursive(node->left, key);
    }
    return subtreeSize(node->left) + 1 + rankRecursive(node->right, key);
}

int AVLTree::rank(int key) const {
    return rankRecursive(root, key);
}

int AVLTree::countInRange(int lo, int hi) const {
    if (lo > hi) {
        return 0;
    }
    return rank(hi) - rank(lo - 1);
}

bool AVLTree::validateRecursive(AVLNode* node, int minVal, int maxVal) const {
    if (node == nullptr) {
        return true;
    }

    if (minVal != -1 && node->key <= minVal) {
        return false;
    }
    if (maxVal != -1 && node->key >= maxVal) {
        return false;
    }

    int bf = balanceFactor(node);
    if (bf < -1 || bf > 1) {
        return false;
    }

    int expectedHeight = 1 + maxInt(height(node->left), height(node->right));
    if (node->height != expectedHeight) {
        return false;
    }

    int expectedSize = 1 + subtreeSize(node->left) + subtreeSize(node->right);
    if (node->size != expectedSize) {
        return false;
    }

    if (node->left != nullptr && node->left->parent != node) {
        return false;
    }
    if (node->right != nullptr && node->right->parent != node) {
        return false;
    }

    return validateRecursive(node->left, minVal, node->key) &&
           validateRecursive(node->right, node->key, maxVal);
}

bool AVLTree::isBalanced(AVLNode* node) const {
    return validateRecursive(node, -1, -1);
}

bool AVLTree::isBalanced() const {
    return isBalanced(root);
}

AVLNode* AVLTree::getRoot() const {
    return root;
}

int AVLTree::getRootHeight() const {
    return height(root);
}

int AVLTree::getRootSize() const {
    return subtreeSize(root);
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

