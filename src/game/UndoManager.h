#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <cstring>

struct TreeSnapshot {
    char structureType[16]; // "tree", "bst", "avl", "heap"
    int keys[32];
    int keyCount;
    int alarmSteps;
    int score;

    TreeSnapshot() {
        structureType[0] = '\0';
        keyCount = 0;
        alarmSteps = 0;
        score = 1000;
        for (int i = 0; i < 32; i++) keys[i] = 0;
    }
};

class UndoManager {
public:
    static const int MAX_SNAPSHOTS = 16;
    TreeSnapshot snapshots[MAX_SNAPSHOTS];
    int count;

    UndoManager() {
        count = 0;
    }

    void clear() {
        count = 0;
    }

    bool canUndo() const {
        return count > 0;
    }

    void pushSnapshot(const char* sType, const int* arr, int n, int alarm, int score) {
        if (count >= MAX_SNAPSHOTS) {
            // Shift left
            for (int i = 0; i < MAX_SNAPSHOTS - 1; i++) {
                snapshots[i] = snapshots[i + 1];
            }
            count = MAX_SNAPSHOTS - 1;
        }

        TreeSnapshot& snap = snapshots[count++];
        strncpy(snap.structureType, sType, sizeof(snap.structureType) - 1);
        snap.structureType[sizeof(snap.structureType) - 1] = '\0';
        snap.keyCount = (n <= 32) ? n : 32;
        for (int i = 0; i < snap.keyCount; i++) {
            snap.keys[i] = arr[i];
        }
        snap.alarmSteps = alarm;
        snap.score = score;
    }

    bool popSnapshot(TreeSnapshot& outSnap) {
        if (count <= 0) return false;
        outSnap = snapshots[--count];
        return true;
    }
};

#endif
