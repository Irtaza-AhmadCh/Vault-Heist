#ifndef EVENT_ANIMATION_CONTROLLER_H
#define EVENT_ANIMATION_CONTROLLER_H

#include <cstring>
#include <cstdio>
#include "../../../logic/event.h"
#include "../tree/TreeLayout.h"

struct EventStep {
    char eventType[32];
    int indexA;
    int indexB;
    int valA;
    int valB;
    char extraStr[32];
    float duration;

    EventStep() {
        eventType[0] = '\0';
        indexA = -1;
        indexB = -1;
        valA = 0;
        valB = 0;
        extraStr[0] = '\0';
        duration = 0.5f;
    }

    EventStep(const char* type, int idxA, int idxB, int vA, int vB, const char* extra = "", float dur = 0.5f) {
        if (type != nullptr) {
            strncpy(eventType, type, sizeof(eventType) - 1);
            eventType[sizeof(eventType) - 1] = '\0';
        } else {
            eventType[0] = '\0';
        }
        indexA = idxA;
        indexB = idxB;
        valA = vA;
        valB = vB;
        if (extra != nullptr) {
            strncpy(extraStr, extra, sizeof(extraStr) - 1);
            extraStr[sizeof(extraStr) - 1] = '\0';
        } else {
            extraStr[0] = '\0';
        }
        duration = dur;
    }
};

class EventAnimationController {
public:
    static const int MAX_STEPS = 256;
    EventStep steps[MAX_STEPS];
    int stepCount;
    int currentStepIndex;
    float stepTimer;
    bool isPlaying;
    bool isPaused;
    float speedMultiplier;

    char currentDescription[128];

    EventAnimationController() {
        stepCount = 0;
        currentStepIndex = 0;
        stepTimer = 0.0f;
        isPlaying = false;
        isPaused = false;
        speedMultiplier = 1.0f;
        currentDescription[0] = '\0';
    }

    void clear() {
        stepCount = 0;
        currentStepIndex = 0;
        stepTimer = 0.0f;
        isPlaying = false;
        isPaused = false;
        currentDescription[0] = '\0';
    }

    void addStep(const EventStep& step) {
        if (stepCount >= MAX_STEPS) return;
        steps[stepCount++] = step;
    }

    void loadFromEventLog(const EventLog& log) {
        clear();
        for (int i = 0; i < log.getCount(); i++) {
            Event ev = log.getEvent(i);
            if (ev.type == nullptr) continue;

            if (strcmp(ev.type, EVENT_HEAP_SWAP) == 0) {
                addStep(EventStep(EVENT_HEAP_SWAP, ev.nodeId, ev.parentId, ev.oldValue, ev.newValue, ev.strExtra ? ev.strExtra : "sift", 0.45f));
            } else if (strcmp(ev.type, EVENT_COMPARE) == 0) {
                if (ev.strExtra != nullptr) {
                    addStep(EventStep(EVENT_COMPARE, ev.nodeId, -1, ev.value, 0, ev.strExtra, 0.35f));
                } else {
                    addStep(EventStep(EVENT_COMPARE, ev.nodeId, ev.parentId, ev.oldValue, ev.newValue, "", 0.35f));
                }
            } else if (strcmp(ev.type, EVENT_VISIT) == 0) {
                addStep(EventStep(EVENT_VISIT, ev.nodeId, -1, ev.value, 0, "", 0.4f));
            } else if (strcmp(ev.type, EVENT_INSERT) == 0) {
                addStep(EventStep(EVENT_INSERT, ev.nodeId, ev.parentId, ev.value, ev.side, "", 0.5f));
            } else if (strcmp(ev.type, EVENT_UNLINK) == 0) {
                addStep(EventStep(EVENT_UNLINK, ev.nodeId, ev.parentId, ev.value, ev.side, "", 0.5f));
            } else if (strcmp(ev.type, EVENT_SWAP_VALUE) == 0) {
                addStep(EventStep(EVENT_SWAP_VALUE, ev.nodeId, ev.parentId, ev.oldValue, ev.newValue, "", 0.5f));
            } else if (strcmp(ev.type, EVENT_ROTATE) == 0) {
                addStep(EventStep(EVENT_ROTATE, ev.nodeId, -1, 0, 0, ev.strExtra ? ev.strExtra : "", 0.65f));
            } else if (strcmp(ev.type, EVENT_BALANCE_ALERT) == 0) {
                addStep(EventStep(EVENT_BALANCE_ALERT, ev.nodeId, -1, ev.intExtra, 0, "", 0.5f));
            } else if (strcmp(ev.type, EVENT_UPDATE_HEIGHT) == 0) {
                addStep(EventStep(EVENT_UPDATE_HEIGHT, ev.nodeId, -1, ev.oldValue, ev.newValue, "", 0.25f));
            } else if (strcmp(ev.type, EVENT_UPDATE_SIZE) == 0) {
                addStep(EventStep(EVENT_UPDATE_SIZE, ev.nodeId, -1, ev.oldValue, ev.newValue, "", 0.25f));
            } else if (strcmp(ev.type, EVENT_HIGHLIGHT_SUBTREE) == 0) {
                addStep(EventStep(EVENT_HIGHLIGHT_SUBTREE, ev.nodeId, -1, 0, 0, ev.strExtra ? ev.strExtra : "", 0.5f));
            } else if (strcmp(ev.type, EVENT_HEAP_BUILD) == 0) {
                addStep(EventStep(EVENT_HEAP_BUILD, -1, -1, ev.intExtra, 0, "", 0.4f));
            } else if (strcmp(ev.type, EVENT_HEAPIFY_START) == 0) {
                addStep(EventStep(EVENT_HEAPIFY_START, ev.nodeId, -1, ev.intExtra, 0, "", 0.35f));
            } else if (strcmp(ev.type, EVENT_HEAPIFY_DONE) == 0) {
                addStep(EventStep(EVENT_HEAPIFY_DONE, ev.nodeId, -1, ev.intExtra, 0, "", 0.3f));
            } else if (strcmp(ev.type, EVENT_EXTRACT_MAX) == 0) {
                addStep(EventStep(EVENT_EXTRACT_MAX, ev.nodeId, -1, ev.value, ev.intExtra, "", 0.5f));
            } else if (strcmp(ev.type, EVENT_MARK_SORTED) == 0) {
                addStep(EventStep(EVENT_MARK_SORTED, ev.nodeId, -1, ev.value, 0, "", 0.35f));
            } else if (strcmp(ev.type, EVENT_HEAP_SIZE_CHANGE) == 0) {
                addStep(EventStep(EVENT_HEAP_SIZE_CHANGE, -1, -1, ev.oldValue, ev.newValue, "", 0.25f));
            } else if (strcmp(ev.type, EVENT_HEAP_SORT_COMPLETE) == 0) {
                addStep(EventStep(EVENT_HEAP_SORT_COMPLETE, -1, -1, ev.intExtra, 0, "", 0.5f));
            } else if (strcmp(ev.type, EVENT_DONE) == 0) {
                addStep(EventStep(EVENT_DONE, -1, -1, 0, 0, ev.strExtra ? ev.strExtra : "", 0.3f));
            }
        }
    }

    void start() {
        if (stepCount > 0) {
            currentStepIndex = 0;
            stepTimer = 0.0f;
            isPlaying = true;
            isPaused = false;
        }
    }

    void replay() {
        if (stepCount > 0) {
            currentStepIndex = 0;
            stepTimer = 0.0f;
            isPlaying = true;
            isPaused = false;
        }
    }

    void pause() {
        isPaused = true;
    }

    void resume() {
        isPaused = false;
    }

    void togglePause() {
        isPaused = !isPaused;
    }

    const char* getState() const {
        if (!isPlaying && currentStepIndex == 0) return "idle";
        if (isPaused) return "paused";
        if (isPlaying) return "playing";
        if (currentStepIndex >= stepCount && stepCount > 0) return "completed";
        return "idle";
    }

    void cycleSpeed() {
        if (speedMultiplier <= 0.6f) speedMultiplier = 1.0f;
        else if (speedMultiplier <= 1.1f) speedMultiplier = 1.5f;
        else if (speedMultiplier <= 1.6f) speedMultiplier = 2.0f;
        else speedMultiplier = 0.5f;
    }

    float getProgress() const {
        if (stepCount <= 1) return (stepCount == 1) ? 1.0f : 0.0f;
        return (float)currentStepIndex / (float)(stepCount - 1);
    }

    int getCurrentStep() const {
        return currentStepIndex;
    }

    int getTotalSteps() const {
        return stepCount;
    }

    void applyStep(TreeLayoutEngine& layout, const EventStep& step) {
        if (strcmp(step.eventType, EVENT_HEAP_SWAP) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("swapping");
                layout.nodes[step.indexA].pulseTimer = 0.0f;
                layout.nodes[step.indexA].key = step.valB;
            }
            if (step.indexB >= 0 && step.indexB < layout.nodeCount) {
                layout.nodes[step.indexB].setState("swapping");
                layout.nodes[step.indexB].pulseTimer = 0.0f;
                layout.nodes[step.indexB].key = step.valA;
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "HEAP SWAP: Exchanging pod [%d] (val %d) with pod [%d] (val %d) [%s]",
                     step.indexA, step.valA, step.indexB, step.valB, step.extraStr[0] ? step.extraStr : "sift");
        } else if (strcmp(step.eventType, EVENT_COMPARE) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("searching");
            }
            if (step.indexB >= 0 && step.indexB < layout.nodeCount) {
                layout.nodes[step.indexB].setState("searching");
            }
            if (step.extraStr[0] != '\0') {
                snprintf(currentDescription, sizeof(currentDescription),
                         "COMPARE: Key %d at pod [%d] (Branch: %s)",
                         step.valA, step.indexA, step.extraStr);
            } else {
                snprintf(currentDescription, sizeof(currentDescription),
                         "COMPARE: Sifting comparison between pod [%d] and pod [%d]",
                         step.indexA, step.indexB);
            }
        } else if (strcmp(step.eventType, EVENT_VISIT) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("highlighted");
                layout.nodes[step.indexA].pulseTimer = 0.0f;
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "VISIT: Impulsing pod [id %d, key %d]",
                     step.indexA, step.valA);
        } else if (strcmp(step.eventType, EVENT_INSERT) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("highlighted");
                layout.nodes[step.indexA].pulseTimer = 0.0f;
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "INSERT: Grafted pod with key %d into lattice",
                     step.valA);
        } else if (strcmp(step.eventType, EVENT_UNLINK) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("inactive");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "UNLINK: Severing pod [id %d, key %d]",
                     step.indexA, step.valA);
        } else if (strcmp(step.eventType, EVENT_SWAP_VALUE) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("target");
                layout.nodes[step.indexA].key = step.valB;
            }
            if (step.indexB >= 0 && step.indexB < layout.nodeCount) {
                layout.nodes[step.indexB].setState("searching");
                layout.nodes[step.indexB].key = step.valA;
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "SWAP VALUE: Successor swap (val %d <-> %d)",
                     step.valA, step.valB);
        } else if (strcmp(step.eventType, EVENT_ROTATE) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("highlighted");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "AVL REBALANCE: %s rotation triggered at pivot pod [id %d]",
                     step.extraStr, step.indexA);
        } else if (strcmp(step.eventType, EVENT_BALANCE_ALERT) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("target");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "BALANCE ALERT: Imbalance detected (BF = %d) at pod [id %d]",
                     step.valA, step.indexA);
        } else if (strcmp(step.eventType, EVENT_UPDATE_HEIGHT) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("highlighted");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "UPDATE HEIGHT: Pod [%d] height updated (%d -> %d)",
                     step.indexA, step.valA, step.valB);
        } else if (strcmp(step.eventType, EVENT_UPDATE_SIZE) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].subtreeSize = step.valB;
                layout.nodes[step.indexA].setState("highlighted");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "UPDATE SIZE: Pod [%d] subtree size updated (%d -> %d)",
                     step.indexA, step.valA, step.valB);
        } else if (strcmp(step.eventType, EVENT_HIGHLIGHT_SUBTREE) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("highlighted");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "HIGHLIGHT SUBTREE: Range branch active [%s]",
                     step.extraStr[0] ? step.extraStr : "tinted");
        } else if (strcmp(step.eventType, EVENT_MARK_SORTED) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("sorted");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "HEAP SORT: Element %d placed in final sorted position [%d]",
                     step.valA, step.indexA);
        } else if (strcmp(step.eventType, EVENT_EXTRACT_MAX) == 0) {
            if (layout.nodeCount > 0) {
                layout.nodes[0].setState("target");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "EXTRACT MAX: Priority root (val %d) retrieved from chamber",
                     step.valA);
        } else if (strcmp(step.eventType, EVENT_HEAP_BUILD) == 0) {
            for (int i = 0; i < layout.nodeCount && i < step.valA; i++) {
                layout.nodes[i].setState("searching");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "BUILD HEAP: Initializing bottom-up heapification (%d pods)",
                     step.valA);
        } else if (strcmp(step.eventType, EVENT_HEAPIFY_START) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("target");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "HEAPIFY START: Sifting pod [%d] (region size %d)",
                     step.indexA, step.valA);
        } else if (strcmp(step.eventType, EVENT_HEAPIFY_DONE) == 0) {
            if (step.indexA >= 0 && step.indexA < layout.nodeCount) {
                layout.nodes[step.indexA].setState("normal");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "HEAPIFY DONE: Subtree at pod [%d] restored",
                     step.indexA);
        } else if (strcmp(step.eventType, EVENT_HEAP_SIZE_CHANGE) == 0) {
            snprintf(currentDescription, sizeof(currentDescription),
                     "HEAP SIZE CHANGE: Size adjusted (%d -> %d pods)",
                     step.valA, step.valB);
        } else if (strcmp(step.eventType, EVENT_HEAP_SORT_COMPLETE) == 0) {
            for (int i = 0; i < layout.nodeCount; i++) {
                if (layout.nodes[i].active) layout.nodes[i].setState("sorted");
            }
            snprintf(currentDescription, sizeof(currentDescription),
                     "HEAP SORT COMPLETE: All %d elements sorted in ascending order!",
                     step.valA);
        } else if (strcmp(step.eventType, EVENT_DONE) == 0) {
            snprintf(currentDescription, sizeof(currentDescription),
                     "DONE: %s",
                     step.extraStr[0] ? step.extraStr : "Operation sequence complete.");
        }
    }

    void jumpToStep(int targetIndex, TreeLayoutEngine& layout) {
        if (stepCount == 0) return;
        if (targetIndex < 0) targetIndex = 0;
        if (targetIndex >= stepCount) targetIndex = stepCount - 1;

        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].active) {
                layout.nodes[i].setState("normal");
            }
        }
        for (int i = 0; i <= targetIndex; i++) {
            applyStep(layout, steps[i]);
        }
        currentStepIndex = targetIndex;
        stepTimer = 0.0f;
    }

    void stepForward(TreeLayoutEngine& layout) {
        if (currentStepIndex < stepCount) {
            applyStep(layout, steps[currentStepIndex]);
            currentStepIndex++;
            if (currentStepIndex >= stepCount) {
                isPlaying = false;
            }
        }
    }

    void stepBackward(TreeLayoutEngine& layout) {
        if (currentStepIndex > 1) {
            jumpToStep(currentStepIndex - 2, layout);
            currentStepIndex++;
        } else if (currentStepIndex == 1) {
            jumpToStep(0, layout);
            currentStepIndex = 1;
        }
    }

    void update(float dt, TreeLayoutEngine& layout) {
        if (!isPlaying || isPaused || stepCount == 0) return;

        stepTimer += dt * speedMultiplier;
        if (currentStepIndex < stepCount) {
            if (stepTimer >= steps[currentStepIndex].duration) {
                stepTimer = 0.0f;
                stepForward(layout);
            }
        } else {
            isPlaying = false;
        }
    }
};

#endif
