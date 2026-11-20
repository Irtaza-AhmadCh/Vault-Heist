#include <iostream>
#include <cstring>
#include "../../game/logic/event.h"
#include "../../game/logic/heap/max_heap.h"

using namespace std;

int testsPassed = 0;
int testsTotal = 0;

void assertCondition(bool condition, const char* testName) {
    testsTotal++;
    if (condition) {
        testsPassed++;
        cout << "[PASS] " << testName << endl;
    } else {
        cout << "[FAIL] " << testName << endl;
    }
}

void testArrayRepresentation() {
    cout << "--- Testing MaxHeap Array Representation ---" << endl;
    MaxHeap heap(4);
    assertCondition(heap.getSize() == 0, "Initial size is 0");
    assertCondition(heap.isEmpty() == true, "Heap is initially empty");
    assertCondition(heap.getCapacity() >= 4, "Initial capacity is at least 4");

    heap.insert(10);
    heap.insert(20);
    heap.insert(5);
    heap.insert(30);
    heap.insert(15);

    assertCondition(heap.getSize() == 5, "Size is 5 after 5 insertions");
    assertCondition(!heap.isEmpty(), "Heap is not empty after insertions");
    assertCondition(heap.getCapacity() >= 8, "Heap resized automatically when full");
    assertCondition(heap.peekMax() == 30, "Max element at root is 30");

    assertCondition(heap.parent(1) == 0, "parent(1) is 0");
    assertCondition(heap.parent(2) == 0, "parent(2) is 0");
    assertCondition(heap.leftChild(0) == 1, "leftChild(0) is 1");
    assertCondition(heap.rightChild(0) == 2, "rightChild(0) is 2");
}

void testBuildMaxHeap() {
    cout << "--- Testing buildMaxHeap ---" << endl;
    int data[] = {12, 11, 13, 5, 6, 7, 25, 1, 99, 44};
    int n = sizeof(data) / sizeof(data[0]);

    MaxHeap heap(data, n);
    assertCondition(heap.getSize() == n, "Heap size matches initial array size");
    assertCondition(heap.isMaxHeap(), "Array satisfies MaxHeap property after buildMaxHeap");
    assertCondition(heap.peekMax() == 99, "Root is the maximum element (99)");

    int duplicates[] = {5, 5, 5, 5, 5, 5};
    MaxHeap dupHeap(duplicates, 6);
    assertCondition(dupHeap.isMaxHeap(), "Array with all duplicates is a valid MaxHeap");

    int negatives[] = {-10, -2, -50, -1, -5};
    MaxHeap negHeap(negatives, 5);
    assertCondition(negHeap.isMaxHeap(), "Array with negative numbers is a valid MaxHeap");
    assertCondition(negHeap.peekMax() == -1, "Maximum of negative array is -1");
}

void testSiftUpAndSiftDown() {
    cout << "--- Testing siftUp and siftDown ---" << endl;
    MaxHeap heap(16);
    heap.insert(40);
    heap.insert(20);
    heap.insert(30);
    heap.insert(10);
    heap.insert(15);
    assertCondition(heap.isMaxHeap(), "Heap is valid after successive siftUp calls");

    heap.insert(100);
    assertCondition(heap.peekMax() == 100, "100 sifted up to root");
    assertCondition(heap.isMaxHeap(), "Heap property maintained after siftUp of max value");

    int rawData[] = {10, 80, 70, 40, 50, 60, 30};
    int n = 7;
    MaxHeap manualHeap(rawData, n);
    assertCondition(manualHeap.isMaxHeap(), "Manual heap is valid");
}

void testExtractMax() {
    cout << "--- Testing extractMax ---" << endl;
    int data[] = {45, 20, 14, 12, 31, 7, 11, 13, 90, 80};
    int n = sizeof(data) / sizeof(data[0]);

    MaxHeap heap(data, n);
    int prev = heap.extractMax();
    bool correctlyOrdered = true;
    bool alwaysMaxHeap = true;

    while (!heap.isEmpty()) {
        if (!heap.isMaxHeap()) {
            alwaysMaxHeap = false;
        }
        int curr = heap.extractMax();
        if (curr > prev) {
            correctlyOrdered = false;
        }
        prev = curr;
    }

    assertCondition(correctlyOrdered, "extractMax returns values in non-increasing order");
    assertCondition(alwaysMaxHeap, "MaxHeap property maintained after each extraction");
    assertCondition(heap.isEmpty(), "Heap is empty after extracting all elements");
    assertCondition(heap.extractMax() == -1, "extractMax on empty heap returns -1 safely");
}

void testHeapSort() {
    cout << "--- Testing Heap Sort ---" << endl;
    int arr[] = {64, 34, 25, 12, 22, 11, 90, -5, 0, 100, 22};
    int n = sizeof(arr) / sizeof(arr[0]);

    MaxHeap::heapSort(arr, n);

    bool isSorted = true;
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            isSorted = false;
            break;
        }
    }
    assertCondition(isSorted, "Static heapSort sorts array in ascending order");
    assertCondition(arr[0] == -5, "First element after sort is minimum (-5)");
    assertCondition(arr[n - 1] == 100, "Last element after sort is maximum (100)");

    int single[] = {42};
    MaxHeap::heapSort(single, 1);
    assertCondition(single[0] == 42, "Single element array sorts correctly");

    MaxHeap internalHeap;
    internalHeap.insert(15);
    internalHeap.insert(3);
    internalHeap.insert(8);
    internalHeap.insert(20);
    internalHeap.insert(1);
    internalHeap.heapSort();

    const int* sortedArray = internalHeap.getArray();
    bool internalSorted = true;
    for (int i = 0; i < internalHeap.getSize() - 1; i++) {
        if (sortedArray[i] > sortedArray[i + 1]) {
            internalSorted = false;
            break;
        }
    }
    assertCondition(internalSorted, "Internal heapSort sorts internal array in ascending order");
}

void testIsMaxHeapValidation() {
    cout << "--- Testing isMaxHeap Validation ---" << endl;
    int validHeap[] = {90, 80, 70, 40, 50, 60, 30, 10, 20};
    int nValid = sizeof(validHeap) / sizeof(validHeap[0]);
    assertCondition(MaxHeap::isMaxHeap(validHeap, nValid) == true, "Valid heap returns true");

    int invalidHeap[] = {90, 40, 70, 80, 50, 60, 30};
    int nInvalid = sizeof(invalidHeap) / sizeof(invalidHeap[0]);
    assertCondition(MaxHeap::isMaxHeap(invalidHeap, nInvalid) == false, "Invalid heap (child > parent) returns false");

    assertCondition(MaxHeap::isMaxHeap(nullptr, 0) == true, "Null array returns true");
    int singleElem[] = {100};
    assertCondition(MaxHeap::isMaxHeap(singleElem, 1) == true, "Single element array returns true");
}

void testEventLogging() {
    cout << "--- Testing Event Logging with String Literals ---" << endl;
    EventLog log;
    MaxHeap heap(8, &log);

    heap.insert(50);
    heap.insert(30);
    heap.insert(70);

    assertCondition(log.getCount() > 0, "Events are generated during heap insert");

    bool hasHeapSwap = false;
    for (int i = 0; i < log.getCount(); i++) {
        if (strcmp(log.getEvent(i).type, EVENT_HEAP_SWAP) == 0) {
            hasHeapSwap = true;
            break;
        }
    }
    assertCondition(hasHeapSwap, "Heap swap event with snake_case string literal recorded");

    int eventCountBefore = log.getCount();
    heap.extractMax();
    assertCondition(log.getCount() > eventCountBefore, "Events are generated during extractMax");

    log.enabled = false;
    int countWithLoggingOff = log.getCount();
    heap.insert(25);
    assertCondition(log.getCount() == countWithLoggingOff, "No events generated when logging is disabled");
}

int main() {
    cout << "========================================" << endl;
    cout << "   RUNNING MEMBER B MAXHEAP UNIT TESTS  " << endl;
    cout << "========================================" << endl;

    testArrayRepresentation();
    testBuildMaxHeap();
    testSiftUpAndSiftDown();
    testExtractMax();
    testHeapSort();
    testIsMaxHeapValidation();
    testEventLogging();

    cout << "========================================" << endl;
    cout << "Results: " << testsPassed << "/" << testsTotal << " tests passed." << endl;
    cout << "========================================" << endl;

    return (testsPassed == testsTotal) ? 0 : 1;
}
