#ifndef BST_H
#define BST_H

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
    BSTNode* findMin(BSTNode* node) const;
    void clearRecursive(BSTNode* node);

public:
    BST();
    ~BST();

    void insert(int key);
    bool search(int key) const;
    void deleteNode(int key);

    BSTNode* getRoot() const;
    bool isEmpty() const;
};

#endif

