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
    if (node == nullptr) return;
    clearRecursive(node->left);
    clearRecursive(node->right);
    delete node;
}

BSTNode* BST::findMinNode(BSTNode* node) const {
    if (node == nullptr) return nullptr;
    while (node->left != nullptr)
        node = node->left;
    return node;
}

BSTNode* BST::findMaxNode(BSTNode* node) const {
    if (node == nullptr) return nullptr;
    while (node->right != nullptr)
        node = node->right;
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
    }
    return node;
}

void BST::insert(int key) {
    root = insertRecursive(root, key, nullptr);
}

bool BST::search(int key) const {
    BSTNode* curr = root;
    while (curr != nullptr) {
        if (key == curr->key) return true;
        if (key < curr->key) curr = curr->left;
        else curr = curr->right;
    }
    return false;
}

BSTNode* BST::deleteRecursive(BSTNode* node, int key) {
    if (node == nullptr) return nullptr;

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
            BSTNode* successor = findMinNode(node->right);
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

int BST::getMin() const {
    BSTNode* node = findMinNode(root);
    if (node == nullptr) return -1;
    return node->key;
}

int BST::getMax() const {
    BSTNode* node = findMaxNode(root);
    if (node == nullptr) return -1;
    return node->key;
}

int BST::successor(int key) const {
    BSTNode* cur = root;
    BSTNode* result = nullptr;
    while (cur != nullptr) {
        if (key < cur->key) {
            result = cur;
            cur = cur->left;
        } else if (key > cur->key) {
            cur = cur->right;
        } else {
            if (cur->right != nullptr)
                result = findMinNode(cur->right);
            break;
        }
    }
    if (result == nullptr) return -1;
    return result->key;
}

int BST::predecessor(int key) const {
    BSTNode* cur = root;
    BSTNode* result = nullptr;
    while (cur != nullptr) {
        if (key > cur->key) {
            result = cur;
            cur = cur->right;
        } else if (key < cur->key) {
            cur = cur->left;
        } else {
            if (cur->left != nullptr)
                result = findMaxNode(cur->left);
            break;
        }
    }
    if (result == nullptr) return -1;
    return result->key;
}

static void kthHelper(BSTNode* node, int& count, int k, int& result) {
    if (node == nullptr || result != -1) return;
    kthHelper(node->left, count, k, result);
    count++;
    if (count == k) {
        result = node->key;
        return;
    }
    kthHelper(node->right, count, k, result);
}

int BST::kthSmallest(int k) const {
    int count = 0;
    int result = -1;
    kthHelper(root, count, k, result);
    return result;
}

void BST::rangeHelper(BSTNode* node, int lo, int hi) const {
    if (node == nullptr) return;
    if (node->key > lo) rangeHelper(node->left, lo, hi);
    if (node->key >= lo && node->key <= hi) cout << node->key << " ";
    if (node->key < hi) rangeHelper(node->right, lo, hi);
}

void BST::rangeQuery(int lo, int hi) const {
    rangeHelper(root, lo, hi);
    cout << endl;
}

void BST::countHelper(BSTNode* node, int lo, int hi, int& count) const {
    if (node == nullptr) return;
    if (node->key > lo) countHelper(node->left, lo, hi, count);
    if (node->key >= lo && node->key <= hi) count++;
    if (node->key < hi) countHelper(node->right, lo, hi, count);
}

int BST::countInRange(int lo, int hi) const {
    int count = 0;
    countHelper(root, lo, hi, count);
    return count;
}

bool BST::validateHelper(BSTNode* node, int minVal, int maxVal) const {
    if (node == nullptr) return true;
    if (node->key <= minVal || node->key >= maxVal) return false;
    return validateHelper(node->left, minVal, node->key) &&
           validateHelper(node->right, node->key, maxVal);
}

bool BST::isValidBST() const {
    return validateHelper(root, -2147483648, 2147483647);
}

int BST::lcaBST(int a, int b) const {
    BSTNode* cur = root;
    while (cur != nullptr) {
        if (a < cur->key && b < cur->key) cur = cur->left;
        else if (a > cur->key && b > cur->key) cur = cur->right;
        else return cur->key;
    }
    return -1;
}

int BST::heightHelper(BSTNode* node) const {
    if (node == nullptr) return 0;
    int leftH = heightHelper(node->left);
    int rightH = heightHelper(node->right);
    if (leftH > rightH) return leftH + 1;
    return rightH + 1;
}

int BST::getHeight() const {
    return heightHelper(root);
}

bool BST::isDegenerate() const {
    BSTNode* cur = root;
    while (cur != nullptr) {
        if (cur->left != nullptr && cur->right != nullptr) return false;
        if (cur->left != nullptr) cur = cur->left;
        else cur = cur->right;
    }
    return true;
}

void BST::inorderHelper(BSTNode* node) const {
    if (node == nullptr) return;
    inorderHelper(node->left);
    cout << node->key << " ";
    inorderHelper(node->right);
}

void BST::printInorder() const {
    inorderHelper(root);
    cout << endl;
}

struct LevelQNode {
    BSTNode* node;
    LevelQNode* next;
    LevelQNode(BSTNode* n) { node = n; next = nullptr; }
};

void BST::levelOrderHelper(BSTNode* start) const {
    if (start == nullptr) return;
    LevelQNode* front = new LevelQNode(start);
    LevelQNode* back = front;
    while (front != nullptr) {
        BSTNode* cur = front->node;
        cout << cur->key << " ";
        if (cur->left != nullptr) {
            back->next = new LevelQNode(cur->left);
            back = back->next;
        }
        if (cur->right != nullptr) {
            back->next = new LevelQNode(cur->right);
            back = back->next;
        }
        LevelQNode* temp = front;
        front = front->next;
        delete temp;
    }
    cout << endl;
}

void BST::levelOrder() const {
    levelOrderHelper(root);
}

BSTNode* BST::getRoot() const {
    return root;
}

bool BST::isEmpty() const {
    return root == nullptr;
}
