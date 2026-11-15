#pragma once
#include <iostream>
using namespace std;

struct BSTNode {
    int data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int val) {
        data = val;
        left = nullptr;
        right = nullptr;
    }
};

struct BSTQueueNode {
    BSTNode* treeNode;
    BSTQueueNode* next;

    BSTQueueNode(BSTNode* t) {
        treeNode = t;
        next = nullptr;
    }
};

struct BSTQueue {
    BSTQueueNode* front;
    BSTQueueNode* back;

    BSTQueue() {
        front = nullptr;
        back = nullptr;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(BSTNode* t) {
        BSTQueueNode* node = new BSTQueueNode(t);
        if (back == nullptr) {
            front = node;
            back = node;
        } else {
            back->next = node;
            back = node;
        }
    }

    BSTNode* dequeue() {
        if (isEmpty()) return nullptr;
        BSTQueueNode* temp = front;
        BSTNode* t = temp->treeNode;
        front = front->next;
        if (front == nullptr) back = nullptr;
        delete temp;
        return t;
    }
};

class BST {
public:
    BSTNode* root;

    BST() {
        root = nullptr;
    }

    ~BST() {
        destroyTree(root);
    }

    void destroyTree(BSTNode* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    // --- Step 1: Insert ---
    BSTNode* insertHelper(BSTNode* node, int key) {
        if (node == nullptr) return new BSTNode(key);
        if (key < node->data)
            node->left = insertHelper(node->left, key);
        else if (key > node->data)
            node->right = insertHelper(node->right, key);
        // duplicate: silently reject
        return node;
    }

    void insert(int key) {
        root = insertHelper(root, key);
    }

    // --- Step 2: Search ---
    bool searchHelper(BSTNode* node, int key) {
        if (node == nullptr) return false;
        if (key == node->data) return true;
        if (key < node->data) return searchHelper(node->left, key);
        return searchHelper(node->right, key);
    }

    bool search(int key) {
        return searchHelper(root, key);
    }

    // --- Step 3: Delete (all three cases) ---
    BSTNode* findMin(BSTNode* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    BSTNode* deleteHelper(BSTNode* node, int key) {
        if (node == nullptr) return nullptr;

        if (key < node->data) {
            node->left = deleteHelper(node->left, key);
        } else if (key > node->data) {
            node->right = deleteHelper(node->right, key);
        } else {
            // Case 1: leaf node
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            }
            // Case 2: one child
            if (node->left == nullptr) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            }
            if (node->right == nullptr) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            // Case 3: two children — replace with inorder successor
            BSTNode* successor = findMin(node->right);
            node->data = successor->data;
            node->right = deleteHelper(node->right, successor->data);
        }
        return node;
    }

    void deleteNode(int key) {
        root = deleteHelper(root, key);
    }

    // --- Step 4: Min and Max ---
    int getMin() {
        if (root == nullptr) return -1;
        BSTNode* cur = root;
        while (cur->left != nullptr)
            cur = cur->left;
        return cur->data;
    }

    int getMax() {
        if (root == nullptr) return -1;
        BSTNode* cur = root;
        while (cur->right != nullptr)
            cur = cur->right;
        return cur->data;
    }

    // --- Step 5: Successor and Predecessor ---
    int successor(int key) {
        BSTNode* cur = root;
        BSTNode* result = nullptr;
        while (cur != nullptr) {
            if (key < cur->data) {
                result = cur;
                cur = cur->left;
            } else if (key > cur->data) {
                cur = cur->right;
            } else {
                if (cur->right != nullptr) {
                    result = findMin(cur->right);
                }
                break;
            }
        }
        if (result == nullptr) return -1;
        return result->data;
    }

    int predecessor(int key) {
        BSTNode* cur = root;
        BSTNode* result = nullptr;
        while (cur != nullptr) {
            if (key > cur->data) {
                result = cur;
                cur = cur->right;
            } else if (key < cur->data) {
                cur = cur->left;
            } else {
                if (cur->left != nullptr) {
                    BSTNode* pred = cur->left;
                    while (pred->right != nullptr)
                        pred = pred->right;
                    result = pred;
                }
                break;
            }
        }
        if (result == nullptr) return -1;
        return result->data;
    }

    // --- Step 6: Range Query ---
    void rangeQueryHelper(BSTNode* node, int lo, int hi) {
        if (node == nullptr) return;
        if (node->data > lo)
            rangeQueryHelper(node->left, lo, hi);
        if (node->data >= lo && node->data <= hi)
            cout << node->data << " ";
        if (node->data < hi)
            rangeQueryHelper(node->right, lo, hi);
    }

    void rangeQuery(int lo, int hi) {
        rangeQueryHelper(root, lo, hi);
        cout << endl;
    }

    int countInRange(int lo, int hi) {
        int count = 0;
        countInRangeHelper(root, lo, hi, count);
        return count;
    }

    void countInRangeHelper(BSTNode* node, int lo, int hi, int& count) {
        if (node == nullptr) return;
        if (node->data > lo)
            countInRangeHelper(node->left, lo, hi, count);
        if (node->data >= lo && node->data <= hi)
            count++;
        if (node->data < hi)
            countInRangeHelper(node->right, lo, hi, count);
    }

    // --- Step 7: BST Validation ---
    bool validateHelper(BSTNode* node, int minVal, int maxVal) {
        if (node == nullptr) return true;
        if (node->data <= minVal || node->data >= maxVal) return false;
        return validateHelper(node->left, minVal, node->data) &&
               validateHelper(node->right, node->data, maxVal);
    }

    bool isValidBST() {
        return validateHelper(root, -2147483648, 2147483647);
    }

    // BST-specific LCA (uses ordering for O(log n) performance)
    int lcaBST(int a, int b) {
        BSTNode* cur = root;
        while (cur != nullptr) {
            if (a < cur->data && b < cur->data)
                cur = cur->left;
            else if (a > cur->data && b > cur->data)
                cur = cur->right;
            else
                return cur->data;
        }
        return -1;
    }

    // Height and degeneration check
    int height(BSTNode* node) {
        if (node == nullptr) return 0;
        int leftH = height(node->left);
        int rightH = height(node->right);
        if (leftH > rightH) return leftH + 1;
        return rightH + 1;
    }

    int getHeight() {
        return height(root);
    }

    bool isDegenerate() {
        BSTNode* cur = root;
        while (cur != nullptr) {
            if (cur->left != nullptr && cur->right != nullptr)
                return false;
            if (cur->left != nullptr) cur = cur->left;
            else cur = cur->right;
        }
        return true;
    }

    // Traversals (inorder gives sorted output for BST)
    void inorder(BSTNode* node) {
        if (node == nullptr) return;
        inorder(node->left);
        cout << node->data << " ";
        inorder(node->right);
    }

    void printInorder() {
        inorder(root);
        cout << endl;
    }

    void levelOrder() {
        if (root == nullptr) return;
        BSTQueue q;
        q.enqueue(root);
        while (!q.isEmpty()) {
            BSTNode* cur = q.dequeue();
            cout << cur->data << " ";
            if (cur->left != nullptr) q.enqueue(cur->left);
            if (cur->right != nullptr) q.enqueue(cur->right);
        }
        cout << endl;
    }
};

