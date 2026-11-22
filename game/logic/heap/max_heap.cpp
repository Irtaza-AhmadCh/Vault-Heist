#include "max_heap.h"
#include <iostream>

using namespace std;

HeapElement::HeapElement() {
    value = 0;
    originalIndex = -1;
}

HeapElement::HeapElement(int val, int origIdx) {
    value = val;
    originalIndex = origIdx;
}

void MaxHeap::swapElements(int indexA, int indexB) {
    if (indexA == indexB) {
        return;
    }
    int temp = heapArray[indexA];
    heapArray[indexA] = heapArray[indexB];
    heapArray[indexB] = temp;

    if (eventLog != nullptr) {
        eventLog->logHeapSwap(indexA, indexB, heapArray[indexB], heapArray[indexA]);
    }
}

void MaxHeap::resize(int newCapacity) {
    if (newCapacity <= capacity) {
        return;
    }
    int* newArray = new int[newCapacity];
    for (int i = 0; i < currentSize; i++) {
        newArray[i] = heapArray[i];
    }
    delete[] heapArray;
    heapArray = newArray;
    capacity = newCapacity;
}

MaxHeap::MaxHeap(int initialCapacity, EventLog* log) {
    if (initialCapacity < 4) {
        initialCapacity = 4;
    }
    capacity = initialCapacity;
    currentSize = 0;
    eventLog = log;
    heapArray = new int[capacity];
}

MaxHeap::MaxHeap(const int* arr, int n, EventLog* log) {
    capacity = (n > 16) ? (n * 2) : 32;
    currentSize = n;
    eventLog = log;
    heapArray = new int[capacity];
    for (int i = 0; i < n; i++) {
        heapArray[i] = arr[i];
    }
    buildMaxHeap();
}

MaxHeap::MaxHeap(const MaxHeap& other) {
    capacity = other.capacity;
    currentSize = other.currentSize;
    eventLog = other.eventLog;
    heapArray = new int[capacity];
    for (int i = 0; i < currentSize; i++) {
        heapArray[i] = other.heapArray[i];
    }
}

MaxHeap::~MaxHeap() {
    if (heapArray != nullptr) {
        delete[] heapArray;
        heapArray = nullptr;
    }
    currentSize = 0;
    capacity = 0;
}

void MaxHeap::setEventLog(EventLog* log) {
    eventLog = log;
}

EventLog* MaxHeap::getEventLog() const {
    return eventLog;
}

int MaxHeap::parent(int i) const {
    return (i - 1) / 2;
}

int MaxHeap::leftChild(int i) const {
    return 2 * i + 1;
}

int MaxHeap::rightChild(int i) const {
    return 2 * i + 2;
}

bool MaxHeap::hasLeftChild(int i, int heapSize) const {
    return (2 * i + 1) < heapSize;
}

bool MaxHeap::hasRightChild(int i, int heapSize) const {
    return (2 * i + 2) < heapSize;
}

int MaxHeap::getSize() const {
    return currentSize;
}

int MaxHeap::getCapacity() const {
    return capacity;
}

bool MaxHeap::isEmpty() const {
    return currentSize == 0;
}

int MaxHeap::get(int index) const {
    if (index >= 0 && index < currentSize) {
        return heapArray[index];
    }
    return -1;
}

int MaxHeap::peekMax() const {
    if (currentSize <= 0) {
        return -1;
    }
    return heapArray[0];
}

const int* MaxHeap::getArray() const {
    return heapArray;
}

void MaxHeap::siftUp(int index) {
    if (index <= 0 || index >= currentSize) {
        return;
    }
    while (index > 0) {
        int p = parent(index);
        if (eventLog != nullptr) {
            eventLog->logCompare(index, p, heapArray[index], heapArray[p]);
        }
        if (heapArray[index] > heapArray[p]) {
            swapElements(index, p);
            index = p;
        } else {
            break;
        }
    }
}

void MaxHeap::siftDown(int index, int heapSize) {
    if (index < 0 || index >= heapSize) {
        return;
    }
    if (eventLog != nullptr) {
        eventLog->logHeapifyStart(index, heapSize);
    }

    while (hasLeftChild(index, heapSize)) {
        int largest = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < heapSize) {
            if (eventLog != nullptr) {
                eventLog->logCompare(left, largest, heapArray[left], heapArray[largest]);
            }
            if (heapArray[left] > heapArray[largest]) {
                largest = left;
            }
        }

        if (right < heapSize) {
            if (eventLog != nullptr) {
                eventLog->logCompare(right, largest, heapArray[right], heapArray[largest]);
            }
            if (heapArray[right] > heapArray[largest]) {
                largest = right;
            }
        }

        if (largest != index) {
            swapElements(index, largest);
            index = largest;
        } else {
            break;
        }
    }

    if (eventLog != nullptr) {
        eventLog->logHeapifyDone(index, heapSize);
    }
}

void MaxHeap::siftDown(int index) {
    siftDown(index, currentSize);
}

void MaxHeap::insert(int value) {
    if (currentSize >= capacity) {
        resize(capacity * 2);
    }
    int insertIndex = currentSize;
    heapArray[insertIndex] = value;
    currentSize++;

    if (eventLog != nullptr) {
        eventLog->logHeapSizeChange(currentSize - 1, currentSize);
    }

    siftUp(insertIndex);
}

void MaxHeap::buildMaxHeap() {
    if (currentSize <= 1) {
        return;
    }
    if (eventLog != nullptr) {
        eventLog->logHeapBuild(currentSize, heapArray);
    }
    for (int i = (currentSize / 2) - 1; i >= 0; i--) {
        siftDown(i, currentSize);
    }
}

void MaxHeap::loadRawArray(const int* arr, int n) {
    if (n > capacity) {
        resize((n > 16) ? (n * 2) : 32);
    }
    currentSize = n;
    for (int i = 0; i < n; i++) {
        heapArray[i] = arr[i];
    }
}

void MaxHeap::setSize(int s) {
    if (s >= 0 && s <= capacity) {
        currentSize = s;
    }
}

void MaxHeap::appendRaw(int value) {
    if (currentSize >= capacity) {
        resize(capacity * 2);
    }
    heapArray[currentSize++] = value;
}

bool MaxHeap::isSortedAscending(int n) const {
    if (n <= 1) return true;
    for (int i = 0; i < n - 1; i++) {
        if (heapArray[i] > heapArray[i + 1]) return false;
    }
    return true;
}

void MaxHeap::buildMaxHeap(const int* arr, int n) {
    if (n > capacity) {
        resize(n * 2);
    }
    currentSize = n;
    for (int i = 0; i < n; i++) {
        heapArray[i] = arr[i];
    }
    buildMaxHeap();
}

int MaxHeap::extractMax() {
    if (currentSize <= 0) {
        cout << "Heap underflow: empty heap" << endl;
        return -1;
    }

    int maxVal = heapArray[0];
    if (eventLog != nullptr) {
        eventLog->logExtractMax(maxVal, 0, currentSize - 1);
    }

    if (currentSize == 1) {
        currentSize = 0;
        if (eventLog != nullptr) {
            eventLog->logHeapSizeChange(1, 0);
        }
        return maxVal;
    }

    heapArray[0] = heapArray[currentSize - 1];
    currentSize--;

    if (eventLog != nullptr) {
        eventLog->logHeapSizeChange(currentSize + 1, currentSize);
    }

    siftDown(0, currentSize);
    return maxVal;
}

void MaxHeap::heapSort() {
    if (currentSize <= 1) {
        return;
    }

    buildMaxHeap();
    int totalElements = currentSize;

    for (int i = totalElements - 1; i > 0; i--) {
        swapElements(0, i);
        if (eventLog != nullptr) {
            eventLog->logMarkSorted(i, heapArray[i]);
        }
        siftDown(0, i);
    }

    if (eventLog != nullptr) {
        eventLog->logMarkSorted(0, heapArray[0]);
        eventLog->logHeapSortComplete(totalElements, heapArray);
    }
}

bool MaxHeap::isMaxHeap() const {
    return isMaxHeap(heapArray, currentSize);
}

bool MaxHeap::isMaxHeap(const int* arr, int n) {
    if (arr == nullptr || n <= 1) {
        return true;
    }
    for (int i = 0; i <= (n - 2) / 2; i++) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && arr[i] < arr[left]) {
            return false;
        }
        if (right < n && arr[i] < arr[right]) {
            return false;
        }
    }
    return true;
}

void MaxHeap::heapSort(int* arr, int n, EventLog* log) {
    if (arr == nullptr || n <= 1) {
        return;
    }

    if (log != nullptr) {
        log->logHeapBuild(n);
    }

    for (int i = (n / 2) - 1; i >= 0; i--) {
        int curr = i;
        while (2 * curr + 1 < n) {
            int largest = curr;
            int left = 2 * curr + 1;
            int right = 2 * curr + 2;

            if (left < n && arr[left] > arr[largest]) {
                largest = left;
            }
            if (right < n && arr[right] > arr[largest]) {
                largest = right;
            }

            if (largest != curr) {
                int temp = arr[curr];
                arr[curr] = arr[largest];
                arr[largest] = temp;
                if (log != nullptr) {
                    log->logHeapSwap(curr, largest, arr[largest], arr[curr]);
                }
                curr = largest;
            } else {
                break;
            }
        }
    }

    for (int i = n - 1; i > 0; i--) {
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        if (log != nullptr) {
            log->logHeapSwap(0, i, arr[i], arr[0]);
            log->logMarkSorted(i, arr[i]);
        }

        int curr = 0;
        while (2 * curr + 1 < i) {
            int largest = curr;
            int left = 2 * curr + 1;
            int right = 2 * curr + 2;

            if (left < i && arr[left] > arr[largest]) {
                largest = left;
            }
            if (right < i && arr[right] > arr[largest]) {
                largest = right;
            }

            if (largest != curr) {
                int t = arr[curr];
                arr[curr] = arr[largest];
                arr[largest] = t;
                if (log != nullptr) {
                    log->logHeapSwap(curr, largest, arr[largest], arr[curr]);
                }
                curr = largest;
            } else {
                break;
            }
        }
    }

    if (log != nullptr) {
        log->logMarkSorted(0, arr[0]);
        log->logHeapSortComplete(n);
    }
}

void MaxHeap::print() const {
    cout << "Heap (size " << currentSize << "/" << capacity << "): ";
    for (int i = 0; i < currentSize; i++) {
        cout << heapArray[i] << " ";
    }
    cout << endl;
}
