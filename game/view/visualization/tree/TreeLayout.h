#ifndef TREE_LAYOUT_H
#define TREE_LAYOUT_H

#include <cmath>
#include <cstring>

// Predefined string identifiers for NodeVisualState:
// "normal", "highlighted", "inactive", "searching", "target", "swapping", "sorted"

struct VisualNode {
    int key;
    float x;
    float y;
    float targetX;
    float targetY;
    float radius;
    char state[16];
    float pulseTimer;
    int parentIndex;
    int leftChildIndex;
    int rightChildIndex;
    bool active;

    // Member B additions for AVL and Heap
    int balanceFactor;
    int subtreeSize;
    int heapIndex;

    VisualNode() {
        key = 0;
        x = 0.0f;
        y = 0.0f;
        targetX = 0.0f;
        targetY = 0.0f;
        radius = 28.0f;
        setState("normal");
        pulseTimer = 0.0f;
        parentIndex = -1;
        leftChildIndex = -1;
        rightChildIndex = -1;
        active = false;
        balanceFactor = 0;
        subtreeSize = 1;
        heapIndex = -1;
    }

    void setState(const char* s) {
        if (s == nullptr) return;
        strncpy(state, s, sizeof(state) - 1);
        state[sizeof(state) - 1] = '\0';
    }

    bool isState(const char* s) const {
        if (s == nullptr) return false;
        return strcmp(state, s) == 0;
    }
};

class TreeLayoutEngine {
public:
    static const int MAX_NODES = 128;
    VisualNode nodes[MAX_NODES];
    int nodeCount;
    float canvasCenterX;
    float canvasTopY;
    float levelHeight;

    TreeLayoutEngine() {
        nodeCount = 0;
        canvasCenterX = 640.0f;
        canvasTopY = 165.0f;
        levelHeight = 90.0f;
    }

    void clear() {
        for (int i = 0; i < MAX_NODES; i++) {
            nodes[i].active = false;
            nodes[i].parentIndex = -1;
            nodes[i].leftChildIndex = -1;
            nodes[i].rightChildIndex = -1;
            nodes[i].setState("normal");
            nodes[i].x = 0.0f;
            nodes[i].y = 0.0f;
            nodes[i].targetX = 0.0f;
            nodes[i].targetY = 0.0f;
            nodes[i].pulseTimer = 0.0f;
            nodes[i].balanceFactor = 0;
            nodes[i].subtreeSize = 1;
            nodes[i].heapIndex = -1;
        }
        nodeCount = 0;
    }

    int allocateNode(int key) {
        if (nodeCount >= MAX_NODES) return -1;
        int idx = nodeCount++;
        nodes[idx].key = key;
        nodes[idx].active = true;
        nodes[idx].setState("normal");
        nodes[idx].pulseTimer = 0.0f;
        nodes[idx].parentIndex = -1;
        nodes[idx].leftChildIndex = -1;
        nodes[idx].rightChildIndex = -1;
        nodes[idx].x = 0.0f;
        nodes[idx].y = 0.0f;
        nodes[idx].targetX = 0.0f;
        nodes[idx].targetY = 0.0f;
        nodes[idx].balanceFactor = 0;
        nodes[idx].subtreeSize = 1;
        nodes[idx].heapIndex = -1;
        return idx;
    }

    int findRootIndex() const {
        for (int i = 0; i < nodeCount; i++) {
            if (nodes[i].active && nodes[i].parentIndex == -1) {
                return i;
            }
        }
        return (nodeCount > 0 && nodes[0].active) ? 0 : -1;
    }

    void assignInorderPositions(int idx, int depth, int& orderCount, int inorderIndices[], int depths[]) {
        if (idx < 0 || idx >= nodeCount || !nodes[idx].active) return;
        depths[idx] = depth;

        if (nodes[idx].leftChildIndex != -1) {
            assignInorderPositions(nodes[idx].leftChildIndex, depth + 1, orderCount, inorderIndices, depths);
        }

        inorderIndices[idx] = orderCount++;

        if (nodes[idx].rightChildIndex != -1) {
            assignInorderPositions(nodes[idx].rightChildIndex, depth + 1, orderCount, inorderIndices, depths);
        }
    }

    void centerParents(int idx) {
        if (idx < 0 || idx >= nodeCount || !nodes[idx].active) return;

        int left = nodes[idx].leftChildIndex;
        int right = nodes[idx].rightChildIndex;

        if (left != -1) centerParents(left);
        if (right != -1) centerParents(right);

        if (left != -1 && right != -1) {
            nodes[idx].targetX = (nodes[left].targetX + nodes[right].targetX) * 0.5f;
        } else if (left != -1) {
            nodes[idx].targetX = nodes[left].targetX + 35.0f;
        } else if (right != -1) {
            nodes[idx].targetX = nodes[right].targetX - 35.0f;
        }
    }

    template <typename SlotType>
    void assignInorderWithSlots(int idx, int depth, int& orderCount,
                                int inorderIndices[], int depths[],
                                SlotType slots[], int slotParentIdx[],
                                int slotInorder[], int slotDepth[],
                                int& slotCount, int maxSlots)
    {
        if (idx < 0 || idx >= nodeCount || !nodes[idx].active) return;
        depths[idx] = depth;

        if (nodes[idx].leftChildIndex != -1) {
            assignInorderWithSlots(nodes[idx].leftChildIndex, depth + 1, orderCount,
                                   inorderIndices, depths, slots, slotParentIdx,
                                   slotInorder, slotDepth, slotCount, maxSlots);
        } else {
            if (slotCount < maxSlots) {
                int s = slotCount++;
                slots[s].parentKey = nodes[idx].key;
                slots[s].isLeft = true;
                slots[s].isTarget = false;
                slotParentIdx[s] = idx;
                slotDepth[s] = depth + 1;
                slotInorder[s] = orderCount++;
            }
        }

        inorderIndices[idx] = orderCount++;

        if (nodes[idx].rightChildIndex != -1) {
            assignInorderWithSlots(nodes[idx].rightChildIndex, depth + 1, orderCount,
                                   inorderIndices, depths, slots, slotParentIdx,
                                   slotInorder, slotDepth, slotCount, maxSlots);
        } else {
            if (slotCount < maxSlots) {
                int s = slotCount++;
                slots[s].parentKey = nodes[idx].key;
                slots[s].isLeft = false;
                slots[s].isTarget = false;
                slotParentIdx[s] = idx;
                slotDepth[s] = depth + 1;
                slotInorder[s] = orderCount++;
            }
        }
    }

    template <typename SlotType>
    void centerParentsWithSlots(int idx, SlotType slots[], const int slotParentIdx[], int slotCount) {
        if (idx < 0 || idx >= nodeCount || !nodes[idx].active) return;

        int left = nodes[idx].leftChildIndex;
        int right = nodes[idx].rightChildIndex;

        if (left != -1) centerParentsWithSlots(left, slots, slotParentIdx, slotCount);
        if (right != -1) centerParentsWithSlots(right, slots, slotParentIdx, slotCount);

        float leftX = nodes[idx].targetX;
        float rightX = nodes[idx].targetX;
        bool hasLeft = false;
        bool hasRight = false;

        if (left != -1) {
            leftX = nodes[left].targetX;
            hasLeft = true;
        } else {
            for (int s = 0; s < slotCount; s++) {
                if (slotParentIdx[s] == idx && slots[s].isLeft) {
                    leftX = slots[s].x;
                    hasLeft = true;
                    break;
                }
            }
        }

        if (right != -1) {
            rightX = nodes[right].targetX;
            hasRight = true;
        } else {
            for (int s = 0; s < slotCount; s++) {
                if (slotParentIdx[s] == idx && !slots[s].isLeft) {
                    rightX = slots[s].x;
                    hasRight = true;
                    break;
                }
            }
        }

        if (hasLeft && hasRight) {
            nodes[idx].targetX = (leftX + rightX) * 0.5f;
        } else if (hasLeft) {
            nodes[idx].targetX = leftX + 45.0f;
        } else if (hasRight) {
            nodes[idx].targetX = rightX - 45.0f;
        }
    }

    template <typename SlotType>
    void updateLayoutWithNullSlots(float screenWidth, SlotType slots[], int& slotCount, int maxSlots, bool snapImmediate = true) {
        int rootIdx = findRootIndex();
        if (rootIdx == -1) {
            slotCount = 0;
            return;
        }

        int inorderIndices[MAX_NODES];
        int depths[MAX_NODES];
        for (int i = 0; i < MAX_NODES; i++) {
            inorderIndices[i] = -1;
            depths[i] = 0;
        }

        int slotParentIdx[64];
        int slotInorder[64];
        int slotDepth[64];
        slotCount = 0;

        int totalOrdered = 0;
        assignInorderWithSlots(rootIdx, 0, totalOrdered, inorderIndices, depths,
                               slots, slotParentIdx, slotInorder, slotDepth, slotCount, maxSlots);
        if (totalOrdered == 0) return;

        float minSpacing = (totalOrdered > 16) ? 52.0f : ((totalOrdered > 10) ? 60.0f : 74.0f);
        float totalWidth = (totalOrdered - 1) * minSpacing;
        float startX = canvasCenterX - totalWidth * 0.5f;
        if (startX < 70.0f) startX = 70.0f;

        for (int i = 0; i < nodeCount; i++) {
            if (!nodes[i].active || inorderIndices[i] == -1) continue;
            nodes[i].targetX = startX + inorderIndices[i] * minSpacing;
            nodes[i].targetY = canvasTopY + depths[i] * levelHeight;
        }

        for (int s = 0; s < slotCount; s++) {
            slots[s].x = startX + slotInorder[s] * minSpacing;
            slots[s].y = canvasTopY + slotDepth[s] * levelHeight;
        }

        // Apply bottom-up centering for parents over their children (real or null slots)
        centerParentsWithSlots(rootIdx, slots, slotParentIdx, slotCount);

        for (int i = 0; i < nodeCount; i++) {
            if (!nodes[i].active) continue;
            if (snapImmediate || (nodes[i].x == 0.0f && nodes[i].y == 0.0f)) {
                nodes[i].x = nodes[i].targetX;
                nodes[i].y = nodes[i].targetY;
            }
        }
    }

    void updateLayout(float screenWidth, bool snapImmediate = true) {
        int rootIdx = findRootIndex();
        if (rootIdx == -1) return;

        int inorderIndices[MAX_NODES];
        int depths[MAX_NODES];
        for (int i = 0; i < MAX_NODES; i++) {
            inorderIndices[i] = -1;
            depths[i] = 0;
        }

        int totalOrdered = 0;
        assignInorderPositions(rootIdx, 0, totalOrdered, inorderIndices, depths);
        if (totalOrdered == 0) return;

        float minSpacing = (totalOrdered > 14) ? 55.0f : 75.0f;
        float totalWidth = (totalOrdered - 1) * minSpacing;
        float startX = canvasCenterX - totalWidth * 0.5f;
        if (startX < 70.0f) startX = 70.0f;

        for (int i = 0; i < nodeCount; i++) {
            if (!nodes[i].active || inorderIndices[i] == -1) continue;
            nodes[i].targetX = startX + inorderIndices[i] * minSpacing;
            nodes[i].targetY = canvasTopY + depths[i] * levelHeight;
        }

        // Apply bottom-up centering for parents over their children
        centerParents(rootIdx);

        for (int i = 0; i < nodeCount; i++) {
            if (!nodes[i].active) continue;
            if (snapImmediate || (nodes[i].x == 0.0f && nodes[i].y == 0.0f)) {
                nodes[i].x = nodes[i].targetX;
                nodes[i].y = nodes[i].targetY;
            }
        }
    }

    void updatePhysics(float dt) {
        float speed = 12.0f;
        for (int i = 0; i < nodeCount; i++) {
            if (!nodes[i].active) continue;

            float dx = nodes[i].targetX - nodes[i].x;
            float dy = nodes[i].targetY - nodes[i].y;

            nodes[i].x += dx * speed * dt;
            nodes[i].y += dy * speed * dt;

            nodes[i].pulseTimer += dt;
        }
    }

    int findNodeAt(float mouseX, float mouseY) {
        for (int i = 0; i < nodeCount; i++) {
            if (!nodes[i].active) continue;
            float dx = mouseX - nodes[i].x;
            float dy = mouseY - nodes[i].y;
            float hitRadius = nodes[i].radius + 8.0f;
            if (std::sqrt(dx * dx + dy * dy) <= hitRadius) {
                return i;
            }
        }
        return -1;
    }
};

#endif
