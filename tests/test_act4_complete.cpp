#include <iostream>
#include <cassert>
#include "../game/logic/heap/max_heap.h"
#include "../game/scoring/ScoreSystem.h"
#include "../game/controller/act3-4/Act4State.h"

void testLevel41() {
    std::cout << "--- Testing Level 4.1 ---" << std::endl;
    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;
    char fb[256];

    act4.initLevel(1, &heap, score);
    assert(score.optimalSteps == 3);
    assert(score.alarmCapacity == 7);

    // Parent 1 (value 10): no swap needed
    bool ok = act4.handleLevel41NoSwapNeeded(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl41.currentParentIndex == 0);

    // Parent 0 (value 4): select child 10 (idx 1), confirm swap
    ok = act4.handleLevel41NodeClick(1, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel41ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);

    // Position 1 (value 4): select child 5 (idx 3), confirm swap
    ok = act4.handleLevel41NodeClick(3, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel41ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);

    assert(act4.lvl41.isLevelComplete);
    assert(heap.isMaxHeap());
    assert(score.alarmSteps == 3);
    std::cout << "Level 4.1 Passed! (Optimal: 3 steps, Alarm: 3/7)" << std::endl;
}

void testLevel42() {
    std::cout << "--- Testing Level 4.2 ---" << std::endl;
    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;
    char fb[256];

    act4.initLevel(2, &heap, score);
    assert(score.optimalSteps == 4);
    assert(score.alarmCapacity == 8);
    assert(act4.lvl42.round == 1);
    assert(heap.getSize() == 5);

    // Round A: Sift-Down
    // Root 4 has children 10 (idx 1) and 3 (idx 2).
    // Test selecting smaller child (idx 2): should fail (+1 alarm)
    bool ok = act4.handleLevel42NodeClick(2, score, fb, sizeof(fb));
    assert(!ok);
    assert(score.alarmSteps == 1);

    // Select larger child 10 (idx 1)
    ok = act4.handleLevel42NodeClick(1, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel42ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    assert(heap.get(0) == 10);
    assert(heap.get(1) == 4);

    // Now value 4 is at idx 1. Children are 5 (idx 3) and 1 (idx 4).
    ok = act4.handleLevel42NodeClick(3, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel42ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    assert(heap.get(1) == 5);
    assert(heap.get(3) == 4);

    // Round A complete! Automatically transitioned to Round B
    assert(act4.lvl42.round == 2);
    assert(heap.getSize() == 6);
    assert(heap.get(5) == 12);
    assert(act4.lvl42.currentUpChildIndex == 5);
    assert(act4.lvl42.currentUpParentIndex == 2); // Parent is idx 2 (value 3)

    // Round B: Sift-Up
    // Step 1: swap 12 (idx 5) with parent 3 (idx 2)
    ok = act4.handleLevel42MoveUp(score, fb, sizeof(fb));
    assert(ok);
    assert(heap.get(2) == 12);
    assert(heap.get(5) == 3);
    assert(act4.lvl42.currentUpChildIndex == 2);
    assert(act4.lvl42.currentUpParentIndex == 0); // Next parent is root 10

    // Step 2: swap 12 (idx 2) with root 10 (idx 0)
    ok = act4.handleLevel42MoveUp(score, fb, sizeof(fb));
    assert(ok);
    assert(heap.get(0) == 12);
    assert(heap.get(2) == 10);
    assert(act4.lvl42.isLevelComplete);
    assert(heap.isMaxHeap());

    // Final array check: [12, 5, 10, 4, 1, 3]
    const int* arr = heap.getArray();
    assert(arr[0] == 12 && arr[1] == 5 && arr[2] == 10 && arr[3] == 4 && arr[4] == 1 && arr[5] == 3);
    std::cout << "Level 4.2 Passed! Final array: [12, 5, 10, 4, 1, 3]" << std::endl;
}

void testLevel43() {
    std::cout << "--- Testing Level 4.3 ---" << std::endl;
    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;
    char fb[256];

    act4.initLevel(3, &heap, score);
    assert(score.optimalSteps == 8);
    assert(score.alarmCapacity == 14);
    assert(act4.lvl43.activeHeapSize == 5);

    // Extraction 1: root is 10. Extract max!
    bool ok = act4.handleLevel43ExtractMax(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl43.activeHeapSize == 4);
    assert(heap.get(4) == 10); // 10 moved to sorted position at index 4
    assert(act4.lvl43.subPhase == 1); // Needs sift down

    // Sift down: root 1 has children 5 (idx 1) and 3 (idx 2). Select child 5 (idx 1).
    ok = act4.handleLevel43NodeClick(1, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel43ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    // At index 1, child is 4 (idx 3). Select child 4.
    ok = act4.handleLevel43NodeClick(3, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel43ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl43.subPhase == 0); // Sift-down complete

    // Extraction 2: root is 5. Extract max!
    ok = act4.handleLevel43ExtractMax(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl43.activeHeapSize == 3);
    assert(heap.get(3) == 5);

    // Sift down: root 1 has children 4 (idx 1) and 3 (idx 2). Select child 4 (idx 1).
    ok = act4.handleLevel43NodeClick(1, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel43ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl43.subPhase == 0);

    // Extraction 3: root is 4. Extract max!
    ok = act4.handleLevel43ExtractMax(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl43.activeHeapSize == 2);
    assert(heap.get(2) == 4);
    // 3 has child 1, 3 >= 1, no sift down needed!
    assert(act4.lvl43.subPhase == 0);

    // Extraction 4: root is 3. Extract max!
    ok = act4.handleLevel43ExtractMax(score, fb, sizeof(fb));
    assert(ok);
    assert(act4.lvl43.activeHeapSize == 1);
    assert(act4.lvl43.isLevelComplete);

    // Verify ascending order: [1, 3, 4, 5, 10]
    const int* arr = heap.getArray();
    assert(arr[0] == 1 && arr[1] == 3 && arr[2] == 4 && arr[3] == 5 && arr[4] == 10);
    assert(heap.isSortedAscending(5));
    std::cout << "Level 4.3 Passed! Final sorted array: [1, 3, 4, 5, 10]" << std::endl;
}

void testLevel44() {
    std::cout << "--- Testing Level 4.4 ---" << std::endl;
    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;
    char fb[256];

    act4.initLevel(4, &heap, score);
    assert(score.optimalSteps == 7);
    assert(score.alarmCapacity == 13);
    assert(act4.lvl44.activeHeapSize == 6);
    assert(act4.lvl44.extractionRoundTarget == 4);

    // Perform extractions until 4 are complete (including finishing any pending sift-down)
    while (!act4.lvl44.isLevelComplete) {
        bool needMore = (act4.lvl44.completedExtractions < 4) || (act4.lvl44.subPhase == 1);
        if (!needMore) break;
        if (act4.lvl44.subPhase == 0) {
            bool ok = act4.handleLevel44ExtractMax(score, fb, sizeof(fb));
            assert(ok);
        } else if (act4.lvl44.subPhase == 1) {
            int pIdx = act4.lvl44.currentSiftIndex;
            int left = 2 * pIdx + 1;
            int right = 2 * pIdx + 2;
            int n = act4.lvl44.activeHeapSize;
            // If no valid children, shouldn't be in subPhase 1 — safety break
            if (left >= n) break;
            int largest = left;
            if (right < n && heap.get(right) > heap.get(left)) {
                largest = right;
            }
            bool ok = act4.handleLevel44NodeClick(largest, score, fb, sizeof(fb));
            assert(ok);
            ok = act4.handleLevel44ConfirmSwap(score, fb, sizeof(fb));
            assert(ok);
        }
    }

    assert(act4.lvl44.isLevelComplete);
    assert(act4.lvl44.completedExtractions == 4);
    std::cout << "Level 4.4 Passed! Completed 4 extractions within alarm capacity." << std::endl;
}

void testLevel45() {
    std::cout << "--- Testing Level 4.5 ---" << std::endl;
    MaxHeap heap;
    ScoreSystem score;
    Act4Controller act4;
    char fb[256];

    act4.initLevel(5, &heap, score);
    assert(score.optimalSteps == 12);
    assert(score.alarmCapacity == 20);
    assert(strcmp(act4.lvl45.phase, "build") == 0);

    // Phase A: Build Max Heap
    // Initial: [12, 3, 17, 8, 15, 6, 1]
    // Parent 2 (val 17): children 6 and 1. No swap needed!
    assert(act4.lvl45.currentParentIndex == 2);
    bool ok = act4.handleLevel45NoSwapNeeded(score, fb, sizeof(fb));
    assert(ok);

    // Parent 1 (val 3): children 8 (idx 3) and 15 (idx 4). Select 15 (idx 4).
    assert(act4.lvl45.currentParentIndex == 1);
    ok = act4.handleLevel45NodeClick(4, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel45ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);

    // Parent 0 (val 12): children 15 (idx 1) and 17 (idx 2). Larger is 17 (idx 2).
    assert(act4.lvl45.currentParentIndex == 0);
    ok = act4.handleLevel45NodeClick(2, score, fb, sizeof(fb));
    assert(ok);
    ok = act4.handleLevel45ConfirmSwap(score, fb, sizeof(fb));
    assert(ok);

    // After swap, val 12 is at idx 2. Children are 6 (idx 5) and 1 (idx 6). 12 >= 6, 1, sift done!
    assert(strcmp(act4.lvl45.phase, "sort") == 0);
    assert(heap.isMaxHeap());
    std::cout << "Phase A (Build Heap) Passed! Valid Max Heap created." << std::endl;

    // Phase B: Heap Sort extractions
    while (strcmp(act4.lvl45.phase, "sort") == 0 && !act4.lvl45.isLevelComplete) {
        if (act4.lvl45.subPhase == 0) {
            ok = act4.handleLevel45ExtractMax(score, fb, sizeof(fb));
            assert(ok);
        } else if (act4.lvl45.subPhase == 1) {
            int pIdx = act4.lvl45.currentSiftIndex;
            int left = 2 * pIdx + 1;
            int right = 2 * pIdx + 2;
            int n = act4.lvl45.activeHeapSize;
            if (left >= n) break; // No children, shouldn't be in subPhase 1
            int largest = left;
            if (right < n && heap.get(right) > heap.get(left)) {
                largest = right;
            }
            ok = act4.handleLevel45NodeClick(largest, score, fb, sizeof(fb));
            assert(ok);
            ok = act4.handleLevel45ConfirmSwap(score, fb, sizeof(fb));
            assert(ok);
        }
    }

    assert(act4.lvl45.isLevelComplete);
    assert(heap.isSortedAscending(7));
    const int* arr = heap.getArray();
    assert(arr[0] == 1 && arr[1] == 3 && arr[2] == 6 && arr[3] == 8 && arr[4] == 12 && arr[5] == 15 && arr[6] == 17);
    std::cout << "Phase B & C Passed! Final sorted array: [1, 3, 6, 8, 12, 15, 17]" << std::endl;
    std::cout << "Level 4.5 Passed! Boss conquered!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "STARTING COMPLETE ACT 4 TEST SUITE" << std::endl;
    std::cout << "========================================" << std::endl;

    testLevel41();
    testLevel42();
    testLevel43();
    testLevel44();
    testLevel45();

    std::cout << "========================================" << std::endl;
    std::cout << "ALL ACT 4 LEVELS (4.1 - 4.5) PASSED!" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
