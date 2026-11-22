#include <iostream>
#include <cassert>
#include <cstring>
#include "../../game/logic/event.h"
#include "../../game/controller/UndoManager.h"
#include "../../game/logic/bst/bst.h"
#include "../../game/logic/avl/avl_tree.h"
#include "../../game/logic/heap/max_heap.h"
#include "../../game/logic/SaveManager.h"

// Test 1: EventLog copy and record
void testEventLogCopy() {
    EventLog log(16);
    log.logVisit(1, 50);
    log.logCompare(1, 50, "<");
    log.logInsert(2, 1, 0, 25);
    log.logRotate(1, ROTATION_LL);
    log.logHeapSwap(0, 1, 90, 80, "sift");
    log.logDone("Completed batch");

    assert(log.getCount() == 6);

    EventLog copyLog(16);
    copyLog.copyFrom(log);
    assert(copyLog.getCount() == 6);
    assert(strcmp(copyLog.getEvent(0).type, EVENT_VISIT) == 0);
    assert(strcmp(copyLog.getEvent(3).type, EVENT_ROTATE) == 0);
    assert(strcmp(copyLog.getEvent(4).type, EVENT_HEAP_SWAP) == 0);

    // Verify self-assignment is safe
    copyLog.copyFrom(copyLog);
    assert(copyLog.getCount() == 6);

    std::cout << "Test 1 PASSED: EventLog copy and deep memory handling.\n";
}

// Test 2: UndoManager recording and replay buffer
void testUndoManagerReplay() {
    UndoManager mgr;
    assert(!mgr.canUndo());
    assert(mgr.getLastActionLog().getCount() == 0);

    EventLog actionLog(16);
    actionLog.logCompare(10, 20, ">");
    actionLog.logInsert(3, 1, 1, 75);

    mgr.recordActionEvents(actionLog);
    assert(mgr.getLastActionLog().getCount() == 2);
    assert(strcmp(mgr.getLastActionLog().getEvent(1).type, EVENT_INSERT) == 0);

    // Recording an empty log should NOT wipe out the last non-empty action log
    EventLog emptyLog;
    mgr.recordActionEvents(emptyLog);
    assert(mgr.getLastActionLog().getCount() == 2);

    std::cout << "Test 2 PASSED: UndoManager replay buffer retention.\n";
}

// Test 3: BST Preorder Snapshot & Undo Restore
void testBSTUndo() {
    BST bst;
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);

    // Collect preorder keys
    int keys[64];
    int count = 0;
    auto collectBST = [](auto& self, BSTNode* node, int* arr, int& c) -> void {
        if (!node || c >= 64) return;
        arr[c++] = node->key;
        self(self, node->left, arr, c);
        self(self, node->right, arr, c);
    };
    collectBST(collectBST, bst.getRoot(), keys, count);
    assert(count == 5);
    assert(keys[0] == 50 && keys[1] == 30 && keys[2] == 20);

    // Push snapshot
    UndoManager mgr;
    mgr.pushSnapshot("bst", "", keys, count, 0, 1000, "Initial BST");
    assert(mgr.canUndo());

    // Mutate tree
    bst.insert(99);
    bst.deleteNode(20);
    assert(bst.search(99));
    assert(!bst.search(20));

    // Restore from snapshot
    TreeSnapshot snap;
    bool ok = mgr.popSnapshot(snap);
    assert(ok);
    assert(strcmp(snap.structureType, "bst") == 0);
    assert(snap.keyCount == 5);

    while (!bst.isEmpty()) bst.deleteNode(bst.getRoot()->key);
    for (int i = 0; i < snap.keyCount; i++) {
        bst.insert(snap.keys[i]);
    }

    // Verify restored state matches original
    assert(bst.search(20));
    assert(bst.search(50));
    assert(bst.search(70));
    assert(!bst.search(99));
    assert(bst.getRoot()->key == 50);

    std::cout << "Test 3 PASSED: BST Preorder Snapshot and Undo restoration.\n";
}

// Test 4: AVL Preorder Snapshot & Undo Restore (insertRaw)
void testAVLUndo() {
    AVLTree avl;
    avl.insert(30);
    avl.insert(20);
    avl.insert(40);
    avl.insert(10);
    avl.insert(25);

    int keys[64];
    int count = 0;
    auto collectAVL = [](auto& self, AVLNode* node, int* arr, int& c) -> void {
        if (!node || c >= 64) return;
        arr[c++] = node->key;
        self(self, node->left, arr, c);
        self(self, node->right, arr, c);
    };
    collectAVL(collectAVL, avl.getRoot(), keys, count);
    assert(count == 5);

    UndoManager mgr;
    mgr.pushSnapshot("avl", "", keys, count, 0, 1000, "Initial AVL");

    // Mutate
    avl.insert(5);
    avl.deleteNode(40);
    assert(avl.search(5));
    assert(!avl.search(40));

    // Restore
    TreeSnapshot snap;
    bool ok = mgr.popSnapshot(snap);
    assert(ok);

    while (!avl.isEmpty()) avl.deleteNode(avl.getRoot()->key);
    for (int i = 0; i < snap.keyCount; i++) {
        avl.insertRaw(snap.keys[i]);
    }

    assert(avl.search(40));
    assert(!avl.search(5));
    assert(avl.getRoot()->key == 30);
    assert(avl.isBalanced());

    std::cout << "Test 4 PASSED: AVL Preorder Snapshot and Undo restoration.\n";
}

// Test 5: Max Heap Snapshot & Undo Restore
void testHeapUndo() {
    MaxHeap heap;
    int initial[] = {90, 75, 80, 50, 60};
    heap.buildMaxHeap(initial, 5);

    UndoManager mgr;
    mgr.pushSnapshot("heap", "", heap.getArray(), heap.getSize(), 0, 1000, "Initial Heap");

    // Mutate
    int extracted = heap.extractMax();
    assert(extracted == 90);
    assert(heap.getSize() == 4);

    // Restore
    TreeSnapshot snap;
    bool ok = mgr.popSnapshot(snap);
    assert(ok);
    heap.loadRawArray(snap.keys, snap.keyCount);

    assert(heap.getSize() == 5);
    assert(heap.peekMax() == 90);

    std::cout << "Test 5 PASSED: Max Heap Snapshot and Undo restoration.\n";
}

int main() {
    std::cout << "Running Event, Undo, and Replay System Tests...\n";
    testEventLogCopy();
    testUndoManagerReplay();
    testBSTUndo();
    testAVLUndo();
    testHeapUndo();
    std::cout << "\nALL 5 TESTS PASSED SUCCESSFULLY!\n";
    return 0;
}
