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

BST::BST(EventLog* log) {
    root = nullptr;
    nextId = 1;
    eventLog = log;
}

BST::~BST() {
    clearRecursive(root);
    root = nullptr;
}

void BST::setEventLog(EventLog* log) {
    eventLog = log;
}

EventLog* BST::getEventLog() const {
    return eventLog;
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
        if (eventLog != nullptr) {
            int pId = parentNode ? parentNode->id : -1;
            int side = (parentNode && key < parentNode->key) ? 0 : 1;
            eventLog->logInsert(newNode->id, pId, side, key);
        }
        return newNode;
    }

    if (key < node->key) {
        if (eventLog != nullptr) {
            eventLog->logCompare(node->id, node->key, "<");
        }
        node->left = insertRecursive(node->left, key, node);
    } else if (key > node->key) {
        if (eventLog != nullptr) {
            eventLog->logCompare(node->id, node->key, ">");
        }
        node->right = insertRecursive(node->right, key, node);
    }
    return node;
}

void BST::insert(int key) {
    root = insertRecursive(root, key, nullptr);
    if (eventLog != nullptr) {
        eventLog->logDone("BST insertion complete");
    }
}

bool BST::search(int key) const {
    BSTNode* curr = root;
    while (curr != nullptr) {
        if (eventLog != nullptr) {
            const char* res = (key == curr->key) ? "=" : (key < curr->key ? "<" : ">");
            eventLog->logCompare(curr->id, curr->key, res);
        }
        if (key == curr->key) {
            if (eventLog != nullptr) {
                eventLog->logVisit(curr->id, curr->key);
                eventLog->logDone("Search target pod located!");
            }
            return true;
        }
        if (key < curr->key) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    if (eventLog != nullptr) {
        eventLog->logDone("Target key not in BST.");
    }
    return false;
}

BSTNode* BST::deleteRecursive(BSTNode* node, int key) {
    if (node == nullptr) return nullptr;

    if (key < node->key) {
        if (eventLog != nullptr) {
            eventLog->logCompare(node->id, node->key, "<");
        }
        node->left = deleteRecursive(node->left, key);
    } else if (key > node->key) {
        if (eventLog != nullptr) {
            eventLog->logCompare(node->id, node->key, ">");
        }
        node->right = deleteRecursive(node->right, key);
    } else {
        if (node->left == nullptr && node->right == nullptr) {
            if (eventLog != nullptr) {
                int pId = node->parent ? node->parent->id : -1;
                eventLog->logUnlink(node->id, pId, 0, node->key);
            }
            delete node;
            return nullptr;
        } else if (node->left == nullptr) {
            BSTNode* temp = node->right;
            temp->parent = node->parent;
            if (eventLog != nullptr) {
                int pId = node->parent ? node->parent->id : -1;
                eventLog->logUnlink(node->id, pId, 1, node->key);
            }
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            BSTNode* temp = node->left;
            temp->parent = node->parent;
            if (eventLog != nullptr) {
                int pId = node->parent ? node->parent->id : -1;
                eventLog->logUnlink(node->id, pId, 0, node->key);
            }
            delete node;
            return temp;
        } else {
            BSTNode* successor = findMinNode(node->right);
            if (eventLog != nullptr) {
                eventLog->logSwapValue(node->id, successor->id, node->key, successor->key);
            }
            node->key = successor->key;
            node->value = successor->value;
            node->right = deleteRecursive(node->right, successor->key);
        }
    }
    return node;
}

void BST::deleteNode(int key) {
    root = deleteRecursive(root, key);
    if (eventLog != nullptr) {
        eventLog->logDone("BST deletion complete");
    }
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
    if (node->key >= lo && node->key <= hi) {
        if (eventLog != nullptr) {
            eventLog->logHighlightSubtree(node->id, "visited");
        }
        cout << node->key << " ";
    }
    if (node->key < hi) rangeHelper(node->right, lo, hi);
}

void BST::rangeQuery(int lo, int hi) const {
    rangeHelper(root, lo, hi);
    cout << endl;
    if (eventLog != nullptr) {
        eventLog->logDone("BST range query complete");
    }
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
    return validateHelper(root, -2147483647, 2147483647);
}

int BST::lcaBST(int a, int b) const {
    BSTNode* cur = root;
    while (cur != nullptr) {
        if (a < cur->key && b < cur->key) {
            cur = cur->left;
        } else if (a > cur->key && b > cur->key) {
            cur = cur->right;
        } else {
            return cur->key;
        }
    }
    return -1;
}

int BST::heightHelper(BSTNode* node) const {
    if (node == nullptr) return 0;
    int lh = heightHelper(node->left);
    int rh = heightHelper(node->right);
    return 1 + (lh > rh ? lh : rh);
}

int BST::getHeight() const {
    return heightHelper(root);
}

bool BST::isDegenerate() const {
    if (root == nullptr) return false;
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
    if (eventLog != nullptr) {
        eventLog->logVisit(node->id, node->key);
    }
    cout << node->key << " ";
    inorderHelper(node->right);
}

void BST::printInorder() const {
    inorderHelper(root);
    cout << endl;
    if (eventLog != nullptr) {
        eventLog->logDone("Inorder traversal complete");
    }
}

void BST::levelOrderHelper(BSTNode* node) const {
    if (node == nullptr) return;
    BSTNode* queue[128];
    int front = 0, back = 0;
    queue[back++] = node;
    while (front < back) {
        BSTNode* cur = queue[front++];
        if (eventLog != nullptr) {
            eventLog->logVisit(cur->id, cur->key);
        }
        cout << cur->key << " ";
        if (cur->left != nullptr) queue[back++] = cur->left;
        if (cur->right != nullptr) queue[back++] = cur->right;
    }
}

void BST::levelOrder() const {
    levelOrderHelper(root);
    cout << endl;
    if (eventLog != nullptr) {
        eventLog->logDone("Level-order traversal complete");
    }
}

BSTNode* BST::getRoot() const {
    return root;
}

bool BST::isEmpty() const {
    return root == nullptr;
}
