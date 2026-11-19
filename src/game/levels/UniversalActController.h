#ifndef UNIVERSAL_ACT_CONTROLLER_H
#define UNIVERSAL_ACT_CONTROLLER_H

#include "../scoring/ScoreSystem.h"
#include "LevelDef.h"
#include "../../structures/bst/BST.h"
#include "../../structures/avl/avl_tree.h"
#include "../../structures/heap/max_heap.h"

// Universal controller for Acts 1‑4. It selects the appropriate data structure
// based on the LevelDef::structureType field and executes the required action
// (requiredAction). The implementation focuses on the BST workflow needed for
// Act 2; other acts are scaffolded for future extension.

class UniversalActController {
public:
    UniversalActController();
    ~UniversalActController();

    // Initialise the controller for the supplied level. Stores pointers to the
    // level definition and the ScoreSystem used by the game.
    void initLevel(const LevelDef& lvl, ScoreSystem& sc);

    // Handle a UI‑driven action. "action" is a string identifier (e.g.
    // "search", "insert", "delete", "range", "boss_insert", "boss_search").
    // "value" carries the integer payload required for the action (target key,
    // insertion value, low/high bound, etc.). Returns true on a valid action.
    bool handleAction(const char* action, int value, char* feedback, int maxLen);

    // Accessors used by the rendering layer – primarily for the BST visualizer
    // side‑car.
    BST* getBST() const { return bst; }
private:
    // Act‑2 specific helpers.
    bool act2Search(int target, char* fb, int maxLen);
    bool act2Insert(int key, char* fb, int maxLen);
    bool act2Delete(int key, char* fb, int maxLen);
    bool act2Range(int lo, int hi, char* fb, int maxLen);
    bool act2BossInsert(int key, char* fb, int maxLen);
    bool act2BossSearch(int target, char* fb, int maxLen);
    // Generic helpers.
    void recordAlarm(int steps = 1);
    void recordPenalty(int penaltyScore = 0, int alarmPenalty = 1);
    const LevelDef* currentLevel = nullptr;
    ScoreSystem* scoreSystem = nullptr;
    // One data structure is active at a time.
    BST* bst = nullptr;
    AVLTree* avl = nullptr;
    MaxHeap* heap = nullptr;
};

#endif // UNIVERSAL_ACT_CONTROLLER_H
