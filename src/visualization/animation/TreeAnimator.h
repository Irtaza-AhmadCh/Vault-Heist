#ifndef TREE_ANIMATOR_H
#define TREE_ANIMATOR_H

#include <cstring>
#include "../tree/TreeLayout.h"

struct AnimationStep {
    int nodeIndex;
    char targetState[16];
    float duration;

    AnimationStep() {
        nodeIndex = -1;
        targetState[0] = '\0';
        duration = 0.5f;
    }

    AnimationStep(int idx, const char* stateStr, float dur = 0.5f) {
        nodeIndex = idx;
        if (stateStr != nullptr) {
            strncpy(targetState, stateStr, sizeof(targetState) - 1);
            targetState[sizeof(targetState) - 1] = '\0';
        } else {
            targetState[0] = '\0';
        }
        duration = dur;
    }
};

class TreeAnimator {
public:
    static const int MAX_STEPS = 256;
    AnimationStep steps[MAX_STEPS];
    int stepCount;
    int currentStepIndex;
    float stepTimer;
    bool isPlaying;
    bool isPaused;
    float speedMultiplier;

    TreeAnimator() {
        stepCount = 0;
        currentStepIndex = 0;
        stepTimer = 0.0f;
        isPlaying = false;
        isPaused = false;
        speedMultiplier = 1.0f;
    }

    void clear() {
        stepCount = 0;
        currentStepIndex = 0;
        stepTimer = 0.0f;
        isPlaying = false;
        isPaused = false;
    }

    void addStep(int nodeIndex, const char* stateStr, float duration = 0.5f) {
        if (stepCount >= MAX_STEPS) return;
        steps[stepCount++] = AnimationStep(nodeIndex, stateStr, duration);
    }

    void start() {
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

    void stepForward(TreeLayoutEngine& layout) {
        if (currentStepIndex < stepCount) {
            applyStep(layout, steps[currentStepIndex]);
            currentStepIndex++;
            if (currentStepIndex >= stepCount) {
                isPlaying = false;
            }
        }
    }

    void applyStep(TreeLayoutEngine& layout, const AnimationStep& step) {
        if (step.nodeIndex >= 0 && step.nodeIndex < layout.nodeCount) {
            layout.nodes[step.nodeIndex].setState(step.targetState);
            layout.nodes[step.nodeIndex].pulseTimer = 0.0f;
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
