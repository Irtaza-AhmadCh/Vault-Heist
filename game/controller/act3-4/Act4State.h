#ifndef ACT4_STATE_H
#define ACT4_STATE_H

#include "../../logic/heap/max_heap.h"
#include "../../scoring/ScoreSystem.h"
#include "../LevelDef.h"
#include <cstring>
#include <cstdio>

// ============================================================================
// LEVEL 4.1: ACTIVATE THE VAULT (BOTTOM-UP HEAP CONSTRUCTION)
// ============================================================================
struct Level41State {
    int initialArray[16];
    int heapSize;

    int currentParentIndex;     // Current parent being processed bottom-up (1 -> 0)
    int currentSiftIndex;       // Position of the element currently sifting down
    int selectedChildIndex;     // Selected child index (-1 if none selected)

    char statusMsg[160];
    char proposedSwapText[64];
    bool isLevelComplete;
};

// ============================================================================
// LEVEL 4.2: REPAIR THE SECURITY LOCKS (SIFT-DOWN ROUND A & SIFT-UP ROUND B)
// ============================================================================
struct Level42State {
    int round;                  // 1 = Round A (sift-down), 2 = Round B (sift-up)
    int initialArray[16];
    int heapSize;

    // Round A (Sift-down)
    int currentSiftIndex;       // Starting at 0
    int selectedChildIndex;     // -1 if none

    // Round B (Sift-up)
    int currentUpChildIndex;    // Starts at 5 (value 12)
    int currentUpParentIndex;   // (5-1)/2 = 2

    char statusMsg[160];
    char proposedSwapText[64];
    bool isLevelComplete;
};

// ============================================================================
// LEVEL 4.3: EXTRACT THE GREATEST TREASURE (EXTRACT-MAX & HEAPIFY)
// ============================================================================
struct Level43State {
    int initialArray[16];
    int totalElements;          // 5
    int activeHeapSize;         // 5 -> 4 -> 3 -> 2 -> 1
    int extractionCount;        // 0 -> 4

    int subPhase;               // 0 = WAIT_EXTRACT, 1 = SIFTING_DOWN
    int currentSiftIndex;
    int selectedChildIndex;

    char statusMsg[160];
    char proposedSwapText[64];
    bool isLevelComplete;
};

// ============================================================================
// LEVEL 4.4: RACE AGAINST THE GUARDS (MULTI-ROUND EXTRACTION UNDER PRESSURE)
// ============================================================================
struct Level44State {
    int initialArray[16];
    int totalElements;          // 6
    int activeHeapSize;         // 6
    int extractionRoundTarget;  // 4
    int completedExtractions;   // 0 -> 4

    int subPhase;               // 0 = WAIT_EXTRACT, 1 = SIFTING_DOWN
    int currentSiftIndex;
    int selectedChildIndex;

    char statusMsg[160];
    char proposedSwapText[64];
    bool isLevelComplete;
};

// ============================================================================
// LEVEL 4.5: FINAL VAULT BOSS (COMPLETE INDEPENDENT HEAP SORT)
// ============================================================================
struct Level45State {
    int initialArray[16];
    int totalElements;          // 7
    int activeHeapSize;         // 7

    char phase[16];             // "build", "sort", "validate"

    // Phase A (Build Heap)
    int currentParentIndex;     // Starts at 2
    int currentSiftIndex;       // For downward sift during build
    int selectedChildIndex;

    // Phase B (Heap Sort extractions)
    int subPhase;               // 0 = WAIT_EXTRACT, 1 = SIFTING_DOWN
    int completedExtractions;   // 0 -> 6

    char statusMsg[160];
    char proposedSwapText[64];
    bool isLevelComplete;
};

// ============================================================================
// ACT 4 CONTROLLER
// ============================================================================
class Act4Controller {
public:
    int activeLevelNumber;
    Level41State lvl41;
    Level42State lvl42;
    Level43State lvl43;
    Level44State lvl44;
    Level45State lvl45;

    MaxHeap* currentHeap;

    Act4Controller();

    void initLevel(int levelNum, MaxHeap* heapModel, ScoreSystem& score, const LevelDef* lvlDef = nullptr);
    void cancelSelection();

    // Level 4.1 methods
    bool handleLevel41NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel41ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel41NoSwapNeeded(ScoreSystem& score, char* feedback, int maxLen);

    // Level 4.2 methods
    bool handleLevel42NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel42ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel42MoveUp(ScoreSystem& score, char* feedback, int maxLen);

    // Level 4.3 methods
    bool handleLevel43ExtractMax(ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel43NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel43ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen);

    // Level 4.4 methods
    bool handleLevel44ExtractMax(ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel44NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel44ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen);

    // Level 4.5 methods
    bool handleLevel45NodeClick(int clickedIndex, ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel45ConfirmSwap(ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel45NoSwapNeeded(ScoreSystem& score, char* feedback, int maxLen);
    bool handleLevel45ExtractMax(ScoreSystem& score, char* feedback, int maxLen);

private:
    void advanceAfterSiftComplete(ScoreSystem& score, char* feedback, int maxLen);
    void advance45AfterSiftComplete(ScoreSystem& score, char* feedback, int maxLen);
};

#endif // ACT4_STATE_H
