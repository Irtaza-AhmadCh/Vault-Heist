#ifndef EVENT_H
#define EVENT_H

#include <cstring>

// Standardized snake_case event type string literals (Section 4.3)
const char* const EVENT_NONE = "none";
const char* const EVENT_VISIT = "visit";
const char* const EVENT_COMPARE = "compare";
const char* const EVENT_INSERT = "insert";
const char* const EVENT_UNLINK = "unlink";
const char* const EVENT_SWAP_VALUE = "swap_value";
const char* const EVENT_ROTATE = "rotate";
const char* const EVENT_UPDATE_HEIGHT = "update_height";
const char* const EVENT_UPDATE_SIZE = "update_size";
const char* const EVENT_BALANCE_ALERT = "balance_alert";
const char* const EVENT_HIGHLIGHT_SUBTREE = "highlight_subtree";
const char* const EVENT_HEAP_BUILD = "heap_build";
const char* const EVENT_HEAP_SWAP = "heap_swap";
const char* const EVENT_HEAPIFY_START = "heapify_start";
const char* const EVENT_HEAPIFY_DONE = "heapify_done";
const char* const EVENT_EXTRACT_MAX = "extract_max";
const char* const EVENT_MARK_SORTED = "mark_sorted";
const char* const EVENT_HEAP_SIZE_CHANGE = "heap_size_change";
const char* const EVENT_HEAP_SORT_COMPLETE = "heap_sort_complete";
const char* const EVENT_DONE = "done";

// Rotation sub-types
const char* const ROTATION_LL = "ll";
const char* const ROTATION_RR = "rr";
const char* const ROTATION_LR = "lr";
const char* const ROTATION_RL = "rl";

struct Event {
    const char* type;
    int nodeId;
    int parentId;
    int side;
    int value;
    int oldValue;
    int newValue;
    const char* strExtra;
    int intExtra;
    int arraySnapshot[32];
    int arraySnapshotSize;

    Event();
};

struct EventLog {
    Event* items;
    int capacity;
    int count;
    bool enabled;

    EventLog(int initialCapacity = 64);
    EventLog(const EventLog& other);
    EventLog& operator=(const EventLog& other);
    ~EventLog();

    void resize(int newCapacity);
    void copyFrom(const EventLog& other);
    void addEvent(const Event& ev);

    // Logging helpers for Section 4.3 Event Model
    void logVisit(int nodeId, int value = 0);
    void logCompare(int nodeId, int key, const char* result);
    void logCompare(int indexA, int indexB, int valA, int valB);
    void logInsert(int nodeId, int parentId, int side, int value);
    void logUnlink(int nodeId, int parentId, int side, int value);
    void logSwapValue(int nodeA, int nodeB, int valA, int valB);
    void logRotate(int pivotId, const char* rotType);
    void logUpdateHeight(int nodeId, int oldHeight, int newHeight);
    void logUpdateSize(int nodeId, int oldSize, int newSize);
    void logBalanceAlert(int nodeId, int balanceFactor);
    void logHighlightSubtree(int nodeId, const char* mode);

    // Heap events
    void logHeapBuild(int heapSize, const int* arr = nullptr);
    void logHeapSwap(int indexA, int indexB, int valA, int valB, const char* reason = "sift");
    void logHeapifyStart(int startIndex, int heapSize);
    void logHeapifyDone(int startIndex, int heapSize);
    void logExtractMax(int maxVal, int rootIndex, int remainingHeapSize);
    void logMarkSorted(int index, int value);
    void logHeapSizeChange(int oldSize, int newSize);
    void logHeapSortComplete(int totalElements, const int* arr = nullptr);

    void logDone(const char* summary);
    void clear();
    int getCount() const;
    Event getEvent(int index) const;
};

#endif
