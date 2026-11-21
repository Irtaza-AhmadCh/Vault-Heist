#include "Act4State.h"

Act4Controller::Act4Controller() {
    activeLevelNumber = 1;
    currentHeap = nullptr;
}

void Act4Controller::initLevel(int levelNum, MaxHeap* heapModel, ScoreSystem& score, const LevelDef* lvlDef) {
    activeLevelNumber = levelNum;
    currentHeap = heapModel;

    if (currentHeap == nullptr) return;

    int mech = levelNum;
    if (lvlDef != nullptr) {
        if (strcmp(lvlDef->gameplayType, "build_heap") == 0 || strcmp(lvlDef->requiredAction, "build_heap") == 0) mech = 1;
        else if (strcmp(lvlDef->gameplayType, "heap_repair") == 0 || strcmp(lvlDef->requiredAction, "repair") == 0) mech = 2;
        else if (strcmp(lvlDef->gameplayType, "extract_max") == 0 || strcmp(lvlDef->requiredAction, "extract_max") == 0) mech = 3;
        else if (strcmp(lvlDef->gameplayType, "extraction_race") == 0 || strcmp(lvlDef->requiredAction, "race") == 0) mech = 4;
        else if (strcmp(lvlDef->gameplayType, "heap_sort") == 0 || strcmp(lvlDef->requiredAction, "boss") == 0) mech = 5;
        else mech = ((levelNum - 1) % 5) + 1;
    } else {
        mech = ((levelNum - 1) % 5) + 1;
    }

    if (mech == 1) {
        // Level 4.1: Activate the Vault (Bottom-up Max Heap construction)
        if (lvlDef && lvlDef->initialValueCount > 0) {
            lvl41.heapSize = lvlDef->initialValueCount;
            for (int i = 0; i < lvlDef->initialValueCount; i++) lvl41.initialArray[i] = lvlDef->initialValues[i];
        } else {
            int raw[5] = {4, 10, 3, 5, 1};
            lvl41.heapSize = 5;
            for (int i = 0; i < 5; i++) lvl41.initialArray[i] = raw[i];
        }

        currentHeap->loadRawArray(lvl41.initialArray, lvl41.heapSize);

        // Last non-leaf index: floor(n / 2) - 1
        lvl41.currentParentIndex = (lvl41.heapSize / 2) - 1;
        lvl41.currentSiftIndex = lvl41.currentParentIndex;
        lvl41.selectedChildIndex = -1;
        lvl41.proposedSwapText[0] = '\0';
        lvl41.isLevelComplete = false;

        snprintf(lvl41.statusMsg, sizeof(lvl41.statusMsg),
                 "ACTIVATE THE VAULT: Parent Pod [%d] (value %d) highlighted. Inspect children.",
                 lvl41.currentParentIndex, currentHeap->get(lvl41.currentParentIndex));

        score.reset(score.optimalSteps, score.alarmCapacity);
    } else if (mech == 2) {
        // Level 4.2: Repair the Security Locks (Sift-Down Round A & Sift-Up Round B)
        if (lvlDef && lvlDef->initialValueCount > 0) {
            lvl42.heapSize = lvlDef->initialValueCount;
            for (int i = 0; i < lvlDef->initialValueCount; i++) lvl42.initialArray[i] = lvlDef->initialValues[i];
        } else {
            int raw[5] = {4, 10, 3, 5, 1};
            lvl42.heapSize = 5;
            for (int i = 0; i < 5; i++) lvl42.initialArray[i] = raw[i];
        }
        lvl42.round = 1;
        currentHeap->loadRawArray(lvl42.initialArray, lvl42.heapSize);

        lvl42.currentSiftIndex = 0; // Root violates max heap
        lvl42.selectedChildIndex = -1;
        lvl42.currentUpChildIndex = -1;
        lvl42.currentUpParentIndex = -1;
        lvl42.proposedSwapText[0] = '\0';
        lvl42.isLevelComplete = false;

        snprintf(lvl42.statusMsg, sizeof(lvl42.statusMsg),
                 "ROUND A (SIFT-DOWN): Disturbed root [0] (value %d). Select larger child to repair.",
                 currentHeap->get(0));

        score.reset(score.optimalSteps, score.alarmCapacity);
    } else if (mech == 3) {
        // Level 4.3: Extract the Greatest Treasure (Extract Maximum & Heapify)
        if (lvlDef && lvlDef->initialValueCount > 0) {
            lvl43.totalElements = lvlDef->initialValueCount;
            lvl43.activeHeapSize = lvlDef->initialValueCount;
            for (int i = 0; i < lvlDef->initialValueCount; i++) lvl43.initialArray[i] = lvlDef->initialValues[i];
        } else {
            int raw[5] = {10, 5, 3, 4, 1};
            lvl43.totalElements = 5;
            lvl43.activeHeapSize = 5;
            for (int i = 0; i < 5; i++) lvl43.initialArray[i] = raw[i];
        }
        lvl43.extractionCount = 0;
        lvl43.subPhase = 0; // 0 = WAIT_EXTRACT, 1 = SIFTING_DOWN
        lvl43.currentSiftIndex = -1;
        lvl43.selectedChildIndex = -1;
        lvl43.proposedSwapText[0] = '\0';
        lvl43.isLevelComplete = false;

        currentHeap->loadRawArray(lvl43.initialArray, lvl43.activeHeapSize);

        snprintf(lvl43.statusMsg, sizeof(lvl43.statusMsg),
                 "EXTRACT GREATEST TREASURE: Root contains maximum (%d). Click 'Extract Maximum'.",
                 currentHeap->get(0));

        score.reset(score.optimalSteps, score.alarmCapacity);
    } else if (mech == 4) {
        // Level 4.4: Race Against the Guards (Multi-round extraction under alarm pressure)
        if (lvlDef && lvlDef->initialValueCount > 0) {
            lvl44.totalElements = lvlDef->initialValueCount;
            lvl44.activeHeapSize = lvlDef->initialValueCount;
            for (int i = 0; i < lvlDef->initialValueCount; i++) lvl44.initialArray[i] = lvlDef->initialValues[i];
        } else {
            int raw[6] = {15, 12, 8, 10, 6, 4};
            lvl44.totalElements = 6;
            lvl44.activeHeapSize = 6;
            for (int i = 0; i < 6; i++) lvl44.initialArray[i] = raw[i];
        }
        lvl44.extractionRoundTarget = lvl44.totalElements > 2 ? lvl44.totalElements - 2 : 1;
        lvl44.completedExtractions = 0;
        lvl44.subPhase = 0; // 0 = WAIT_EXTRACT, 1 = SIFTING_DOWN
        lvl44.currentSiftIndex = -1;
        lvl44.selectedChildIndex = -1;
        lvl44.proposedSwapText[0] = '\0';
        lvl44.isLevelComplete = false;

        currentHeap->loadRawArray(lvl44.initialArray, lvl44.activeHeapSize);

        snprintf(lvl44.statusMsg, sizeof(lvl44.statusMsg),
                 "RACE AGAINST GUARDS: Complete %d extractions before alarm reaches capacity! Click 'Extract Maximum'.",
                 lvl44.extractionRoundTarget);

        score.reset(score.optimalSteps, score.alarmCapacity);
    } else if (mech == 5) {
        // Level 4.5: Final Vault Boss (Independent full Heap Sort)
        if (lvlDef && lvlDef->initialValueCount > 0) {
            lvl45.totalElements = lvlDef->initialValueCount;
            lvl45.activeHeapSize = lvlDef->initialValueCount;
            for (int i = 0; i < lvlDef->initialValueCount; i++) lvl45.initialArray[i] = lvlDef->initialValues[i];
        } else {
            int raw[7] = {12, 3, 17, 8, 15, 6, 1};
            lvl45.totalElements = 7;
            lvl45.activeHeapSize = 7;
            for (int i = 0; i < 7; i++) lvl45.initialArray[i] = raw[i];
        }
        strncpy(lvl45.phase, "build", sizeof(lvl45.phase) - 1);

        lvl45.currentParentIndex = (lvl45.totalElements / 2) - 1;
        lvl45.currentSiftIndex = lvl45.currentParentIndex;
        lvl45.selectedChildIndex = -1;
        lvl45.subPhase = 0;
        lvl45.completedExtractions = 0;
        lvl45.proposedSwapText[0] = '\0';
        lvl45.isLevelComplete = false;

        currentHeap->loadRawArray(lvl45.initialArray, lvl45.activeHeapSize);

        snprintf(lvl45.statusMsg, sizeof(lvl45.statusMsg),
                 "PHASE A (BUILD): Process parent [%d] (value %d). Inspect children.",
                 lvl45.currentParentIndex, currentHeap->get(lvl45.currentParentIndex));

        score.reset(score.optimalSteps, score.alarmCapacity);
    }
}

void Act4Controller::cancelSelection() {
    lvl41.selectedChildIndex = -1;
    lvl41.proposedSwapText[0] = '\0';
    lvl42.selectedChildIndex = -1;
    lvl42.proposedSwapText[0] = '\0';
    lvl43.selectedChildIndex = -1;
    lvl43.proposedSwapText[0] = '\0';
    lvl44.selectedChildIndex = -1;
    lvl44.proposedSwapText[0] = '\0';
    lvl45.selectedChildIndex = -1;
    lvl45.proposedSwapText[0] = '\0';
}

void Act4Controller::advanceAfterSiftComplete(ScoreSystem& score, char* feedback, int maxLen) {
    lvl41.currentParentIndex--;
    if (lvl41.currentParentIndex < 0) {
        // Bottom-up construction finished!
        if (currentHeap->isMaxHeap()) {
            lvl41.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Max Heap constructed! All parents satisfy Parent >= Children.");
        } else {
            snprintf(feedback, maxLen, "Construction complete, but invariant check failed.");
        }
    } else {
        lvl41.currentSiftIndex = lvl41.currentParentIndex;
        lvl41.selectedChildIndex = -1;
        lvl41.proposedSwapText[0] = '\0';
        snprintf(feedback, maxLen, "Parent verified. Moving to parent at index [%d] (value %d).",
                 lvl41.currentParentIndex, currentHeap->get(lvl41.currentParentIndex));
    }
}

// ============================================================================
// LEVEL 4.1 IMPLEMENTATION
// ============================================================================

bool Act4Controller::handleLevel41NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl41.isLevelComplete || currentHeap == nullptr) return false;

    int parentIdx = lvl41.currentSiftIndex;
    int parentVal = currentHeap->get(parentIdx);
    int leftIdx = 2 * parentIdx + 1;
    int rightIdx = 2 * parentIdx + 2;
    int n = currentHeap->getSize();

    bool isLeft = (leftIdx < n && clickedIndex == leftIdx);
    bool isRight = (rightIdx < n && clickedIndex == rightIdx);

    if (!isLeft && !isRight) {
        if (clickedIndex == parentIdx) {
            snprintf(feedback, maxLen, "Pod [%d] is the parent. Choose its larger child or click 'No Swap Needed'.", clickedIndex);
            return false;
        }
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "You can only swap the highlighted parent with one of its children. (+1 Alarm)");
        return false;
    }

    int largestChild = leftIdx;
    if (rightIdx < n && currentHeap->get(rightIdx) > currentHeap->get(leftIdx)) {
        largestChild = rightIdx;
    }

    if (clickedIndex != largestChild) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Choose the larger child. (+1 Alarm)");
        return false;
    }

    int childVal = currentHeap->get(clickedIndex);
    if (childVal <= parentVal) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "This parent already satisfies the Max Heap property. Choose 'No Swap Needed'. (+1 Alarm)");
        return false;
    }

    lvl41.selectedChildIndex = clickedIndex;
    snprintf(lvl41.proposedSwapText, sizeof(lvl41.proposedSwapText), "Swap %d <-> %d", parentVal, childVal);
    snprintf(feedback, maxLen, "Selected Child [%d] (value %d). Proposed: Swap %d <-> %d. Click 'Confirm Swap'.",
             clickedIndex, childVal, parentVal, childVal);
    return true;
}

bool Act4Controller::handleLevel41ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl41.isLevelComplete || currentHeap == nullptr) return false;

    if (lvl41.selectedChildIndex == -1) {
        snprintf(feedback, maxLen, "Select the larger child first.");
        return false;
    }

    int pIdx = lvl41.currentSiftIndex;
    int cIdx = lvl41.selectedChildIndex;
    int pVal = currentHeap->get(pIdx);
    int cVal = currentHeap->get(cIdx);

    currentHeap->swapElements(pIdx, cIdx);
    score.addAlarmStep(1);

    lvl41.selectedChildIndex = -1;
    lvl41.proposedSwapText[0] = '\0';

    lvl41.currentSiftIndex = cIdx;
    int n = currentHeap->getSize();
    int newLeft = 2 * cIdx + 1;
    int newRight = 2 * cIdx + 2;

    if (newLeft >= n) {
        advanceAfterSiftComplete(score, feedback, maxLen);
        return true;
    }

    int largestChild = newLeft;
    if (newRight < n && currentHeap->get(newRight) > currentHeap->get(newLeft)) {
        largestChild = newRight;
    }

    if (currentHeap->get(largestChild) > currentHeap->get(lvl41.currentSiftIndex)) {
        snprintf(feedback, maxLen, "Swapped %d <-> %d! Continue sift-down at index [%d]: Child [%d] (value %d) is larger.",
                 pVal, cVal, cIdx, largestChild, currentHeap->get(largestChild));
    } else {
        advanceAfterSiftComplete(score, feedback, maxLen);
    }
    return true;
}

bool Act4Controller::handleLevel41NoSwapNeeded(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl41.isLevelComplete || currentHeap == nullptr) return false;

    int parentIdx = lvl41.currentSiftIndex;
    int parentVal = currentHeap->get(parentIdx);
    int leftIdx = 2 * parentIdx + 1;
    int rightIdx = 2 * parentIdx + 2;
    int n = currentHeap->getSize();

    bool childIsLarger = false;
    if (leftIdx < n && currentHeap->get(leftIdx) > parentVal) childIsLarger = true;
    if (rightIdx < n && currentHeap->get(rightIdx) > parentVal) childIsLarger = true;

    if (childIsLarger) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "A child is greater than the parent. Select the largest child. (+1 Alarm)");
        return false;
    }

    score.addAlarmStep(1);
    advanceAfterSiftComplete(score, feedback, maxLen);
    return true;
}

// ============================================================================
// LEVEL 4.2 IMPLEMENTATION: REPAIR THE SECURITY LOCKS
// ============================================================================

bool Act4Controller::handleLevel42NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl42.isLevelComplete || currentHeap == nullptr) return false;

    if (lvl42.round == 1) {
        // Round A: Sift-Down
        int parentIdx = lvl42.currentSiftIndex;
        int parentVal = currentHeap->get(parentIdx);
        int leftIdx = 2 * parentIdx + 1;
        int rightIdx = 2 * parentIdx + 2;
        int n = currentHeap->getSize();

        bool isLeft = (leftIdx < n && clickedIndex == leftIdx);
        bool isRight = (rightIdx < n && clickedIndex == rightIdx);

        if (!isLeft && !isRight) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Select a child of the disturbed node [%d]. (+1 Alarm)", parentIdx);
            return false;
        }

        int largestChild = leftIdx;
        if (rightIdx < n && currentHeap->get(rightIdx) > currentHeap->get(leftIdx)) {
            largestChild = rightIdx;
        }

        if (clickedIndex != largestChild) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Choose the larger child. (+1 Alarm)");
            return false;
        }

        int childVal = currentHeap->get(clickedIndex);
        if (childVal <= parentVal) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "No repair needed here. (+1 Alarm)");
            return false;
        }

        lvl42.selectedChildIndex = clickedIndex;
        snprintf(lvl42.proposedSwapText, sizeof(lvl42.proposedSwapText), "Swap %d <-> %d", parentVal, childVal);
        snprintf(feedback, maxLen, "Selected Child [%d] (value %d). Click 'Confirm Swap'.", clickedIndex, childVal);
        return true;
    }

    return false;
}

bool Act4Controller::handleLevel42ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl42.isLevelComplete || currentHeap == nullptr) return false;

    if (lvl42.round == 1) {
        if (lvl42.selectedChildIndex == -1) {
            snprintf(feedback, maxLen, "Select the larger child first.");
            return false;
        }

        int pIdx = lvl42.currentSiftIndex;
        int cIdx = lvl42.selectedChildIndex;
        int pVal = currentHeap->get(pIdx);
        int cVal = currentHeap->get(cIdx);

        currentHeap->swapElements(pIdx, cIdx);
        score.addAlarmStep(1);

        lvl42.selectedChildIndex = -1;
        lvl42.proposedSwapText[0] = '\0';
        lvl42.currentSiftIndex = cIdx;

        int n = currentHeap->getSize();
        int newLeft = 2 * cIdx + 1;
        int newRight = 2 * cIdx + 2;

        if (newLeft >= n) {
            // Round A Complete! Transition to Round B: Sift-up
            lvl42.round = 2;
            currentHeap->appendRaw(12); // Append new treasure 12 at index 5
            lvl42.currentUpChildIndex = 5;
            lvl42.currentUpParentIndex = (5 - 1) / 2; // Index 2 (value 3)
            snprintf(feedback, maxLen, "ROUND A COMPLETE! Round B (Sift-Up): New treasure 12 appended at leaf [5]. 12 > parent 3. Click 'Confirm Move Up'.");
            return true;
        }

        int largestChild = newLeft;
        if (newRight < n && currentHeap->get(newRight) > currentHeap->get(newLeft)) {
            largestChild = newRight;
        }

        if (currentHeap->get(largestChild) > currentHeap->get(lvl42.currentSiftIndex)) {
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Value %d at [%d] still violates heap: Child [%d] (value %d) is larger.",
                     pVal, cVal, pVal, cIdx, largestChild, currentHeap->get(largestChild));
        } else {
            // Round A Complete! Transition to Round B
            lvl42.round = 2;
            currentHeap->appendRaw(12);
            lvl42.currentUpChildIndex = 5;
            lvl42.currentUpParentIndex = (5 - 1) / 2;
            snprintf(feedback, maxLen, "ROUND A COMPLETE! Round B (Sift-Up): New treasure 12 appended at leaf [5]. 12 > parent 3. Click 'Confirm Move Up'.");
        }
        return true;
    }

    return false;
}

bool Act4Controller::handleLevel42MoveUp(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl42.isLevelComplete || currentHeap == nullptr || lvl42.round != 2) return false;

    int childIdx = lvl42.currentUpChildIndex;
    int parentIdx = lvl42.currentUpParentIndex;

    if (childIdx <= 0 || parentIdx < 0) {
        snprintf(feedback, maxLen, "Element has already reached the root.");
        return false;
    }

    int childVal = currentHeap->get(childIdx);
    int parentVal = currentHeap->get(parentIdx);

    if (childVal <= parentVal) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Child is not greater than parent. Move up not required. (+1 Alarm)");
        return false;
    }

    // Perform swap in heap model
    currentHeap->swapElements(childIdx, parentIdx);
    score.addAlarmStep(1);

    // New child index is the former parent index
    lvl42.currentUpChildIndex = parentIdx;
    if (parentIdx == 0) {
        // Reached root!
        if (currentHeap->isMaxHeap()) {
            lvl42.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Security locks repaired! Final heap invariant verified.");
        } else {
            snprintf(feedback, maxLen, "Reached root, but invariant check failed.");
        }
        return true;
    }

    // Next parent
    lvl42.currentUpParentIndex = (parentIdx - 1) / 2;
    int newPVal = currentHeap->get(lvl42.currentUpParentIndex);

    if (childVal > newPVal) {
        snprintf(feedback, maxLen, "Moved up! Treasure %d at [%d] is larger than parent %d at [%d]. Click 'Confirm Move Up'.",
                 childVal, parentIdx, newPVal, lvl42.currentUpParentIndex);
    } else {
        if (currentHeap->isMaxHeap()) {
            lvl42.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Security locks repaired! Final heap invariant verified.");
        }
    }
    return true;
}

// ============================================================================
// LEVEL 4.3 IMPLEMENTATION: EXTRACT THE GREATEST TREASURE
// ============================================================================

bool Act4Controller::handleLevel43ExtractMax(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl43.isLevelComplete || currentHeap == nullptr) return false;

    if (lvl43.subPhase != 0) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Cannot extract while sift-down is in progress! Repair active heap first. (+1 Alarm)");
        return false;
    }

    if (lvl43.activeHeapSize <= 1) {
        snprintf(feedback, maxLen, "Only 1 active element remains; it is already in sorted position.");
        return false;
    }

    // Swap root (index 0) with last active element (activeHeapSize - 1)
    int lastIdx = lvl43.activeHeapSize - 1;
    int maxVal = currentHeap->get(0);
    int promoVal = currentHeap->get(lastIdx);

    currentHeap->swapElements(0, lastIdx);
    lvl43.activeHeapSize--;
    lvl43.extractionCount++;
    score.addAlarmStep(1);

    if (lvl43.activeHeapSize <= 1) {
        if (currentHeap->isSortedAscending(lvl43.totalElements)) {
            lvl43.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Extracted %d! Final array is fully sorted in ascending order.", maxVal);
        } else {
            snprintf(feedback, maxLen, "All extractions finished, but ascending order check failed.");
        }
        return true;
    }

    // Check if new root needs sift down within activeHeapSize
    int leftIdx = 1;
    int rightIdx = 2;
    int largest = 0;
    if (leftIdx < lvl43.activeHeapSize && currentHeap->get(leftIdx) > currentHeap->get(largest)) {
        largest = leftIdx;
    }
    if (rightIdx < lvl43.activeHeapSize && currentHeap->get(rightIdx) > currentHeap->get(largest)) {
        largest = rightIdx;
    }

    if (largest != 0) {
        lvl43.subPhase = 1; // SIFTING_DOWN
        lvl43.currentSiftIndex = 0;
        lvl43.selectedChildIndex = -1;
        snprintf(feedback, maxLen, "Extracted %d (moved to sorted tray). Promoted %d to root. Select larger child to sift down.",
                 maxVal, promoVal);
    } else {
        lvl43.subPhase = 0; // Already valid
        snprintf(feedback, maxLen, "Extracted %d. Promoted %d satisfies heap invariant! Ready for next extraction.",
                 maxVal, promoVal);
    }
    return true;
}

bool Act4Controller::handleLevel43NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl43.isLevelComplete || currentHeap == nullptr || lvl43.subPhase != 1) return false;

    int parentIdx = lvl43.currentSiftIndex;
    int leftIdx = 2 * parentIdx + 1;
    int rightIdx = 2 * parentIdx + 2;
    int n = lvl43.activeHeapSize; // Must be within active heap

    bool isLeft = (leftIdx < n && clickedIndex == leftIdx);
    bool isRight = (rightIdx < n && clickedIndex == rightIdx);

    if (!isLeft && !isRight) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Select a valid active child of sifting pod [%d]. (+1 Alarm)", parentIdx);
        return false;
    }

    int largestChild = leftIdx;
    if (rightIdx < n && currentHeap->get(rightIdx) > currentHeap->get(leftIdx)) {
        largestChild = rightIdx;
    }

    if (clickedIndex != largestChild) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Choose the larger child. (+1 Alarm)");
        return false;
    }

    lvl43.selectedChildIndex = clickedIndex;
    int pVal = currentHeap->get(parentIdx);
    int cVal = currentHeap->get(clickedIndex);
    snprintf(lvl43.proposedSwapText, sizeof(lvl43.proposedSwapText), "Swap %d <-> %d", pVal, cVal);
    snprintf(feedback, maxLen, "Selected Child [%d] (value %d). Click 'Confirm Swap'.", clickedIndex, cVal);
    return true;
}

bool Act4Controller::handleLevel43ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl43.isLevelComplete || currentHeap == nullptr || lvl43.subPhase != 1) return false;

    if (lvl43.selectedChildIndex == -1) {
        snprintf(feedback, maxLen, "Select the larger child first.");
        return false;
    }

    int pIdx = lvl43.currentSiftIndex;
    int cIdx = lvl43.selectedChildIndex;
    int pVal = currentHeap->get(pIdx);
    int cVal = currentHeap->get(cIdx);

    currentHeap->swapElements(pIdx, cIdx);
    score.addAlarmStep(1);

    lvl43.selectedChildIndex = -1;
    lvl43.proposedSwapText[0] = '\0';
    lvl43.currentSiftIndex = cIdx;

    int n = lvl43.activeHeapSize;
    int newLeft = 2 * cIdx + 1;
    int newRight = 2 * cIdx + 2;

    if (newLeft >= n) {
        // Reached leaf of active heap; sift-down complete!
        lvl43.subPhase = 0;
        snprintf(feedback, maxLen, "Swapped %d <-> %d! Sift-down complete. Active heap restored! Click 'Extract Maximum'.", pVal, cVal);
        return true;
    }

    int largestChild = newLeft;
    if (newRight < n && currentHeap->get(newRight) > currentHeap->get(newLeft)) {
        largestChild = newRight;
    }

    if (currentHeap->get(largestChild) > currentHeap->get(lvl43.currentSiftIndex)) {
        snprintf(feedback, maxLen, "Swapped %d <-> %d! Continue sift-down: Child [%d] (value %d) is larger.",
                 pVal, cVal, largestChild, currentHeap->get(largestChild));
    } else {
        lvl43.subPhase = 0;
        snprintf(feedback, maxLen, "Swapped %d <-> %d! Sift-down complete. Active heap restored! Click 'Extract Maximum'.", pVal, cVal);
    }
    return true;
}

// ============================================================================
// LEVEL 4.4 IMPLEMENTATION: RACE AGAINST THE GUARDS
// ============================================================================

bool Act4Controller::handleLevel44ExtractMax(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl44.isLevelComplete || currentHeap == nullptr) return false;

    if (lvl44.subPhase != 0) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Sift-down in progress! Repair active heap first. (+1 Alarm)");
        return false;
    }

    if (lvl44.completedExtractions >= lvl44.extractionRoundTarget) {
        snprintf(feedback, maxLen, "Target extractions already completed!");
        return false;
    }

    int lastIdx = lvl44.activeHeapSize - 1;
    int maxVal = currentHeap->get(0);
    int promoVal = currentHeap->get(lastIdx);

    currentHeap->swapElements(0, lastIdx);
    lvl44.activeHeapSize--;
    lvl44.completedExtractions++;
    score.addAlarmStep(1);

    // Check if new root needs sift down within activeHeapSize
    int leftIdx = 1;
    int rightIdx = 2;
    int largest = 0;
    if (leftIdx < lvl44.activeHeapSize && currentHeap->get(leftIdx) > currentHeap->get(largest)) {
        largest = leftIdx;
    }
    if (rightIdx < lvl44.activeHeapSize && currentHeap->get(rightIdx) > currentHeap->get(largest)) {
        largest = rightIdx;
    }

    if (largest != 0) {
        lvl44.subPhase = 1;
        lvl44.currentSiftIndex = 0;
        lvl44.selectedChildIndex = -1;
        snprintf(feedback, maxLen, "Extracted %d (promoted %d)! Extraction %d/%d. Select larger child to repair heap.",
                 maxVal, promoVal, lvl44.completedExtractions, lvl44.extractionRoundTarget);
    } else {
        lvl44.subPhase = 0;
        if (lvl44.completedExtractions >= lvl44.extractionRoundTarget) {
            lvl44.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Guards evaded! 4 extractions complete.\nComplexity: Build O(N) | Sift O(log N) | Full Sort O(N log N).");
        } else {
            snprintf(feedback, maxLen, "Extracted %d! Extraction %d/%d. Ready for next extraction.",
                     maxVal, lvl44.completedExtractions, lvl44.extractionRoundTarget);
        }
    }
    return true;
}

bool Act4Controller::handleLevel44NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl44.isLevelComplete || currentHeap == nullptr || lvl44.subPhase != 1) return false;

    int parentIdx = lvl44.currentSiftIndex;
    int leftIdx = 2 * parentIdx + 1;
    int rightIdx = 2 * parentIdx + 2;
    int n = lvl44.activeHeapSize;

    bool isLeft = (leftIdx < n && clickedIndex == leftIdx);
    bool isRight = (rightIdx < n && clickedIndex == rightIdx);

    if (!isLeft && !isRight) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Select a valid active child of sifting pod [%d]. (+1 Alarm)", parentIdx);
        return false;
    }

    int largestChild = leftIdx;
    if (rightIdx < n && currentHeap->get(rightIdx) > currentHeap->get(leftIdx)) {
        largestChild = rightIdx;
    }

    if (clickedIndex != largestChild) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Choose the larger child. (+1 Alarm)");
        return false;
    }

    lvl44.selectedChildIndex = clickedIndex;
    int pVal = currentHeap->get(parentIdx);
    int cVal = currentHeap->get(clickedIndex);
    snprintf(lvl44.proposedSwapText, sizeof(lvl44.proposedSwapText), "Swap %d <-> %d", pVal, cVal);
    snprintf(feedback, maxLen, "Selected Child [%d] (value %d). Click 'Confirm Swap'.", clickedIndex, cVal);
    return true;
}

bool Act4Controller::handleLevel44ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl44.isLevelComplete || currentHeap == nullptr || lvl44.subPhase != 1) return false;

    if (lvl44.selectedChildIndex == -1) {
        snprintf(feedback, maxLen, "Select the larger child first.");
        return false;
    }

    int pIdx = lvl44.currentSiftIndex;
    int cIdx = lvl44.selectedChildIndex;
    int pVal = currentHeap->get(pIdx);
    int cVal = currentHeap->get(cIdx);

    currentHeap->swapElements(pIdx, cIdx);
    score.addAlarmStep(1);

    lvl44.selectedChildIndex = -1;
    lvl44.proposedSwapText[0] = '\0';
    lvl44.currentSiftIndex = cIdx;

    int n = lvl44.activeHeapSize;
    int newLeft = 2 * cIdx + 1;
    int newRight = 2 * cIdx + 2;

    if (newLeft >= n) {
        lvl44.subPhase = 0;
        if (lvl44.completedExtractions >= lvl44.extractionRoundTarget) {
            lvl44.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Guards evaded! 4 extractions complete.\nComplexity: Build O(N) | Sift O(log N) | Full Sort O(N log N).");
        } else {
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Active heap restored. Click 'Extract Maximum'.", pVal, cVal);
        }
        return true;
    }

    int largestChild = newLeft;
    if (newRight < n && currentHeap->get(newRight) > currentHeap->get(newLeft)) {
        largestChild = newRight;
    }

    if (currentHeap->get(largestChild) > currentHeap->get(lvl44.currentSiftIndex)) {
        snprintf(feedback, maxLen, "Swapped %d <-> %d! Continue sift-down: Child [%d] (value %d) is larger.",
                 pVal, cVal, largestChild, currentHeap->get(largestChild));
    } else {
        lvl44.subPhase = 0;
        if (lvl44.completedExtractions >= lvl44.extractionRoundTarget) {
            lvl44.isLevelComplete = true;
            snprintf(feedback, maxLen, "LEVEL COMPLETE: Guards evaded! 4 extractions complete.\nComplexity: Build O(N) | Sift O(log N) | Full Sort O(N log N).");
        } else {
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Active heap restored. Click 'Extract Maximum'.", pVal, cVal);
        }
    }
    return true;
}

// ============================================================================
// LEVEL 4.5 IMPLEMENTATION: FINAL VAULT BOSS (HEAP SORT)
// ============================================================================

void Act4Controller::advance45AfterSiftComplete(ScoreSystem& score, char* feedback, int maxLen) {
    lvl45.currentParentIndex--;
    if (lvl45.currentParentIndex < 0) {
        // Bottom-up construction finished!
        if (currentHeap->isMaxHeap()) {
            strncpy(lvl45.phase, "sort", sizeof(lvl45.phase) - 1);
            lvl45.subPhase = 0;
            lvl45.activeHeapSize = lvl45.totalElements;
            lvl45.completedExtractions = 0;
            lvl45.selectedChildIndex = -1;
            lvl45.proposedSwapText[0] = '\0';
            snprintf(feedback, maxLen, "PHASE A COMPLETE: Max Heap constructed! Proceed to Phase B: Click 'Extract Maximum'.");
        } else {
            snprintf(feedback, maxLen, "Build complete, but Max Heap invariant check failed.");
        }
    } else {
        lvl45.currentSiftIndex = lvl45.currentParentIndex;
        lvl45.selectedChildIndex = -1;
        lvl45.proposedSwapText[0] = '\0';
        snprintf(feedback, maxLen, "Parent verified. Moving to parent at index [%d] (value %d).",
                 lvl45.currentParentIndex, currentHeap->get(lvl45.currentParentIndex));
    }
}

bool Act4Controller::handleLevel45NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl45.isLevelComplete || currentHeap == nullptr) return false;

    if (strcmp(lvl45.phase, "build") == 0) {
        int parentIdx = lvl45.currentSiftIndex;
        int parentVal = currentHeap->get(parentIdx);
        int leftIdx = 2 * parentIdx + 1;
        int rightIdx = 2 * parentIdx + 2;
        int n = lvl45.totalElements;

        bool isLeft = (leftIdx < n && clickedIndex == leftIdx);
        bool isRight = (rightIdx < n && clickedIndex == rightIdx);

        if (!isLeft && !isRight) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Select a child of the highlighted parent [%d]. (+1 Alarm)", parentIdx);
            return false;
        }

        int largestChild = leftIdx;
        if (rightIdx < n && currentHeap->get(rightIdx) > currentHeap->get(leftIdx)) {
            largestChild = rightIdx;
        }

        if (clickedIndex != largestChild) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Choose the larger child. (+1 Alarm)");
            return false;
        }

        int childVal = currentHeap->get(clickedIndex);
        if (childVal <= parentVal) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Parent already satisfies heap property. Choose 'No Swap Needed'. (+1 Alarm)");
            return false;
        }

        lvl45.selectedChildIndex = clickedIndex;
        snprintf(lvl45.proposedSwapText, sizeof(lvl45.proposedSwapText), "Swap %d <-> %d", parentVal, childVal);
        snprintf(feedback, maxLen, "Selected Child [%d] (value %d). Click 'Confirm Swap'.", clickedIndex, childVal);
        return true;
    } else if (strcmp(lvl45.phase, "sort") == 0 && lvl45.subPhase == 1) {
        // Sifting down new root during Heap Sort
        int parentIdx = lvl45.currentSiftIndex;
        int leftIdx = 2 * parentIdx + 1;
        int rightIdx = 2 * parentIdx + 2;
        int n = lvl45.activeHeapSize;

        bool isLeft = (leftIdx < n && clickedIndex == leftIdx);
        bool isRight = (rightIdx < n && clickedIndex == rightIdx);

        if (!isLeft && !isRight) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Select a valid active child of sifting pod [%d]. (+1 Alarm)", parentIdx);
            return false;
        }

        int largestChild = leftIdx;
        if (rightIdx < n && currentHeap->get(rightIdx) > currentHeap->get(leftIdx)) {
            largestChild = rightIdx;
        }

        if (clickedIndex != largestChild) {
            score.addAlarmStep(1);
            snprintf(feedback, maxLen, "Choose the larger child. (+1 Alarm)");
            return false;
        }

        lvl45.selectedChildIndex = clickedIndex;
        int pVal = currentHeap->get(parentIdx);
        int cVal = currentHeap->get(clickedIndex);
        snprintf(lvl45.proposedSwapText, sizeof(lvl45.proposedSwapText), "Swap %d <-> %d", pVal, cVal);
        snprintf(feedback, maxLen, "Selected Child [%d] (value %d). Click 'Confirm Swap'.", clickedIndex, cVal);
        return true;
    }

    return false;
}

bool Act4Controller::handleLevel45ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl45.isLevelComplete || currentHeap == nullptr) return false;

    if (strcmp(lvl45.phase, "build") == 0) {
        if (lvl45.selectedChildIndex == -1) {
            snprintf(feedback, maxLen, "Select the larger child first.");
            return false;
        }

        int pIdx = lvl45.currentSiftIndex;
        int cIdx = lvl45.selectedChildIndex;
        int pVal = currentHeap->get(pIdx);
        int cVal = currentHeap->get(cIdx);

        currentHeap->swapElements(pIdx, cIdx);
        score.addAlarmStep(1);

        lvl45.selectedChildIndex = -1;
        lvl45.proposedSwapText[0] = '\0';
        lvl45.currentSiftIndex = cIdx;

        int n = lvl45.totalElements;
        int newLeft = 2 * cIdx + 1;
        int newRight = 2 * cIdx + 2;

        if (newLeft >= n) {
            advance45AfterSiftComplete(score, feedback, maxLen);
            return true;
        }

        int largestChild = newLeft;
        if (newRight < n && currentHeap->get(newRight) > currentHeap->get(newLeft)) {
            largestChild = newRight;
        }

        if (currentHeap->get(largestChild) > currentHeap->get(lvl45.currentSiftIndex)) {
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Continue sift-down at index [%d]: Child [%d] (value %d) is larger.",
                     pVal, cVal, cIdx, largestChild, currentHeap->get(largestChild));
        } else {
            advance45AfterSiftComplete(score, feedback, maxLen);
        }
        return true;
    } else if (strcmp(lvl45.phase, "sort") == 0 && lvl45.subPhase == 1) {
        if (lvl45.selectedChildIndex == -1) {
            snprintf(feedback, maxLen, "Select the larger child first.");
            return false;
        }

        int pIdx = lvl45.currentSiftIndex;
        int cIdx = lvl45.selectedChildIndex;
        int pVal = currentHeap->get(pIdx);
        int cVal = currentHeap->get(cIdx);

        currentHeap->swapElements(pIdx, cIdx);
        score.addAlarmStep(1);

        lvl45.selectedChildIndex = -1;
        lvl45.proposedSwapText[0] = '\0';
        lvl45.currentSiftIndex = cIdx;

        int n = lvl45.activeHeapSize;
        int newLeft = 2 * cIdx + 1;
        int newRight = 2 * cIdx + 2;

        if (newLeft >= n) {
            lvl45.subPhase = 0;
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Active heap restored. Click 'Extract Maximum'.", pVal, cVal);
            return true;
        }

        int largestChild = newLeft;
        if (newRight < n && currentHeap->get(newRight) > currentHeap->get(newLeft)) {
            largestChild = newRight;
        }

        if (currentHeap->get(largestChild) > currentHeap->get(lvl45.currentSiftIndex)) {
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Continue sift-down: Child [%d] (value %d) is larger.",
                     pVal, cVal, largestChild, currentHeap->get(largestChild));
        } else {
            lvl45.subPhase = 0;
            snprintf(feedback, maxLen, "Swapped %d <-> %d! Active heap restored. Click 'Extract Maximum'.", pVal, cVal);
        }
        return true;
    }

    return false;
}

bool Act4Controller::handleLevel45NoSwapNeeded(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl45.isLevelComplete || currentHeap == nullptr || strcmp(lvl45.phase, "build") != 0) return false;

    int parentIdx = lvl45.currentSiftIndex;
    int parentVal = currentHeap->get(parentIdx);
    int leftIdx = 2 * parentIdx + 1;
    int rightIdx = 2 * parentIdx + 2;
    int n = lvl45.totalElements;

    bool childIsLarger = false;
    if (leftIdx < n && currentHeap->get(leftIdx) > parentVal) childIsLarger = true;
    if (rightIdx < n && currentHeap->get(rightIdx) > parentVal) childIsLarger = true;

    if (childIsLarger) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "A child is greater than the parent. Select the largest child. (+1 Alarm)");
        return false;
    }

    score.addAlarmStep(1);
    advance45AfterSiftComplete(score, feedback, maxLen);
    return true;
}

bool Act4Controller::handleLevel45ExtractMax(ScoreSystem& score, char* feedback, int maxLen) {
    if (lvl45.isLevelComplete || currentHeap == nullptr || strcmp(lvl45.phase, "sort") != 0) return false;

    if (lvl45.subPhase != 0) {
        score.addAlarmStep(1);
        snprintf(feedback, maxLen, "Sift-down in progress! Repair active heap first. (+1 Alarm)");
        return false;
    }

    if (lvl45.activeHeapSize <= 1) {
        snprintf(feedback, maxLen, "Only 1 active element remains.");
        return false;
    }

    int lastIdx = lvl45.activeHeapSize - 1;
    int maxVal = currentHeap->get(0);
    int promoVal = currentHeap->get(lastIdx);

    currentHeap->swapElements(0, lastIdx);
    lvl45.activeHeapSize--;
    lvl45.completedExtractions++;
    score.addAlarmStep(1);

    if (lvl45.activeHeapSize <= 1) {
        // Phase C: Validate
        strncpy(lvl45.phase, "validate", sizeof(lvl45.phase) - 1);
        if (currentHeap->isSortedAscending(lvl45.totalElements)) {
            lvl45.isLevelComplete = true;
            snprintf(feedback, maxLen, "FINAL VAULT UNLOCKED: Complete Heap Sort verified! All elements sorted ascending. Achievement: Final Vault Unlocked!");
        } else {
            snprintf(feedback, maxLen, "Sort finished, but ascending order check failed.");
        }
        return true;
    }

    // Check if new root needs sift down within activeHeapSize
    int leftIdx = 1;
    int rightIdx = 2;
    int largest = 0;
    if (leftIdx < lvl45.activeHeapSize && currentHeap->get(leftIdx) > currentHeap->get(largest)) {
        largest = leftIdx;
    }
    if (rightIdx < lvl45.activeHeapSize && currentHeap->get(rightIdx) > currentHeap->get(largest)) {
        largest = rightIdx;
    }

    if (largest != 0) {
        lvl45.subPhase = 1;
        lvl45.currentSiftIndex = 0;
        lvl45.selectedChildIndex = -1;
        snprintf(feedback, maxLen, "Extracted %d! Promoted %d. Select larger child to sift down.",
                 maxVal, promoVal);
    } else {
        lvl45.subPhase = 0;
        snprintf(feedback, maxLen, "Extracted %d! Promoted %d satisfies active heap. Ready for next extraction.",
                 maxVal, promoVal);
    }
    return true;
}
