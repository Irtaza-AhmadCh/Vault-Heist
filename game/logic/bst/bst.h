#ifndef BST_H
#define BST_H

#include <iostream>
using namespace std;

struct BSTNode {
    int id;
    int key;
    int value;
    BSTNode* left;
    BSTNode* right;
    BSTNode* parent;

    BSTNode();
    BSTNode(int k, int val = 0, int nodeId = -1);
};

class BST {
private:
    BSTNode* root;
    int nextId;

    BSTNode* insertRecursive(BSTNode* node, int key, BSTNode* parentNode);
    BSTNode* deleteRecursive(BSTNode* node, int key);
    BSTNode* findMinNode(BSTNode* node) const;
    BSTNode* findMaxNode(BSTNode* node) const;
    void clearRecursive(BSTNode* node);
    void inorderHelper(BSTNode* node) const;
    void levelOrderHelper(BSTNode* node) const;
    bool validateHelper(BSTNode* node, int minVal, int maxVal) const;
    int heightHelper(BSTNode* node) const;
    void rangeHelper(BSTNode* node, int lo, int hi) const;
    void countHelper(BSTNode* node, int lo, int hi, int& count) const;

public:
    BST();
    ~BST();

    void insert(int key);
    bool search(int key) const;
    void deleteNode(int key);

    int getMin() const;
    int getMax() const;

    int successor(int key) const;
    int predecessor(int key) const;
    int kthSmallest(int k) const;

    void rangeQuery(int lo, int hi) const;
    int countInRange(int lo, int hi) const;

    bool isValidBST() const;
    int lcaBST(int a, int b) const;
    int getHeight() const;
    bool isDegenerate() const;

    void printInorder() const;
    void levelOrder() const;

    BSTNode* getRoot() const;
    bool isEmpty() const;
};

#endif
