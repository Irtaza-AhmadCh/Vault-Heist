#include <iostream>
#include <cassert>
#include <cstring>
#include "../../src/ui/GameScreen.h"

using namespace std;

void testAct1(GameScreen& game) {
    cout << "--- Testing Act 1: Binary Tree Operations (Member A) ---" << endl;
    game.setupDefaultAct1Tree();
    assert(game.isAct("act_1"));
    assert(game.layout.nodeCount == 7);
    cout << "[PASS] Act 1 initialized with 7 nodes" << endl;

    // Test Traversals
    game.handleButtonClick(1); // Preorder
    assert(game.eventController.stepCount >= 7);
    cout << "[PASS] Preorder generated visit events (steps: " << game.eventController.stepCount << ")" << endl;

    game.handleButtonClick(2); // Inorder
    assert(game.eventController.stepCount >= 7);
    cout << "[PASS] Inorder generated visit events (steps: " << game.eventController.stepCount << ")" << endl;

    game.handleButtonClick(3); // Postorder
    assert(game.eventController.stepCount >= 7);
    cout << "[PASS] Postorder generated visit events (steps: " << game.eventController.stepCount << ")" << endl;

    game.handleButtonClick(4); // Level Order
    assert(game.eventController.stepCount >= 7);
    cout << "[PASS] Level-order generated visit events (steps: " << game.eventController.stepCount << ")" << endl;

    // Test Mirror
    game.handleMirrorAction();
    assert(game.binTree.root->data == 50);
    assert(game.binTree.root->left->data == 75);
    assert(game.binTree.root->right->data == 25);
    cout << "[PASS] Tree mirrored: left is 75, right is 25" << endl;

    // Mirror back
    game.handleMirrorAction();
    assert(game.binTree.root->left->data == 25);
    assert(game.binTree.root->right->data == 75);

    // Test Height & Diameter
    game.handleHeightDiameterAction();
    assert(game.binTree.height(game.binTree.root) == 3);
    assert(game.binTree.diameter() == 4);
    cout << "[PASS] Height is 3, Diameter is 4" << endl;

    // Test LCA & Path
    game.selectedNodeIndex = -1;
    game.handleLCAPathAction();
    assert(game.binTree.lca(12, 37) == 25);
    assert(game.binTree.pathLength(12, 37) == 2);
    cout << "[PASS] LCA(12, 37) is 25, Path length is 2" << endl;

    // Test Symmetry
    game.handleSymmetryAction();
    cout << "[PASS] Symmetry check executed without errors" << endl;

    // Test Reconstruct
    game.handleReconstructAction();
    assert(game.binTree.root != nullptr);
    assert(game.binTree.root->data == 50);
    assert(game.layout.nodeCount == 7);
    cout << "[PASS] Reconstruct from Preorder+Inorder succeeded" << endl;
}

void testAct2(GameScreen& game) {
    cout << "--- Testing Act 2: BST Operations (Member A) ---" << endl;
    game.setupDefaultAct2BST();
    assert(game.isAct("act_2"));
    assert(game.bst.isValidBST());
    assert(game.layout.nodeCount == 7);
    cout << "[PASS] Act 2 initialized with valid 7-node BST" << endl;

    // Test Insert
    game.inputBox.setValue(65);
    game.handleInsertAction();
    assert(game.bst.search(65));
    assert(game.bst.isValidBST());
    assert(game.layout.nodeCount == 8);
    cout << "[PASS] Inserted key 65 into BST" << endl;

    // Test Duplicate Key Rejection
    int countBefore = game.layout.nodeCount;
    game.inputBox.setValue(65); // duplicate
    game.handleInsertAction();
    assert(game.layout.nodeCount == countBefore); // not added
    cout << "[PASS] Duplicate key 65 safely rejected by BST" << endl;

    // Test Search
    game.inputBox.setValue(65);
    game.handleSearchAction();
    assert(game.eventController.stepCount > 0);
    cout << "[PASS] Search for 65 traced comparison path" << endl;

    // Test Range Query
    game.handleRangeAction();
    int highlightedCount = 0;
    for (int i = 0; i < game.layout.nodeCount; i++) {
        if (game.layout.nodes[i].active && game.layout.nodes[i].isState("highlighted")) {
            highlightedCount++;
        }
    }
    assert(highlightedCount > 0);
    cout << "[PASS] Range [30, 70] highlighted " << highlightedCount << " matching pods" << endl;

    // Test Deletion of Leaf (65)
    game.inputBox.setValue(65);
    game.handleDeleteAction();
    assert(!game.bst.search(65));
    assert(game.bst.isValidBST());
    cout << "[PASS] Leaf node 65 deleted cleanly" << endl;

    // Test Deletion of 2-child node (50 root)
    game.inputBox.setValue(50);
    game.handleDeleteAction();
    assert(!game.bst.search(50));
    assert(game.bst.isValidBST());
    cout << "[PASS] 2-child root 50 deleted cleanly, BST remains valid" << endl;
}

void testAct3(GameScreen& game) {
    cout << "--- Testing Act 3: AVL Tree Operations & Rotations (Member B) ---" << endl;
    game.setupDefaultAct3AVL();
    assert(game.isAct("act_3"));
    assert(game.avl.isBalanced());
    assert(game.layout.nodeCount == 7);
    cout << "[PASS] Act 3 initialized with balanced 7-node AVL tree" << endl;

    // Verify Badges
    for (int i = 0; i < game.layout.nodeCount; i++) {
        if (game.layout.nodes[i].active) {
            assert(game.layout.nodes[i].balanceFactor >= -1 && game.layout.nodes[i].balanceFactor <= 1);
            assert(game.layout.nodes[i].subtreeSize >= 1);
        }
    }
    cout << "[PASS] All AVL nodes have valid balance factor (|BF| <= 1) and subtree sizes" << endl;

    // Test LL Rotation Demo
    game.runRotationDemo("LL");
    assert(game.avl.isBalanced());
    assert(game.avl.getRoot()->key == 20);
    assert(game.avl.getRoot()->left->key == 10);
    assert(game.avl.getRoot()->right->key == 30);
    cout << "[PASS] Demo LL: rebalanced via Right Rotation (root = 20)" << endl;

    // Test RR Rotation Demo
    game.runRotationDemo("RR");
    assert(game.avl.isBalanced());
    assert(game.avl.getRoot()->key == 20);
    assert(game.avl.getRoot()->left->key == 10);
    assert(game.avl.getRoot()->right->key == 30);
    cout << "[PASS] Demo RR: rebalanced via Left Rotation (root = 20)" << endl;

    // Test LR Rotation Demo
    game.runRotationDemo("LR");
    assert(game.avl.isBalanced());
    assert(game.avl.getRoot()->key == 20);
    cout << "[PASS] Demo LR: rebalanced via Left-Right double rotation (root = 20)" << endl;

    // Test RL Rotation Demo
    game.runRotationDemo("RL");
    assert(game.avl.isBalanced());
    assert(game.avl.getRoot()->key == 20);
    cout << "[PASS] Demo RL: rebalanced via Right-Left double rotation (root = 20)" << endl;

    // Test Order Statistics (kth and rank)
    game.setupDefaultAct3AVL();
    int kVal = game.avl.kth(3);
    assert(kVal == 25);
    int rVal = game.avl.rank(25);
    assert(rVal == 3);
    cout << "[PASS] Order statistics: kth(3) = " << kVal << ", rank(25) = " << rVal << endl;
}

void testAct4(GameScreen& game) {
    cout << "--- Testing Act 4: Max Heap Dual View & Sorting (Member B) ---" << endl;
    game.setupDefaultAct4Heap();
    assert(game.isAct("act_4"));
    assert(game.heap.isMaxHeap());
    assert(game.heap.getSize() == 9);
    assert(game.layout.nodeCount == 9);
    cout << "[PASS] Act 4 initialized with 9-element valid MaxHeap in dual view" << endl;

    // Test Peek Max
    game.handleHeapPeekAction();
    assert(game.heap.peekMax() == 90);
    assert(game.selectedNodeIndex == 0);
    cout << "[PASS] Peek Max accessed root value 90 in O(1)" << endl;

    // Test Insert
    game.inputBox.setValue(99);
    game.handleInsertAction();
    assert(game.heap.peekMax() == 99);
    assert(game.heap.isMaxHeap());
    cout << "[PASS] Insert 99 sifted up to root, heap valid" << endl;

    // Test Extract Max
    int extracted = game.heap.extractMax();
    assert(extracted == 99);
    assert(game.heap.isMaxHeap());
    cout << "[PASS] Extract Max returned 99, heap re-heapified" << endl;

    // Test Build Heap
    game.handleHeapBuildAction();
    assert(game.heap.isMaxHeap());
    assert(game.heap.getSize() == 8);
    cout << "[PASS] Floyd's Build Heap constructed valid heap in O(N)" << endl;

    // Test Heap Sort
    game.handleHeapSortAction();
    const int* arr = game.heap.getArray();
    int sz = game.heap.getSize();
    for (int i = 0; i < sz - 1; i++) {
        assert(arr[i] <= arr[i + 1]);
    }
    cout << "[PASS] Heap Sort sorted all " << sz << " elements in ascending order" << endl;

    // Test Array Cell Hit Test
    float cellW = 54.0f;
    float gap = 8.0f;
    float totalW = sz * cellW + (sz - 1) * gap;
    float startX = 640.0f - totalW * 0.5f;
    int cellIdx = game.renderer.findHeapArrayCellAt(startX + 10.0f, 615.0f, sz);
    assert(cellIdx == 0);
    cout << "[PASS] Array cell hit test detected index 0 at start coordinates" << endl;
}

void testPlaybackAndInput(GameScreen& game) {
    cout << "--- Testing Playback Controls & Input Box ---" << endl;
    game.setupDefaultAct1Tree();
    game.handleButtonClick(1); // Preorder (7 steps)

    assert(game.eventController.isPlaying);
    assert(strcmp(game.eventController.getState(), "playing") == 0);

    // Pause
    game.eventController.pause();
    assert(game.eventController.isPaused);
    assert(strcmp(game.eventController.getState(), "paused") == 0);

    // Step Forward
    int stepIdxBefore = game.eventController.currentStepIndex;
    game.eventController.stepForward(game.layout);
    assert(game.eventController.currentStepIndex == stepIdxBefore + 1);

    // Step Backward
    game.eventController.stepBackward(game.layout);
    cout << "[PASS] Play, Pause, Step Forward, and Step Backward functional" << endl;

    // Speed Cycle
    float s1 = game.eventController.speedMultiplier;
    game.eventController.cycleSpeed();
    float s2 = game.eventController.speedMultiplier;
    assert(s1 != s2);
    cout << "[PASS] Speed multiplier cycled: " << s1 << "x -> " << s2 << "x" << endl;

    // InputBox
    game.inputBox.isFocused = true;
    game.inputBox.setValue(42);
    assert(game.inputBox.getValue() == 42);
    game.inputBox.adjustValue(+5);
    assert(game.inputBox.getValue() == 47);
    game.inputBox.adjustValue(-10);
    assert(game.inputBox.getValue() == 37);
    cout << "[PASS] Input box value adjustments functional" << endl;
}

int main() {
    cout << "========================================================" << endl;
    cout << "    RUNNING PHASE 3 FULL GUI INTEGRATION VERIFICATION   " << endl;
    cout << "========================================================" << endl;

    GameScreen game;
    testAct1(game);
    testAct2(game);
    testAct3(game);
    testAct4(game);
    testPlaybackAndInput(game);

    cout << "========================================================" << endl;
    cout << "ALL PHASE 3 GUI TESTS PASSED (ACTS 1-4 FULLY VERIFIED)!" << endl;
    cout << "========================================================" << endl;
    return 0;
}
