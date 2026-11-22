#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <cstring>
#include "../logic/event.h"

struct TreeSnapshot {
    char structureType[16]; // "tree", "bst", "avl", "heap"
    char serialized[512];   // Serialized state for exact restoration
    int keys[64];
    int keyCount;
    int alarmSteps;
    int score;
    char description[64];

    TreeSnapshot() {
        structureType[0] = '\0';
        serialized[0] = '\0';
        keyCount = 0;
        alarmSteps = 0;
        score = 1000;
        description[0] = '\0';
        for (int i = 0; i < 64; i++) keys[i] = 0;
    }
};

class UndoManager {
public:
    static const int MAX_SNAPSHOTS = 16;
    TreeSnapshot snapshots[MAX_SNAPSHOTS];
    int count;
    EventLog lastActionLog;

    UndoManager() : count(0), lastActionLog(64) {}

    void clear() {
        count = 0;
        lastActionLog.clear();
    }

    bool canUndo() const {
        return count > 0;
    }

    int getCount() const {
        return count;
    }

    void recordActionEvents(const EventLog& log) {
        if (log.getCount() > 0) {
            lastActionLog.copyFrom(log);
        }
    }

    const EventLog& getLastActionLog() const {
        return lastActionLog;
    }

    void pushSnapshot(const char* sType, const char* serializedStr, const int* arr, int n, int alarm, int score, const char* desc = "") {
        if (count >= MAX_SNAPSHOTS) {
            for (int i = 0; i < MAX_SNAPSHOTS - 1; i++) {
                snapshots[i] = snapshots[i + 1];
            }
            count = MAX_SNAPSHOTS - 1;
        }

        TreeSnapshot& snap = snapshots[count++];
        if (sType != nullptr) {
            strncpy(snap.structureType, sType, sizeof(snap.structureType) - 1);
            snap.structureType[sizeof(snap.structureType) - 1] = '\0';
        } else {
            snap.structureType[0] = '\0';
        }

        if (serializedStr != nullptr) {
            strncpy(snap.serialized, serializedStr, sizeof(snap.serialized) - 1);
            snap.serialized[sizeof(snap.serialized) - 1] = '\0';
        } else {
            snap.serialized[0] = '\0';
        }

        snap.keyCount = (n <= 64) ? n : 64;
        if (arr != nullptr) {
            for (int i = 0; i < snap.keyCount; i++) {
                snap.keys[i] = arr[i];
            }
        }

        snap.alarmSteps = alarm;
        snap.score = score;

        if (desc != nullptr) {
            strncpy(snap.description, desc, sizeof(snap.description) - 1);
            snap.description[sizeof(snap.description) - 1] = '\0';
        } else {
            snap.description[0] = '\0';
        }
    }

    void pushSnapshot(const char* sType, const int* arr, int n, int alarm, int score) {
        pushSnapshot(sType, "", arr, n, alarm, score, "");
    }

    bool popSnapshot(TreeSnapshot& outSnap) {
        if (count <= 0) return false;
        outSnap = snapshots[--count];
        return true;
    }

    bool peekSnapshot(TreeSnapshot& outSnap) const {
        if (count <= 0) return false;
        outSnap = snapshots[count - 1];
        return true;
    }
};

#endif
