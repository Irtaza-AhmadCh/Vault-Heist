#pragma once
#include <iostream>
#include "../event.h"
using namespace std;

struct TreeNode {
    int data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int val) {
        data = val;
        left = nullptr;
        right = nullptr;
    }
};

struct QueueNode {
    TreeNode* treeNode;
    QueueNode* next;

    QueueNode(TreeNode* t) {
        treeNode = t;
        next = nullptr;
    }
};

struct SimpleQueue {
    QueueNode* front;
    QueueNode* back;

    SimpleQueue() {
        front = nullptr;
        back = nullptr;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(TreeNode* t) {
        QueueNode* node = new QueueNode(t);
        if (back == nullptr) {
            front = node;
            back = node;
        } else {
            back->next = node;
            back = node;
        }
    }

    TreeNode* dequeue() {
        if (isEmpty()) return nullptr;
        QueueNode* temp = front;
        TreeNode* t = temp->treeNode;
        front = front->next;
        if (front == nullptr) back = nullptr;
        delete temp;
        return t;
    }
};

class BinaryTree {
public:
    TreeNode* root;
    EventLog* eventLog;

    BinaryTree(EventLog* log = nullptr) {
        root = nullptr;
        eventLog = log;
    }

    void setEventLog(EventLog* log) {
        eventLog = log;
    }

    EventLog* getEventLog() const {
        return eventLog;
    }

    ~BinaryTree() {
        destroyTree(root);
    }

    void destroyTree(TreeNode* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    void preorder(TreeNode* node) {
        if (node == nullptr) return;
        if (eventLog != nullptr) {
            eventLog->logVisit(node->data, node->data);
        }
        cout << node->data << " ";
        preorder(node->left);
        preorder(node->right);
    }

    void inorder(TreeNode* node) {
        if (node == nullptr) return;
        inorder(node->left);
        if (eventLog != nullptr) {
            eventLog->logVisit(node->data, node->data);
        }
        cout << node->data << " ";
        inorder(node->right);
    }

    void postorder(TreeNode* node) {
        if (node == nullptr) return;
        postorder(node->left);
        postorder(node->right);
        if (eventLog != nullptr) {
            eventLog->logVisit(node->data, node->data);
        }
        cout << node->data << " ";
    }

    void levelOrder() {
        if (root == nullptr) return;
        SimpleQueue q;
        q.enqueue(root);
        while (!q.isEmpty()) {
            TreeNode* current = q.dequeue();
            if (eventLog != nullptr) {
                eventLog->logVisit(current->data, current->data);
            }
            cout << current->data << " ";
            if (current->left != nullptr) q.enqueue(current->left);
            if (current->right != nullptr) q.enqueue(current->right);
        }
        cout << endl;
    }

    int height(TreeNode* node) {
        if (node == nullptr) return 0;
        int leftH = height(node->left);
        int rightH = height(node->right);
        if (leftH > rightH) return leftH + 1;
        return rightH + 1;
    }

    int diameterHelper(TreeNode* node, int& maxDiam) {
        if (node == nullptr) return 0;
        int leftH = diameterHelper(node->left, maxDiam);
        int rightH = diameterHelper(node->right, maxDiam);
        int through = leftH + rightH;
        if (through > maxDiam) maxDiam = through;
        if (leftH > rightH) return leftH + 1;
        return rightH + 1;
    }

    int diameter() {
        int maxDiam = 0;
        diameterHelper(root, maxDiam);
        return maxDiam;
    }

    TreeNode* buildFromPreIn(int* pre, int* in, int start, int end, int& preIndex, int size) {
        if (start > end) return nullptr;
        int rootVal = pre[preIndex];
        preIndex++;
        TreeNode* node = new TreeNode(rootVal);
        int inIndex = -1;
        for (int i = start; i <= end; i++) {
            if (in[i] == rootVal) {
                inIndex = i;
                break;
            }
        }
        node->left = buildFromPreIn(pre, in, start, inIndex - 1, preIndex, size);
        node->right = buildFromPreIn(pre, in, inIndex + 1, end, preIndex, size);
        return node;
    }

    void reconstructFromPreIn(int* pre, int* in, int size) {
        destroyTree(root);
        int preIndex = 0;
        root = buildFromPreIn(pre, in, 0, size - 1, preIndex, size);
    }

    TreeNode* buildFromPostIn(int* post, int* in, int start, int end, int& postIndex) {
        if (start > end) return nullptr;
        int rootVal = post[postIndex];
        postIndex--;
        TreeNode* node = new TreeNode(rootVal);
        int inIndex = -1;
        for (int i = start; i <= end; i++) {
            if (in[i] == rootVal) {
                inIndex = i;
                break;
            }
        }
        node->right = buildFromPostIn(post, in, inIndex + 1, end, postIndex);
        node->left = buildFromPostIn(post, in, start, inIndex - 1, postIndex);
        return node;
    }

    void reconstructFromPostIn(int* post, int* in, int size) {
        destroyTree(root);
        int postIndex = size - 1;
        root = buildFromPostIn(post, in, 0, size - 1, postIndex);
    }

    TreeNode* lcaHelper(TreeNode* node, int a, int b) {
        if (node == nullptr) return nullptr;
        if (node->data == a || node->data == b) return node;
        TreeNode* leftLCA = lcaHelper(node->left, a, b);
        TreeNode* rightLCA = lcaHelper(node->right, a, b);
        if (leftLCA != nullptr && rightLCA != nullptr) return node;
        if (leftLCA != nullptr) return leftLCA;
        return rightLCA;
    }

    int lca(int a, int b) {
        TreeNode* result = lcaHelper(root, a, b);
        if (result == nullptr) return -1;
        return result->data;
    }

    bool findPath(TreeNode* node, int target, int* path, int& pathLen) {
        if (node == nullptr) return false;
        path[pathLen] = node->data;
        pathLen++;
        if (node->data == target) return true;
        if (findPath(node->left, target, path, pathLen)) return true;
        if (findPath(node->right, target, path, pathLen)) return true;
        pathLen--;
        return false;
    }

    int pathLength(int a, int b) {
        int pathA[100], pathB[100];
        int lenA = 0, lenB = 0;
        findPath(root, a, pathA, lenA);
        findPath(root, b, pathB, lenB);
        int lcaNode = lca(a, b);
        int lcaDepthA = -1, lcaDepthB = -1;
        for (int i = 0; i < lenA; i++) {
            if (pathA[i] == lcaNode) { lcaDepthA = i; break; }
        }
        for (int i = 0; i < lenB; i++) {
            if (pathB[i] == lcaNode) { lcaDepthB = i; break; }
        }
        if (lcaDepthA == -1 || lcaDepthB == -1) return -1;
        return (lenA - 1 - lcaDepthA) + (lenB - 1 - lcaDepthB);
    }

    void mirror(TreeNode* node) {
        if (node == nullptr) return;
        if (eventLog != nullptr && node->left != nullptr && node->right != nullptr) {
            eventLog->logSwapValue(node->left->data, node->right->data, node->left->data, node->right->data);
        }
        TreeNode* temp = node->left;
        node->left = node->right;
        node->right = temp;
        mirror(node->left);
        mirror(node->right);
    }

    bool isSymmetricHelper(TreeNode* left, TreeNode* right) {
        if (left == nullptr && right == nullptr) return true;
        if (left == nullptr || right == nullptr) return false;
        if (left->data != right->data) return false;
        return isSymmetricHelper(left->left, right->right) &&
               isSymmetricHelper(left->right, right->left);
    }

    bool isSymmetric() {
        if (root == nullptr) return true;
        return isSymmetricHelper(root->left, root->right);
    }

    bool isSameStructure(TreeNode* a, TreeNode* b) {
        if (a == nullptr && b == nullptr) return true;
        if (a == nullptr || b == nullptr) return false;
        if (a->data != b->data) return false;
        return isSameStructure(a->left, b->left) && isSameStructure(a->right, b->right);
    }

    void getPreorderHelper(TreeNode* node, int* arr, int& count) {
        if (node == nullptr) return;
        arr[count++] = node->data;
        getPreorderHelper(node->left, arr, count);
        getPreorderHelper(node->right, arr, count);
    }

    void getPreorder(int* arr, int& count) {
        count = 0;
        getPreorderHelper(root, arr, count);
    }

    void getInorderHelper(TreeNode* node, int* arr, int& count) {
        if (node == nullptr) return;
        getInorderHelper(node->left, arr, count);
        arr[count++] = node->data;
        getInorderHelper(node->right, arr, count);
    }

    void getInorder(int* arr, int& count) {
        count = 0;
        getInorderHelper(root, arr, count);
    }

    void getPostorderHelper(TreeNode* node, int* arr, int& count) {
        if (node == nullptr) return;
        getPostorderHelper(node->left, arr, count);
        getPostorderHelper(node->right, arr, count);
        arr[count++] = node->data;
    }

    void getPostorder(int* arr, int& count) {
        count = 0;
        getPostorderHelper(root, arr, count);
    }

    void getLevelOrder(int* arr, int& count) {
        count = 0;
        if (root == nullptr) return;
        SimpleQueue q;
        q.enqueue(root);
        while (!q.isEmpty()) {
            TreeNode* cur = q.dequeue();
            arr[count++] = cur->data;
            if (cur->left != nullptr) q.enqueue(cur->left);
            if (cur->right != nullptr) q.enqueue(cur->right);
        }
    }

    int heightEdge(TreeNode* node) {
        if (node == nullptr) return -1;
        if (node->left == nullptr && node->right == nullptr) return 0;
        int leftH = heightEdge(node->left);
        int rightH = heightEdge(node->right);
        return 1 + (leftH > rightH ? leftH : rightH);
    }

    TreeNode* cloneTree(TreeNode* node) {
        if (node == nullptr) return nullptr;
        TreeNode* copy = new TreeNode(node->data);
        copy->left = cloneTree(node->left);
        copy->right = cloneTree(node->right);
        return copy;
    }

    TreeNode* findNode(TreeNode* node, int val) {
        if (node == nullptr) return nullptr;
        if (node->data == val) return node;
        TreeNode* leftFound = findNode(node->left, val);
        if (leftFound != nullptr) return leftFound;
        return findNode(node->right, val);
    }

    int countNodes(TreeNode* node) {
        if (node == nullptr) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    bool isSameTree(TreeNode* a, TreeNode* b) {
        return isSameStructure(a, b);
    }

    bool getPathNodes(int a, int b, int* outPath, int& outLen) {
        outLen = 0;
        int pathA[64], pathB[64];
        int lenA = 0, lenB = 0;
        if (!findPath(root, a, pathA, lenA)) return false;
        if (!findPath(root, b, pathB, lenB)) return false;

        int lcaVal = lca(a, b);
        int lcaIdxA = -1, lcaIdxB = -1;
        for (int i = 0; i < lenA; i++) {
            if (pathA[i] == lcaVal) { lcaIdxA = i; break; }
        }
        for (int i = 0; i < lenB; i++) {
            if (pathB[i] == lcaVal) { lcaIdxB = i; break; }
        }
        if (lcaIdxA == -1 || lcaIdxB == -1) return false;

        // Path from a up to LCA
        for (int i = lenA - 1; i >= lcaIdxA; i--) {
            outPath[outLen++] = pathA[i];
        }
        // Path from after LCA down to b
        for (int i = lcaIdxB + 1; i < lenB; i++) {
            outPath[outLen++] = pathB[i];
        }
        return true;
    }

    void printPreorder() {
        preorder(root);
        cout << endl;
    }

    void printInorder() {
        inorder(root);
        cout << endl;
    }

    void printPostorder() {
        postorder(root);
        cout << endl;
    }
};

