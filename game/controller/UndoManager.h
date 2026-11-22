#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <cstring>
#include "../logic/event.h"

struct TreeSnapshot {
    static const int MAX_CONTROLLER_STATE = 4096;
    char structureType[16]; // "tree", "bst", "avl", "heap"
    char serialized[512];   // Serialized state for exact restoration
    int keys[64];
    int keyCount;
    int alarmSteps;
    int score;
    char description[64];
    char controllerStateType[16];
    unsigned char controllerState[MAX_CONTROLLER_STATE];
    int controllerStateSize;

    TreeSnapshot() {
        structureType[0] = '\0';
        serialized[0] = '\0';
        keyCount = 0;
        alarmSteps = 0;
        score = 1000;
        description[0] = '\0';
        controllerStateType[0] = '\0';
        controllerStateSize = 0;
        memset(controllerState, 0, sizeof(controllerState));
        for (int i = 0; i < 64; i++) keys[i] = 0;
    }

    void set(const char* sType, const char* serializedStr, const int* arr, int n, int alarm, int currentScore, const char* desc = "") {
        structureType[0] = '\0';
        serialized[0] = '\0';
        if (sType != nullptr) {
            strncpy(structureType, sType, sizeof(structureType) - 1);
            structureType[sizeof(structureType) - 1] = '\0';
        }
        if (serializedStr != nullptr) {
            strncpy(serialized, serializedStr, sizeof(serialized) - 1);
            serialized[sizeof(serialized) - 1] = '\0';
        }
        keyCount = n < 0 ? 0 : (n <= 64 ? n : 64);
        for (int i = 0; i < keyCount; i++) keys[i] = arr != nullptr ? arr[i] : 0;
        alarmSteps = alarm;
        score = currentScore;
        if (desc != nullptr) {
            strncpy(description, desc, sizeof(description) - 1);
            description[sizeof(description) - 1] = '\0';
        } else description[0] = '\0';
        controllerStateType[0] = '\0';
        controllerStateSize = 0;
    }

    void setControllerState(const char* stateType, const void* state, int size) {
        if (stateType == nullptr || state == nullptr || size <= 0 || size > MAX_CONTROLLER_STATE) return;
        strncpy(controllerStateType, stateType, sizeof(controllerStateType) - 1);
        controllerStateType[sizeof(controllerStateType) - 1] = '\0';
        memcpy(controllerState, state, size);
        controllerStateSize = size;
    }
};

class UndoManager {
public:
    static const int MAX_SNAPSHOTS = 16;
    TreeSnapshot snapshots[MAX_SNAPSHOTS];
    EventLog snapshotEventLogs[MAX_SNAPSHOTS];
    int count;
    EventLog lastActionLog;
    TreeSnapshot replaySnapshot;
    TreeSnapshot undoneSnapshot;
    bool hasReplayState;
    bool hasUndoneState;

    UndoManager() : count(0), lastActionLog(64), hasReplayState(false), hasUndoneState(false) {}

    void clear() {
        count = 0;
        lastActionLog.clear();
        for (int i = 0; i < MAX_SNAPSHOTS; i++) snapshotEventLogs[i].clear();
        hasReplayState = false;
        hasUndoneState = false;
    }

    bool canUndo() const {
        return count > 0;
    }

    int getCount() const {
        return count;
    }

    void recordActionEvents(const EventLog& log) {
        lastActionLog.clear();
        if (log.getCount() > 0) {
            lastActionLog.copyFrom(log);
        }
        if (count > 0) snapshotEventLogs[count - 1].copyFrom(lastActionLog);
    }

    void clearLastActionEvents() {
        lastActionLog.clear();
    }

    void clearReplayState() {
        hasReplayState = false;
        hasUndoneState = false;
    }

    void setReplaySnapshot(const TreeSnapshot& snapshot) {
        replaySnapshot = snapshot;
        hasReplayState = true;
    }

    bool getReplaySnapshot(TreeSnapshot& snapshot) const {
        if (!hasReplayState) return false;
        snapshot = replaySnapshot;
        return true;
    }

    void setUndoneSnapshot(const TreeSnapshot& snapshot) {
        undoneSnapshot = snapshot;
        hasUndoneState = true;
    }

    bool getUndoneSnapshot(TreeSnapshot& snapshot) const {
        if (!hasUndoneState) return false;
        snapshot = undoneSnapshot;
        return true;
    }

    void clearUndoneSnapshot() {
        hasUndoneState = false;
    }

    void pushExistingSnapshot(const TreeSnapshot& snapshot) {
        if (count >= MAX_SNAPSHOTS) {
            for (int i = 0; i < MAX_SNAPSHOTS - 1; i++) {
                snapshots[i] = snapshots[i + 1];
                snapshotEventLogs[i].copyFrom(snapshotEventLogs[i + 1]);
            }
            count = MAX_SNAPSHOTS - 1;
        }
        snapshots[count++] = snapshot;
        snapshotEventLogs[count - 1].copyFrom(lastActionLog);
    }

    void pushSnapshot(const TreeSnapshot& snapshot) {
        pushExistingSnapshot(snapshot);
    }

    const EventLog& getLastActionLog() const {
        return lastActionLog;
    }

    void pushSnapshot(const char* sType, const char* serializedStr, const int* arr, int n, int alarm, int score, const char* desc = "") {
        TreeSnapshot snap;
        snap.set(sType, serializedStr, arr, n, alarm, score, desc);
        clearLastActionEvents();
        pushExistingSnapshot(snap);
    }

    void pushSnapshot(const char* sType, const int* arr, int n, int alarm, int score) {
        pushSnapshot(sType, "", arr, n, alarm, score, "");
    }

    void setLatestControllerState(const char* stateType, const void* state, int size) {
        if (count <= 0) return;
        snapshots[count - 1].setControllerState(stateType, state, size);
    }

    bool popSnapshot(TreeSnapshot& outSnap) {
        if (count <= 0) return false;
        --count;
        outSnap = snapshots[count];
        lastActionLog.copyFrom(snapshotEventLogs[count]);
        snapshotEventLogs[count].clear();
        return true;
    }

    bool peekSnapshot(TreeSnapshot& outSnap) const {
        if (count <= 0) return false;
        outSnap = snapshots[count - 1];
        return true;
    }
};

#endif
