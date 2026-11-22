#include "event.h"

Event::Event() {
    type = EVENT_NONE;
    nodeId = -1;
    parentId = -1;
    side = 0;
    value = 0;
    oldValue = 0;
    newValue = 0;
    strExtra = nullptr;
    intExtra = 0;
    arraySnapshotSize = 0;
    for (int i = 0; i < 32; i++) {
        arraySnapshot[i] = 0;
    }
}

EventLog::EventLog(int initialCapacity) {
    if (initialCapacity < 16) {
        initialCapacity = 16;
    }
    capacity = initialCapacity;
    count = 0;
    enabled = true;
    items = new Event[capacity];
}

EventLog::EventLog(const EventLog& other) {
    capacity = other.capacity > 0 ? other.capacity : 16;
    count = other.count;
    enabled = other.enabled;
    items = new Event[capacity];
    for (int i = 0; i < count; i++) {
        items[i] = other.items[i];
    }
}

EventLog& EventLog::operator=(const EventLog& other) {
    if (this == &other) {
        return *this;
    }
    copyFrom(other);
    return *this;
}

EventLog::~EventLog() {
    if (items != nullptr) {
        delete[] items;
        items = nullptr;
    }
    count = 0;
    capacity = 0;
}

void EventLog::resize(int newCapacity) {
    if (newCapacity <= capacity) {
        return;
    }
    Event* newItems = new Event[newCapacity];
    for (int i = 0; i < count; i++) {
        newItems[i] = items[i];
    }
    delete[] items;
    items = newItems;
    capacity = newCapacity;
}

void EventLog::copyFrom(const EventLog& other) {
    if (this == &other) return;
    if (items != nullptr) {
        delete[] items;
        items = nullptr;
    }
    capacity = other.capacity > other.count ? other.capacity : other.count;
    if (capacity < 16) capacity = 16;
    count = other.count;
    enabled = other.enabled;
    items = new Event[capacity];
    if (other.items != nullptr) {
        for (int i = 0; i < count; i++) {
            items[i] = other.items[i];
        }
    }
}

void EventLog::addEvent(const Event& ev) {
    if (!enabled) {
        return;
    }
    if (count >= capacity) {
        resize(capacity * 2);
    }
    items[count] = ev;
    count++;
}

void EventLog::logVisit(int nodeId, int value) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_VISIT;
    ev.nodeId = nodeId;
    ev.value = value;
    addEvent(ev);
}

void EventLog::logCompare(int nodeId, int key, const char* result) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_COMPARE;
    ev.nodeId = nodeId;
    ev.value = key;
    ev.strExtra = result;
    addEvent(ev);
}

void EventLog::logCompare(int indexA, int indexB, int valA, int valB) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_COMPARE;
    ev.nodeId = indexA;
    ev.parentId = indexB;
    ev.oldValue = valA;
    ev.newValue = valB;
    addEvent(ev);
}

void EventLog::logInsert(int nodeId, int parentId, int side, int value) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_INSERT;
    ev.nodeId = nodeId;
    ev.parentId = parentId;
    ev.side = side;
    ev.value = value;
    addEvent(ev);
}

void EventLog::logUnlink(int nodeId, int parentId, int side, int value) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_UNLINK;
    ev.nodeId = nodeId;
    ev.parentId = parentId;
    ev.side = side;
    ev.value = value;
    addEvent(ev);
}

void EventLog::logSwapValue(int nodeA, int nodeB, int valA, int valB) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_SWAP_VALUE;
    ev.nodeId = nodeA;
    ev.parentId = nodeB;
    ev.oldValue = valA;
    ev.newValue = valB;
    addEvent(ev);
}

void EventLog::logRotate(int pivotId, const char* rotType) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_ROTATE;
    ev.nodeId = pivotId;
    ev.strExtra = rotType;
    addEvent(ev);
}

void EventLog::logUpdateHeight(int nodeId, int oldHeight, int newHeight) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_UPDATE_HEIGHT;
    ev.nodeId = nodeId;
    ev.oldValue = oldHeight;
    ev.newValue = newHeight;
    addEvent(ev);
}

void EventLog::logUpdateSize(int nodeId, int oldSize, int newSize) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_UPDATE_SIZE;
    ev.nodeId = nodeId;
    ev.oldValue = oldSize;
    ev.newValue = newSize;
    addEvent(ev);
}

void EventLog::logBalanceAlert(int nodeId, int balanceFactor) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_BALANCE_ALERT;
    ev.nodeId = nodeId;
    ev.intExtra = balanceFactor;
    addEvent(ev);
}

void EventLog::logHighlightSubtree(int nodeId, const char* mode) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HIGHLIGHT_SUBTREE;
    ev.nodeId = nodeId;
    ev.strExtra = mode;
    addEvent(ev);
}

void EventLog::logHeapBuild(int heapSize, const int* arr) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HEAP_BUILD;
    ev.intExtra = heapSize;
    if (arr != nullptr && heapSize > 0) {
        int n = (heapSize <= 32) ? heapSize : 32;
        ev.arraySnapshotSize = n;
        for (int i = 0; i < n; i++) {
            ev.arraySnapshot[i] = arr[i];
        }
    }
    addEvent(ev);
}

void EventLog::logHeapSwap(int indexA, int indexB, int valA, int valB, const char* reason) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HEAP_SWAP;
    ev.nodeId = indexA;
    ev.parentId = indexB;
    ev.oldValue = valA;
    ev.newValue = valB;
    ev.strExtra = reason;
    addEvent(ev);
}

void EventLog::logHeapifyStart(int startIndex, int heapSize) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HEAPIFY_START;
    ev.nodeId = startIndex;
    ev.intExtra = heapSize;
    addEvent(ev);
}

void EventLog::logHeapifyDone(int startIndex, int heapSize) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HEAPIFY_DONE;
    ev.nodeId = startIndex;
    ev.intExtra = heapSize;
    addEvent(ev);
}

void EventLog::logExtractMax(int maxVal, int rootIndex, int remainingHeapSize) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_EXTRACT_MAX;
    ev.value = maxVal;
    ev.nodeId = rootIndex;
    ev.intExtra = remainingHeapSize;
    addEvent(ev);
}

void EventLog::logMarkSorted(int index, int value) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_MARK_SORTED;
    ev.nodeId = index;
    ev.value = value;
    addEvent(ev);
}

void EventLog::logHeapSizeChange(int oldSize, int newSize) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HEAP_SIZE_CHANGE;
    ev.oldValue = oldSize;
    ev.newValue = newSize;
    addEvent(ev);
}

void EventLog::logHeapSortComplete(int totalElements, const int* arr) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_HEAP_SORT_COMPLETE;
    ev.intExtra = totalElements;
    if (arr != nullptr && totalElements > 0) {
        int n = (totalElements <= 32) ? totalElements : 32;
        ev.arraySnapshotSize = n;
        for (int i = 0; i < n; i++) {
            ev.arraySnapshot[i] = arr[i];
        }
    }
    addEvent(ev);
}

void EventLog::logDone(const char* summary) {
    if (!enabled) return;
    Event ev;
    ev.type = EVENT_DONE;
    ev.strExtra = summary;
    addEvent(ev);
}

void EventLog::clear() {
    count = 0;
}

int EventLog::getCount() const {
    return count;
}

Event EventLog::getEvent(int index) const {
    if (index >= 0 && index < count) {
        return items[index];
    }
    return Event();
}
