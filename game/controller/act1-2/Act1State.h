#ifndef ACT1_STATE_H
#define ACT1_STATE_H

#include "../../../structures/binary_tree/BinaryTree.h"
#include "../../scoring/ScoreSystem.h"

struct TraversalStage {
    char name[24];
    char displayName[48];
    int expectedValues[32];
    int expectedCount;
    int currentIndex;
    bool completed;
};

struct Level11State {
    static const int TOTAL_STAGES = 4;
    TraversalStage stages[TOTAL_STAGES];
    int currentStage;
    int visitedValues[32];
    int visitedCount;
    bool isLevelComplete;
};

struct ReconToken {
    int value;
    bool placed;
    float trayX, trayY;
};

struct ReconSlot {
    int slotIndex;
    int parentSlot;
    bool isLeft;
    bool isAvailable;
    bool isOccupied;
    int placedValue;
    int targetValue;
    float x;
    float y;
};

struct Level12State {
    static const int TOTAL_STAGES = 2;
    int currentStage;
    char stageName[32];

    int preSeq[16];
    int inSeq[16];
    int postSeq[16];
    int seqLen;

    ReconToken tokens[16];
    int tokenCount;

    ReconSlot slots[16];
    int slotCount;

    int selectedTokenIndex;
    bool isLevelComplete;
};

struct HeightNodeInfo {
    int value;
    int expectedHeight;
    int enteredHeight;
    bool isCalculated;
};

struct Level13State {
    char subState[24];
    int postorderValues[16];
    int expectedHeights[16];
    int enteredHeights[16];
    int nodeCount;
    int currentPostorderIdx;
    int activeValue;
    int expectedDiameter;
    int enteredDiameter;
    bool isLevelComplete;
};

struct Level14State {
    char phase[24];
    int nodeA;
    int nodeB;
    int expectedLCA;
    int selectedLCA;
    int expectedPath[16];
    int pathLength;
    int tracedPath[16];
    int tracedCount;
    int currentTraceIdx;
    bool isLevelComplete;
};

struct MirrorNodeStatus {
    int value;
    bool swapped;
};

struct Level15State {
    MirrorNodeStatus nodes[16];
    int nodeCount;
    bool isLevelComplete;
};

class Act1Controller {
public:
    int activeLevelNumber;
    Level11State lvl11;
    Level12State lvl12;
    Level13State lvl13;
    Level14State lvl14;
    Level15State lvl15;

    BinaryTree* currentTree;
    BinaryTree targetTree;

    Act1Controller();

    void initLevel(int levelNum, BinaryTree* tree, ScoreSystem& score);

    bool handleLevel11Click(int clickedValue, ScoreSystem& score, char* feedback, int maxLen);
    void loadStageTree(int stageIdx);

    bool selectToken(int tokenIdx);
    bool placeTokenInSlot(int slotIdx, ScoreSystem& score, char* feedback, int maxLen);
    bool checkReconstructionComplete(ScoreSystem& score);

    bool handleLevel13NodeClick(int clickedValue, ScoreSystem& score, char* feedback, int maxLen);
    bool submitLevel13Input(int inputVal, ScoreSystem& score, char* feedback, int maxLen);

    bool handleLevel14Click(int clickedValue, ScoreSystem& score, char* feedback, int maxLen);

    bool isNodeEligibleForSwap(int nodeVal);
    bool handleLevel15Swap(int nodeVal, ScoreSystem& score, char* feedback, int maxLen);
    bool handleAutoMirror(ScoreSystem& score, char* feedback, int maxLen);

private:
    bool isNodeSwapped(int val);
    void markNodeSwapped(int val);
};

#endif
