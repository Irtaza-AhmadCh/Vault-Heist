#include <iostream>
#include <cassert>
#include "../game/logic/heap/max_heap.h"
#include "../game/scoring/ScoreSystem.h"
#include "../game/controller/act3-4/Act4State.h"

int main() {
    std::cout << "Running Act 4 Level 4.1 Acceptance Tests..." << std::endl;

    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;

    char fb[256];

    // Initialize level 4.1
    act4.initLevel(1, &heap, score);
    assert(act4.lvl41.currentParentIndex == 1);
    assert(act4.lvl41.currentSiftIndex == 1);
    assert(heap.get(1) == 10);
    assert(score.optimalSteps == 3);
    assert(score.alarmCapacity == 7);
    assert(score.alarmSteps == 0);

    // Test 1: Selecting smaller child (index 4, value 1) when child 3 (value 5) is larger
    bool ok = act4.handleLevel41NodeClick(4, score, fb, sizeof(fb));
    assert(!ok);
    assert(score.alarmSteps == 1); // +1 alarm
    std::cout << "Test 1 Passed: Smaller child rejected (+1 alarm)" << std::endl;

    // Test 2: Selecting non-child (index 0 or 2)
    ok = act4.handleLevel41NodeClick(2, score, fb, sizeof(fb));
    assert(!ok);
    assert(score.alarmSteps == 2); // +1 alarm
    std::cout << "Test 2 Passed: Non-child rejected (+1 alarm)" << std::endl;

    // Test 3: Parent 1 satisfies Max Heap (10 >= 5 and 1) -> No Swap Needed
    ok = act4.handleLevel41NoSwapNeeded(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl41.currentParentIndex == 0);
    assert(act4.lvl41.currentSiftIndex == 0);
    std::cout << "Test 3 Passed: No Swap Needed accepted on parent 1" << std::endl;

    // Test 4: At parent 0 (value 4), clicking No Swap Needed when 10 > 4 -> rejected
    int prevAlarm = score.alarmSteps;
    ok = act4.handleLevel41NoSwapNeeded(score, fb, sizeof(fb));
    assert(!ok);
    assert(score.alarmSteps == prevAlarm + 1);
    std::cout << "Test 4 Passed: No Swap Needed rejected when child is larger" << std::endl;

    // Test 5: At parent 0 (value 4), select larger child 10 (index 1)
    ok = act4.handleLevel41NodeClick(1, score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl41.selectedChildIndex == 1);
    std::cout << "Test 5 Passed: Child 10 selected" << std::endl;

    // Test 6: Confirm swap
    ok = act4.handleLevel41ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    assert(heap.get(0) == 10);
    assert(heap.get(1) == 4);
    assert(act4.lvl41.currentSiftIndex == 1);
    std::cout << "Test 6 Passed: Root swap executed, sift-down at index 1" << std::endl;

    // Test 7: At index 1 (value 4), children are 5 (idx 3) and 1 (idx 4). Select 5 (idx 3).
    ok = act4.handleLevel41NodeClick(3, score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl41.selectedChildIndex == 3);
    std::cout << "Test 7 Passed: Child 5 selected" << std::endl;

    // Test 8: Confirm swap -> 4 moves to index 3 (leaf) -> construction complete!
    ok = act4.handleLevel41ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    assert(heap.get(1) == 5);
    assert(heap.get(3) == 4);
    assert(act4.lvl41.isLevelComplete);
    assert(heap.isMaxHeap());
    std::cout << "Test 8 Passed: Sift-down complete, Max Heap validated!" << std::endl;

    // Verify final array
    const int* arr = heap.getArray();
    assert(arr[0] == 10 && arr[1] == 5 && arr[2] == 3 && arr[3] == 4 && arr[4] == 1);
    std::cout << "Final array: [10, 5, 3, 4, 1] - Perfectly matches spec!" << std::endl;

    std::cout << "ALL LEVEL 4.1 TESTS PASSED!" << std::endl;
    return 0;
}
