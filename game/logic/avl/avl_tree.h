#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "../event.h"

struct AVLNode {
    int id;
    int key;
    int value;
    int height;
    int size;
    AVLNode* left;
    AVLNode* right;
    AVLNode* parent;

    AVLNode();
    AVLNode(int k, int val = 0, int nodeId = -1);
};

class AVLTree {
private:
    AVLNode* root;
    int nextId;
    EventLog* eventLog;

    int maxInt(int a, int b) const;
    void updateHeightAndSize(AVLNode* node);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rebalance(AVLNode* node);

    AVLNode* insertRecursive(AVLNode* node, int key, int val, AVLNode* parentNode);
    AVLNode* deleteRecursive(AVLNode* node, int key);
    AVLNode* findMin(AVLNode* node) const;
    void clearRecursive(AVLNode* node);

    int rankRecursive(AVLNode* node, int key) const;
    int kthRecursive(AVLNode* node, int k) const;
    bool validateRecursive(AVLNode* node, int minVal, int maxVal) const;

public:
    AVLTree(EventLog* log = nullptr);
    ~AVLTree();

    void setEventLog(EventLog* log);
    EventLog* getEventLog() const;

    int height(AVLNode* node) const;
    int subtreeSize(AVLNode* node) const;
    int balanceFactor(AVLNode* node) const;

    void insert(int key, int value = 0);
    void deleteNode(int key);
    bool search(int key) const;

    int kth(int k) const;
    int rank(int key) const;
    int countInRange(int lo, int hi) const;

    bool isBalanced() const;
    bool isBalanced(AVLNode* node) const;

    AVLNode* getRoot() const;
    int getRootHeight() const;
    int getRootSize() const;
    bool isEmpty() const;

    AVLNode* findNode(int key) const;
    void insertRaw(int key, int value = 0);
    bool manualRotateLeft(int pivotKey);
    bool manualRotateRight(int pivotKey);
    bool manualRotateLR(int pivotKey);
    bool manualRotateRL(int pivotKey);
    const char* predictRotationOnInsert(int key) const;
};

#endif

