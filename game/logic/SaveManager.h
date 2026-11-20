#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "../controller/LevelDef.h"
#include "binary_tree/BinaryTree.h"

struct EndlessSaveData {
    int wave;
    int score;
    int seed;
    int keyCount;
    int keys[32];

    EndlessSaveData() {
        wave = 1;
        score = 0;
        seed = 0;
        keyCount = 0;
        for (int i = 0; i < 32; i++) keys[i] = 0;
    }
};

class SaveManager {
public:
    static const int TOTAL_LEVELS = 20;

    static bool saveProgress(const char* filepath, const LevelDef levels[], int levelCount);
    static bool loadProgress(const char* filepath, LevelDef levels[], int levelCount);

    static bool serializeTree(TreeNode* node, char* buffer, int maxLen);
    static TreeNode* deserializeTree(const char* buffer);

    static bool serializeHeap(const int* arr, int n, char* buffer, int maxLen);
    static bool deserializeHeap(const char* buffer, int* arr, int& n, int maxN);

    static bool saveEndless(const char* filepath, const EndlessSaveData& data);
    static bool loadEndless(const char* filepath, EndlessSaveData& data);
};

#endif


