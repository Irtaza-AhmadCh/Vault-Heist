#include <iostream>
#include <cassert>
#include <cstring>
#include "../game/view/GameScreen.h"

void testSandboxBasics() {
    std::cout << "--- Testing Sandbox Navigation & Structure Switching ---" << std::endl;
    GameScreen screen;

    // Default screen is menu
    assert(screen.isScreen("menu"));

    // Enter sandbox (actionId 211)
    screen.handleButtonClick(211);
    assert(screen.isScreen("sandbox"));
    assert(screen.isAct("act_1"));
    assert(screen.binTree.root != nullptr);
    assert(screen.binTree.root->data == 50);

    // Switch to BST (actionId 102)
    screen.handleButtonClick(102);
    assert(screen.isAct("act_2"));
    assert(!screen.bst.isEmpty());
    assert(screen.bst.getRoot()->key == 50);

    // Switch to AVL (actionId 103)
    screen.handleButtonClick(103);
    assert(screen.isAct("act_3"));
    assert(!screen.avl.isEmpty());
    assert(screen.avl.isBalanced());

    // Switch to Heap (actionId 104)
    screen.handleButtonClick(104);
    assert(screen.isAct("act_4"));
    assert(!screen.heap.isEmpty());
    assert(screen.heap.isMaxHeap());

    std::cout << "Sandbox structure switching verified!" << std::endl;
}

void testSandboxAlarmIsolation() {
    std::cout << "--- Testing Sandbox Alarm Isolation ---" << std::endl;
    GameScreen screen;
    screen.handleButtonClick(211); // Enter sandbox

    int startAlarm = screen.levelManager.scoreSystem.alarmSteps;

    // Perform operations in sandbox (BST insert, delete, search, range)
    screen.handleButtonClick(102); // Switch to BST
    screen.inputBox.setValue(55);
    screen.handleButtonClick(10); // Insert 55
    assert(screen.bst.search(55));

    screen.inputBox.setValue(55);
    screen.handleButtonClick(11); // Search 55

    screen.inputBox.setValue(55);
    screen.handleButtonClick(12); // Delete 55
    assert(!screen.bst.search(55));

    // Traversals
    screen.handleButtonClick(1); // Preorder
    screen.handleButtonClick(2); // Inorder
    screen.handleButtonClick(3); // Postorder
    screen.handleButtonClick(4); // Levelorder

    // Switch to AVL and test operations
    screen.handleButtonClick(103);
    screen.inputBox.setValue(65);
    screen.handleButtonClick(10); // AVL Insert
    assert(screen.avl.search(65));
    screen.handleButtonClick(15); // k-th
    screen.handleButtonClick(16); // Rank

    // Switch to Heap and test operations
    screen.handleButtonClick(104);
    screen.inputBox.setValue(99);
    screen.handleButtonClick(10); // Heap Insert
    screen.handleButtonClick(43); // Peek Max
    screen.handleButtonClick(44); // Check Heap
    screen.handleButtonClick(40); // Extract Max

    // Verify that alarm steps did NOT increase during sandbox mode
    assert(screen.levelManager.scoreSystem.alarmSteps == startAlarm);
    std::cout << "Sandbox alarm isolation verified! Alarm steps remained 0." << std::endl;
}

void testSandboxRandomAndReset() {
    std::cout << "--- Testing Sandbox Random & Reset ---" << std::endl;
    GameScreen screen;
    screen.handleButtonClick(211); // Enter sandbox

    // 1. Act 1: Binary Tree
    screen.handleButtonClick(101);
    screen.handleButtonClick(221); // Random
    assert(screen.binTree.root != nullptr);
    screen.handleButtonClick(220); // Reset
    assert(screen.binTree.root == nullptr);

    // 2. Act 2: BST
    screen.handleButtonClick(102);
    screen.handleButtonClick(221); // Random
    assert(!screen.bst.isEmpty());
    screen.handleButtonClick(220); // Reset
    assert(screen.bst.isEmpty());

    // 3. Act 3: AVL
    screen.handleButtonClick(103);
    screen.handleButtonClick(221); // Random
    assert(!screen.avl.isEmpty());
    assert(screen.avl.isBalanced());
    screen.handleButtonClick(220); // Reset
    assert(screen.avl.isEmpty());

    // 4. Act 4: Heap
    screen.handleButtonClick(104);
    screen.handleButtonClick(221); // Random
    assert(!screen.heap.isEmpty());
    assert(screen.heap.isMaxHeap());
    screen.handleButtonClick(220); // Reset
    assert(screen.heap.isEmpty());

    std::cout << "Sandbox Random and Reset verified across all 4 structures!" << std::endl;
}

int main() {
    testSandboxBasics();
    testSandboxAlarmIsolation();
    testSandboxRandomAndReset();
    std::cout << "\nALL SANDBOX TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
