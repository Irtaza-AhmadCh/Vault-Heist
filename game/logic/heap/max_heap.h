#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "../event.h"

struct HeapElement {
    int value;
    int originalIndex;

    HeapElement();
    HeapElement(int val, int origIdx);
};

class MaxHeap {
private:
    int* heapArray;
    int capacity;
    int currentSize;
    EventLog* eventLog;

    void swapElements(int indexA, int indexB);
    void resize(int newCapacity);

public:
    MaxHeap(int initialCapacity = 32, EventLog* log = nullptr);
    MaxHeap(const int* arr, int n, EventLog* log = nullptr);
    MaxHeap(const MaxHeap& other);
    ~MaxHeap();

    void setEventLog(EventLog* log);
    EventLog* getEventLog() const;

    int parent(int i) const;
    int leftChild(int i) const;
    int rightChild(int i) const;
    bool hasLeftChild(int i, int heapSize) const;
    bool hasRightChild(int i, int heapSize) const;

    int getSize() const;
    int getCapacity() const;
    bool isEmpty() const;
    int get(int index) const;
    int peekMax() const;
    const int* getArray() const;

    void siftUp(int index);
    void siftDown(int index, int heapSize);
    void siftDown(int index);
    void insert(int value);
    void buildMaxHeap();
    void buildMaxHeap(const int* arr, int n);
    int extractMax();
    void heapSort();

    bool isMaxHeap() const;
    static bool isMaxHeap(const int* arr, int n);
    static void heapSort(int* arr, int n, EventLog* log = nullptr);

    void print() const;
};

#endif
