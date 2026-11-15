#ifndef EVENT_H
#define EVENT_H

#include <cstring>

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

    Event();
};

struct EventLog {
    Event* items;
    int capacity;
    int count;
    bool enabled;

    EventLog(int initialCapacity = 64);
    ~EventLog();

    void resize(int newCapacity);
    void addEvent(const Event& ev);
    void logHeapSwap(int indexA, int indexB, int valA, int valB);
    void logCompare(int indexA, int indexB, int valA, int valB);
    void logHeapBuild(int heapSize);
    void logHeapifyStart(int startIndex, int heapSize);
    void logHeapifyDone(int startIndex, int heapSize);
    void logExtractMax(int maxVal, int rootIndex, int remainingHeapSize);
    void logMarkSorted(int index, int value);
    void logHeapSizeChange(int oldSize, int newSize);
    void logHeapSortComplete(int totalElements);
    void logRotate(int pivotId, const char* rotType);
    void logUpdateHeight(int nodeId, int oldHeight, int newHeight);
    void logUpdateSize(int nodeId, int oldSize, int newSize);
    void logBalanceAlert(int nodeId, int balanceFactor);
    void clear();
    int getCount() const;
    Event getEvent(int index) const;
};

#endif

