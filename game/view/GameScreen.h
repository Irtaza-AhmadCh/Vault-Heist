#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "../logic/binary_tree/BinaryTree.h"
#include "../logic/bst/bst.h"
#include "../logic/avl/avl_tree.h"
#include "../logic/heap/max_heap.h"
#include "../logic/event.h"

#include "visualization/tree/TreeLayout.h"
#include "visualization/tree/TreeRenderer.h"
#include "visualization/animation/EventAnimationController.h"
#include "UIComponents.h"

#include "../controller/LevelManager.h"
#include "../logic/SaveManager.h"
#include "../controller/UndoManager.h"
#include "../controller/act1-2/Act1State.h"
#include "../controller/act1-2/Act2State.h"
#include "../controller/act3-4/Act3State.h"
#include "../controller/act3-4/Act4State.h"

// Standardized string identifiers per guardrails:
// Screen states: "menu", "campaign_map", "level_play", "sandbox", "endless"
// Acts: "act_1", "act_2", "act_3", "act_4"

class GameScreen {
public:
    char screenState[16];
    char currentAct[16];
    int campaignSelectedAct;

    // Core Data Structures
    BinaryTree binTree;
    BST bst;
    AVLTree avl;
    MaxHeap heap;
    EventLog eventLog;

    // Visualization and Animation Engines
    TreeLayoutEngine layout;
    TreeRenderer renderer;
    EventAnimationController eventController;
    HUD hud;
    BioInputBox inputBox;

    // Phase 4 Game Systems
    LevelManager levelManager;
    UndoManager undoManager;
    Act1Controller act1;
    Act2Controller act2;
    Act3Controller act3;
    Act4Controller act4;

    static const int MAX_BUTTONS = 48;
    BioButton buttons[MAX_BUTTONS];
    int buttonCount;

    int selectedNodeIndex;
    bool isDraggingAVLNode;
    int draggedAVLNodeIndex;
    int draggedAVLNodeKey;
    int draggedAVLLevelNumber;
    float avlDragStartX;
    bool isAct3StageTransitionPending;
    int pendingAct3StageLevel;
    float act3StageTransitionTimer;

    // Camera view panning & zoom
    sf::View cameraView;
    bool isPanning;
    sf::Vector2i panStartPos;
    sf::Vector2f viewCenter;
    float zoomLevel;

    // Endless mode timer
    float endlessTimer;
    int endlessWave;

    GameScreen() : eventLog(128) {
        setScreenState("menu");
        setAct("act_1");
        campaignSelectedAct = 1;
        buttonCount = 0;
        selectedNodeIndex = -1;
        isDraggingAVLNode = false;
        draggedAVLNodeIndex = -1;
        draggedAVLNodeKey = -1;
        draggedAVLLevelNumber = 0;
        avlDragStartX = 0.0f;
        isAct3StageTransitionPending = false;
        pendingAct3StageLevel = 0;
        act3StageTransitionTimer = 0.0f;
        endlessTimer = 0.0f;
        endlessWave = 1;

        isPanning = false;
        viewCenter = sf::Vector2f(640.0f, 360.0f);
        zoomLevel = 1.0f;
        cameraView.setSize(1280.0f, 720.0f);
        cameraView.setCenter(viewCenter);

        inputBox = BioInputBox(410.0f, 58.0f, 50.0f, 28.0f, "50");
    }

    void setScreenState(const char* st) {
        if (st == nullptr) return;
        strncpy(screenState, st, sizeof(screenState) - 1);
        screenState[sizeof(screenState) - 1] = '\0';
    }

    bool isScreen(const char* st) const {
        if (st == nullptr) return false;
        return strcmp(screenState, st) == 0;
    }

    void setAct(const char* act) {
        if (act == nullptr) return;
        strncpy(currentAct, act, sizeof(currentAct) - 1);
        currentAct[sizeof(currentAct) - 1] = '\0';
    }

    bool isAct(const char* act) const {
        if (act == nullptr) return false;
        return strcmp(currentAct, act) == 0;
    }

    bool initialize(const char* assetsDir) {
        srand(static_cast<unsigned>(time(nullptr)));
        bool ok = renderer.loadAssets(assetsDir);

        // Load persistent savegame
        SaveManager::loadProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);

        setupDefaultAct1Tree();
        setupButtons();
        return ok;
    }

    void takeSnapshot() {
        if (isAct("act_1")) {
            // Snapshot of Binary Tree
            int keys[32];
            int n = 0;
            collectSnapshotKeys(binTree.root, keys, n);
            undoManager.pushSnapshot("tree", keys, n, levelManager.scoreSystem.alarmSteps, levelManager.scoreSystem.currentScore);
        } else if (isAct("act_2")) {
            int keys[32];
            int n = 0;
            collectBSTKeys(bst.getRoot(), keys, n);
            undoManager.pushSnapshot("bst", keys, n, levelManager.scoreSystem.alarmSteps, levelManager.scoreSystem.currentScore);
        } else if (isAct("act_3")) {
            int keys[32];
            int n = 0;
            collectAVLKeys(avl.getRoot(), keys, n);
            undoManager.pushSnapshot("avl", keys, n, levelManager.scoreSystem.alarmSteps, levelManager.scoreSystem.currentScore);
        } else if (isAct("act_4")) {
            undoManager.pushSnapshot("heap", heap.getArray(), heap.getSize(), levelManager.scoreSystem.alarmSteps, levelManager.scoreSystem.currentScore);
        }
    }

    void handleUndo() {
        TreeSnapshot snap;
        if (!undoManager.popSnapshot(snap)) {
            hud.setStatus("UNDO: No previous snapshots in buffer.");
            return;
        }

        if (strcmp(snap.structureType, "tree") == 0) {
            setupDefaultAct1Tree();
        } else if (strcmp(snap.structureType, "bst") == 0) {
            while (!bst.isEmpty()) bst.deleteNode(bst.getRoot()->key);
            for (int i = 0; i < snap.keyCount; i++) bst.insert(snap.keys[i]);
            syncBSTVisuals();
        } else if (strcmp(snap.structureType, "avl") == 0) {
            while (!avl.isEmpty()) avl.deleteNode(avl.getRoot()->key);
            for (int i = 0; i < snap.keyCount; i++) avl.insert(snap.keys[i]);
            syncAVLVisuals();
        } else if (strcmp(snap.structureType, "heap") == 0) {
            heap.buildMaxHeap(snap.keys, snap.keyCount);
            syncHeapVisuals();
        }

        levelManager.scoreSystem.alarmSteps = snap.alarmSteps;
        levelManager.scoreSystem.currentScore = snap.score;
        hud.setStatus("UNDO EXECUTED: Restored previous neural state.");
    }

    void handleHint() {
        if (!isScreen("level_play")) {
            hud.setStatus("HINT: Available during active campaign levels.");
            return;
        }
        LevelDef* lvl = levelManager.getCurrentLevel();
        if (lvl != nullptr) {
            levelManager.scoreSystem.useHint();
            char buf[160];
            snprintf(buf, sizeof(buf), "HINT: %s", lvl->hint);
            hud.setStatus(buf, "Hint used: -1 max star penalty applied.");
        }
    }

    void collectSnapshotKeys(TreeNode* node, int* arr, int& count) {
        if (node == nullptr || count >= 32) return;
        arr[count++] = node->data;
        collectSnapshotKeys(node->left, arr, count);
        collectSnapshotKeys(node->right, arr, count);
    }

    void collectBSTKeys(BSTNode* node, int* arr, int& count) {
        if (node == nullptr || count >= 32) return;
        collectBSTKeys(node->left, arr, count);
        if (count < 32) arr[count++] = node->key;
        collectBSTKeys(node->right, arr, count);
    }

    void collectAVLKeys(AVLNode* node, int* arr, int& count) {
        if (node == nullptr || count >= 32) return;
        collectAVLKeys(node->left, arr, count);
        if (count < 32) arr[count++] = node->key;
        collectAVLKeys(node->right, arr, count);
    }

    void loadCampaignLevel(int levelIndex) {
        if (!levelManager.loadLevel(levelIndex)) return;

        LevelDef* lvl = levelManager.getCurrentLevel();
        if (lvl == nullptr) return;

        undoManager.clear();

        if (lvl->actNumber == 1) {
            setAct("act_1");
            act1.initLevel(lvl->levelNumber, &binTree, levelManager.scoreSystem);
            if (lvl->levelNumber == 2) {
                syncReconVisuals();
            } else {
                syncBinaryTreeVisuals();
            }
            if (lvl->levelNumber == 1) {
                hud.setStatus("PATROL STAGE 1/4: Preorder. Click pods in Root -> Left -> Right order.",
                              "Shared alarm capacity tracks across all four traversal stages.");
            } else if (lvl->levelNumber == 2) {
                hud.setStatus("DECODE BLUEPRINT: Select a token from the tray below and place it into an open slot.",
                              act1.lvl12.stageName);
            } else if (lvl->levelNumber == 3) {
                hud.setStatus("HIVE METRICS: Click the next pod in Postorder (Left -> Right -> Root).",
                              "Once selected, enter the pod's height in edges.");
                inputBox.setValue(0);
            } else if (lvl->levelNumber == 4) {
                resetAllVisualStates();
                for (int i = 0; i < layout.nodeCount; i++) {
                    if (layout.nodes[i].key == 12) layout.nodes[i].setState("highlighted");
                    if (layout.nodes[i].key == 62) layout.nodes[i].setState("target");
                }
                hud.setStatus("CONVERGENCE: Pods 12 and 62 selected. Click their Lowest Common Ancestor (LCA).",
                              "Find the deepest meeting pod between the two branches.");
            } else if (lvl->levelNumber == 5) {
                hud.setStatus("BILATERAL REFLECTION: Click pods bottom-up to swap their left and right children.",
                              "Children must be mirrored before their parent pod (or use Auto Mirror).");
            }
        } else if (lvl->actNumber == 2) {
            setAct("act_2");
            eventController.clear();
            selectedNodeIndex = -1;
            while (!bst.isEmpty()) bst.deleteNode(bst.getRoot()->key);
            act2.initLevel(lvl->levelNumber, &bst, levelManager.scoreSystem);
            syncBSTVisuals();
            // Apply node visual states for act 2 start
            applyAct2InitialVisuals(lvl->levelNumber);
            hud.setStatus(act2.lvl21.statusMsg);
            if (lvl->levelNumber == 2) hud.setStatus(act2.lvl22.statusMsg);
            else if (lvl->levelNumber == 3) hud.setStatus(act2.lvl23.statusMsg);
            else if (lvl->levelNumber == 4) hud.setStatus(act2.lvl24.statusMsg);
            else if (lvl->levelNumber == 5) hud.setStatus(act2.lvl25.statusMsg);
        } else if (lvl->actNumber == 3) {
            setAct("act_3");
            eventController.clear();
            selectedNodeIndex = -1;
            avl.setEventLog(&eventLog);
            act3.initLevel(lvl->levelNumber, &avl, levelManager.scoreSystem);
            syncAVLVisuals();
            hud.setStatus(act3.lvl31.statusMsg);
            if (lvl->levelNumber == 2) hud.setStatus(act3.lvl32.statusMsg);
            else if (lvl->levelNumber == 3) hud.setStatus(act3.lvl33.statusMsg);
            else if (lvl->levelNumber == 4) hud.setStatus(act3.lvl34.statusMsg);
            else if (lvl->levelNumber == 5) hud.setStatus(act3.lvl35.statusMsg);
        } else if (lvl->actNumber == 4) {
            setAct("act_4");
            eventController.clear();
            selectedNodeIndex = -1;
            heap.setEventLog(&eventLog);
            act4.initLevel(lvl->levelNumber, &heap, levelManager.scoreSystem);
            syncHeapVisuals();
            applyAct4InitialVisuals(lvl->levelNumber);
            if (lvl->levelNumber == 1) {
                hud.setStatus(act4.lvl41.statusMsg);
            } else if (lvl->levelNumber == 2) {
                hud.setStatus(act4.lvl42.statusMsg);
            } else if (lvl->levelNumber == 3) {
                hud.setStatus(act4.lvl43.statusMsg);
            } else if (lvl->levelNumber == 4) {
                hud.setStatus(act4.lvl44.statusMsg);
            } else if (lvl->levelNumber == 5) {
                hud.setStatus(act4.lvl45.statusMsg);
            }
        }

        setScreenState("level_play");
        setupButtons();

        char buf[160];
        snprintf(buf, sizeof(buf), "[%s] OBJECTIVE: %s", lvl->title, lvl->objective);
        hud.setStatus(buf, "Manage the alarm capacity to earn maximum stars!");
        hud.resetAlarm(lvl->alarmCapacity);
    }

    void setupButtons() {
        buttonCount = 0;

        if (isScreen("menu")) {
            // Main Menu Buttons
            float bx = 490.0f;
            float by = 260.0f;
            float bw = 300.0f;
            float bh = 42.0f;
            float bgap = 18.0f;

            buttons[buttonCount++] = BioButton(bx, by, bw, bh, "1. CAMPAIGN MODE (20 Levels)", 210, true);
            by += bh + bgap;
            buttons[buttonCount++] = BioButton(bx, by, bw, bh, "2. SANDBOX MODE (Free Lab)", 211);
            by += bh + bgap;
            buttons[buttonCount++] = BioButton(bx, by, bw, bh, "3. ENDLESS MODE (Survival)", 212);
            by += bh + bgap;
            buttons[buttonCount++] = BioButton(bx, by, bw, bh, "4. QUIT GAME", 213);
            return;
        }

        if (isScreen("campaign_map")) {
            // Campaign Map: Act Selector Tabs (Row 1)
            float tx = 180.0f;
            float ty = 140.0f;
            float tw = 220.0f;
            float th = 38.0f;
            float tgap = 12.0f;

            buttons[buttonCount++] = BioButton(tx, ty, tw, th, "Act 1: Blueprint Wing", 311, campaignSelectedAct == 1);
            tx += tw + tgap;
            buttons[buttonCount++] = BioButton(tx, ty, tw, th, "Act 2: Sorted Wing", 312, campaignSelectedAct == 2);
            tx += tw + tgap;
            buttons[buttonCount++] = BioButton(tx, ty, tw, th, "Act 3: Architect's Wing", 313, campaignSelectedAct == 3);
            tx += tw + tgap;
            buttons[buttonCount++] = BioButton(tx, ty, tw, th, "Act 4: Heap Fortress", 314, campaignSelectedAct == 4);

            // 5 Level Cards for the selected Act
            float lx = 140.0f;
            float ly = 230.0f;
            float lw = 190.0f;
            float lh = 170.0f;
            float lgap = 15.0f;

            int startLvl = (campaignSelectedAct - 1) * 5;
            for (int i = 0; i < 5; i++) {
                int lvlIdx = startLvl + i;
                const LevelDef& ldef = levelManager.levels[lvlIdx];

                char label[48];
                if (ldef.isUnlocked) {
                    char stars[8] = "";
                    for (int s = 0; s < ldef.starsEarned; s++) strcat(stars, "*");
                    if (ldef.starsEarned == 0) strcpy(stars, "---");
                    snprintf(label, sizeof(label), "Lvl %d.%d [%s]", ldef.actNumber, ldef.levelNumber, stars);
                } else {
                    snprintf(label, sizeof(label), "Lvl %d.%d [LOCKED]", ldef.actNumber, ldef.levelNumber);
                }

                buttons[buttonCount++] = BioButton(lx + i * (lw + lgap), ly, lw, 44.0f, label, 301 + i, ldef.isCompleted);
            }

            // Back to Menu Button
            buttons[buttonCount++] = BioButton(540.0f, 620.0f, 200.0f, 38.0f, "< Back to Main Menu", 200);
            return;
        }

        // Gameplay / Sandbox Screens
        float row1Y = 58.0f;
        float row2Y = 92.0f;
        float btnH = 28.0f;
        float gap = 5.0f;

        bool inAct1 = isAct("act_1");
        bool inAct2 = isAct("act_2");
        bool inAct3 = isAct("act_3");
        bool inAct4 = isAct("act_4");

        float x1 = 15.0f;

        if (isScreen("level_play")) {
            // Level Play: Clean Navigation only (< Map)
            buttons[buttonCount++] = BioButton(x1, row1Y, 82.0f, btnH, "< Map", 205);
            x1 += 82.0f + gap * 2.0f;
        } else {
            // Sandbox / Endless Mode: Full Act Switchers and Undo
            buttons[buttonCount++] = BioButton(x1, row1Y, 82.0f, btnH, "Act 1: Tree", 101, inAct1);
            x1 += 82.0f + gap;
            buttons[buttonCount++] = BioButton(x1, row1Y, 82.0f, btnH, "Act 2: BST", 102, inAct2);
            x1 += 82.0f + gap;
            buttons[buttonCount++] = BioButton(x1, row1Y, 82.0f, btnH, "Act 3: AVL", 103, inAct3);
            x1 += 82.0f + gap;
            buttons[buttonCount++] = BioButton(x1, row1Y, 82.0f, btnH, "Act 4: Heap", 104, inAct4);
            x1 += 82.0f + gap;
            buttons[buttonCount++] = BioButton(x1, row1Y, 78.0f, btnH, "Undo (Z)", 202);
            x1 += 78.0f + gap * 2.0f;
        }

        // Input Box and +/- adjustments
        bool showInputBox = true;
        if (isScreen("level_play") && inAct1) {
            int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
            if (lvlNum != 3) {
                showInputBox = false;
            }
        }
        // Act 2 campaign levels don't use the input box — driven by click interactions
        if (isScreen("level_play") && inAct2) {
            showInputBox = false;
        }
        // Act 3 campaign levels are click/button driven — no input box needed
        if (isScreen("level_play") && inAct3) {
            showInputBox = false;
        }
        // Act 4 campaign levels are click/button driven — no input box needed
        if (isScreen("level_play") && inAct4) {
            showInputBox = false;
        }

        if (showInputBox) {
            float inputX = x1 + 10.0f;
            inputBox.x = inputX;
            inputBox.y = row1Y;
            inputBox.width = 50.0f;
            inputBox.height = btnH;
            x1 += 10.0f + 50.0f + 3.0f;

            buttons[buttonCount++] = BioButton(x1, row1Y, 24.0f, btnH, "-", 50);
            x1 += 24.0f + 2.0f;
            buttons[buttonCount++] = BioButton(x1, row1Y, 24.0f, btnH, "+", 51);
            x1 += 24.0f + gap * 2.0f;
        } else {
            inputBox.x = -500.0f;
            inputBox.y = -500.0f;
        }

        // Act Operations on Row 1
        if (inAct1) {
            if (isScreen("level_play")) {
                int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                if (lvlNum == 3) {
                    buttons[buttonCount++] = BioButton(x1, row1Y, 110.0f, btnH, "Submit H/D", 60, true);
                    x1 += 110.0f + gap;
                } else if (lvlNum == 5) {
                    buttons[buttonCount++] = BioButton(x1, row1Y, 100.0f, btnH, "Auto Mirror", 52, true);
                    x1 += 100.0f + gap;
                }
            } else {
                buttons[buttonCount++] = BioButton(x1, row1Y, 86.0f, btnH, "Mirror", 5);
                x1 += 86.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 100.0f, btnH, "Height/Diam", 6);
                x1 += 100.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 90.0f, btnH, "LCA/Path", 7);
                x1 += 90.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 86.0f, btnH, "Symmetry", 8);
                x1 += 86.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 96.0f, btnH, "Reconstruct", 9);
                x1 += 96.0f + gap;
            }
        } else if (inAct2) {
            if (isScreen("level_play")) {
                // Level-specific Act 2 buttons
                int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                if (lvlNum == 1) {
                    // Level 2.1: Show "Target Not Found" only when isNullReached
                    if (act2.lvl21.isNullReached && !act2.lvl21.isLevelComplete) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 160.0f, btnH, "Target Not Found", 70, true);
                        x1 += 160.0f + gap;
                    }
                } else if (lvlNum == 2) {
                    // Level 2.2: No action buttons — slot clicks and node clicks drive it
                    // Show hint about what to do
                } else if (lvlNum == 3) {
                    // Level 2.3: Steal and Collapse
                    if (strcmp(act2.lvl23.phase, "case_select") == 0) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 100.0f, btnH, "Leaf Node", 71, true);
                        x1 += 100.0f + gap;
                        buttons[buttonCount++] = BioButton(x1, row1Y, 100.0f, btnH, "One Child", 72, true);
                        x1 += 100.0f + gap;
                        buttons[buttonCount++] = BioButton(x1, row1Y, 120.0f, btnH, "Two Children", 73, true);
                        x1 += 120.0f + gap;
                    } else if (strcmp(act2.lvl23.phase, "stage_cleared") == 0) {
                        if (act2.lvl23.stage == 0) {
                            buttons[buttonCount++] = BioButton(x1, row1Y, 170.0f, btnH, "Continue to Stage 2", 77, true);
                            x1 += 170.0f + gap;
                        } else if (act2.lvl23.stage == 1) {
                            buttons[buttonCount++] = BioButton(x1, row1Y, 170.0f, btnH, "Continue to Stage 3", 77, true);
                            x1 += 170.0f + gap;
                        }
                    } else if (strcmp(act2.lvl23.phase, "successor_select") == 0) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 180.0f, btnH, "Confirm Successor (60)", 78, true);
                        x1 += 180.0f + gap;
                    }
                } else if (lvlNum == 4) {
                    // Level 2.4: Show Scan Range button if not yet executed
                    if (!act2.lvl24.isExecuted) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 150.0f, btnH, "Start Raid [30, 65]", 74, true);
                        x1 += 150.0f + gap;
                    }
                } else if (lvlNum == 5) {
                    // Level 2.5: Show stage-appropriate buttons
                    if (act2.lvl25.stage == 0) {
                        // Stage A: Insert Next button (auto-advances to next value and inserts)
                        if (act2.lvl25.insertedCount < act2.lvl25.totalCount) {
                            char lbl[48];
                            snprintf(lbl, sizeof(lbl), "Insert %d", act2.lvl25.sortedValues[act2.lvl25.insertedCount]);
                            buttons[buttonCount++] = BioButton(x1, row1Y, 120.0f, btnH, lbl, 75, true);
                            x1 += 120.0f + gap;
                        }
                    } else if (act2.lvl25.stage == 1) {
                        // Stage B: Search Next — click next node in chain
                        if (!act2.lvl25.searchFailed) {
                            char lbl[48];
                            snprintf(lbl, sizeof(lbl), "Click Pod %d", act2.lvl25.currentSearchNodeKey);
                            buttons[buttonCount++] = BioButton(x1, row1Y, 140.0f, btnH, lbl, 76);
                            x1 += 140.0f + gap;
                        }
                    } else if (act2.lvl25.stage >= 2) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 160.0f, btnH, "Advance to Act 3 >", 203, true);
                        x1 += 160.0f + gap;
                    }
                }
            } else {
                // Sandbox mode: generic BST buttons
                buttons[buttonCount++] = BioButton(x1, row1Y, 88.0f, btnH, "Insert Pod", 10);
                x1 += 88.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 88.0f, btnH, "Search Pod", 11);
                x1 += 88.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 88.0f, btnH, "Delete Pod", 12);
                x1 += 88.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 108.0f, btnH, "Range [30,70]", 13);
                x1 += 108.0f + gap;
            }
        } else if (inAct3) {
            if (isScreen("level_play")) {
                int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                if (lvlNum == 1) {
                    // Level 3.1: direct node clicks, no special buttons needed
                } else if (lvlNum == 2) {
                    // Level 3.2 rotations are performed by dragging the unbalanced pivot.
                } else if (lvlNum == 3) {
                    // Level 3.3 double rotations are performed by dragging the unbalanced pivot.
                } else if (lvlNum == 4) {
                    // Level 3.4: Prediction buttons (show for each insertion)
                    if (!act3.lvl34.isLevelComplete && act3.lvl34.currentIdx < act3.lvl34.totalCount) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 100.0f, btnH, "No Rotation", 84);
                        x1 += 100.0f + gap;
                        buttons[buttonCount++] = BioButton(x1, row1Y, 60.0f, btnH, "LL", 85);
                        x1 += 60.0f + gap;
                        buttons[buttonCount++] = BioButton(x1, row1Y, 60.0f, btnH, "RR", 86);
                        x1 += 60.0f + gap;
                        buttons[buttonCount++] = BioButton(x1, row1Y, 60.0f, btnH, "LR", 87);
                        x1 += 60.0f + gap;
                        buttons[buttonCount++] = BioButton(x1, row1Y, 60.0f, btnH, "RL", 88);
                        x1 += 60.0f + gap;
                    }
                } else if (lvlNum == 5) {
                    // Level 3.5: drag for Phase A repair; keep the claim action in Phase B.
                    if (act3.lvl35.phase == 1 && act3.lvl35.targetFound) {
                        buttons[buttonCount++] = BioButton(x1, row1Y, 140.0f, btnH, "Claim Treasure!", 89, true);
                        x1 += 140.0f + gap;
                    }
                }
            } else {
                // Sandbox mode: generic AVL buttons
                buttons[buttonCount++] = BioButton(x1, row1Y, 80.0f, btnH, "Insert", 10);
                x1 += 80.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 80.0f, btnH, "Delete", 12);
                x1 += 80.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 80.0f, btnH, "Search", 11);
                x1 += 80.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 78.0f, btnH, "Demo LL", 31);
                x1 += 78.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 78.0f, btnH, "Demo RR", 32);
                x1 += 78.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 78.0f, btnH, "Demo LR", 33);
                x1 += 78.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 78.0f, btnH, "Demo RL", 34);
                x1 += 78.0f + gap;
            }
        } else if (inAct4) {
            if (isScreen("level_play")) {
                int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                if (lvlNum == 1) {
                    if (!act4.lvl41.isLevelComplete) {
                        if (act4.lvl41.selectedChildIndex != -1) {
                            char btnLabel[64];
                            snprintf(btnLabel, sizeof(btnLabel), "Confirm (%s)", act4.lvl41.proposedSwapText);
                            buttons[buttonCount++] = BioButton(x1, row1Y, 190.0f, btnH, btnLabel, 95, true);
                            x1 += 190.0f + gap;
                            buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Cancel Selection", 97);
                            x1 += 130.0f + gap;
                        } else {
                            buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Confirm Swap", 95, false);
                            x1 += 130.0f + gap;
                            buttons[buttonCount++] = BioButton(x1, row1Y, 140.0f, btnH, "No Swap Needed", 96, true);
                            x1 += 140.0f + gap;
                        }
                    }
                } else if (lvlNum == 2) {
                    if (!act4.lvl42.isLevelComplete) {
                        if (act4.lvl42.round == 1) {
                            if (act4.lvl42.selectedChildIndex != -1) {
                                char btnLabel[64];
                                snprintf(btnLabel, sizeof(btnLabel), "Confirm (%s)", act4.lvl42.proposedSwapText);
                                buttons[buttonCount++] = BioButton(x1, row1Y, 190.0f, btnH, btnLabel, 110, true);
                                x1 += 190.0f + gap;
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Cancel Selection", 97);
                                x1 += 130.0f + gap;
                            } else {
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Confirm Swap", 110, false);
                                x1 += 130.0f + gap;
                            }
                        } else if (act4.lvl42.round == 2) {
                            buttons[buttonCount++] = BioButton(x1, row1Y, 160.0f, btnH, "Confirm Move Up", 111, true);
                            x1 += 160.0f + gap;
                        }
                    }
                } else if (lvlNum == 3) {
                    if (!act4.lvl43.isLevelComplete) {
                        if (act4.lvl43.subPhase == 0) {
                            buttons[buttonCount++] = BioButton(x1, row1Y, 150.0f, btnH, "Extract Maximum", 112, true);
                            x1 += 150.0f + gap;
                        } else if (act4.lvl43.subPhase == 1) {
                            if (act4.lvl43.selectedChildIndex != -1) {
                                char btnLabel[64];
                                snprintf(btnLabel, sizeof(btnLabel), "Confirm (%s)", act4.lvl43.proposedSwapText);
                                buttons[buttonCount++] = BioButton(x1, row1Y, 190.0f, btnH, btnLabel, 113, true);
                                x1 += 190.0f + gap;
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Cancel Selection", 97);
                                x1 += 130.0f + gap;
                            } else {
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Confirm Swap", 113, false);
                                x1 += 130.0f + gap;
                            }
                        }
                    }
                } else if (lvlNum == 4) {
                    if (!act4.lvl44.isLevelComplete) {
                        if (act4.lvl44.subPhase == 0) {
                            buttons[buttonCount++] = BioButton(x1, row1Y, 150.0f, btnH, "Extract Maximum", 114, true);
                            x1 += 150.0f + gap;
                        } else if (act4.lvl44.subPhase == 1) {
                            if (act4.lvl44.selectedChildIndex != -1) {
                                char btnLabel[64];
                                snprintf(btnLabel, sizeof(btnLabel), "Confirm (%s)", act4.lvl44.proposedSwapText);
                                buttons[buttonCount++] = BioButton(x1, row1Y, 190.0f, btnH, btnLabel, 115, true);
                                x1 += 190.0f + gap;
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Cancel Selection", 97);
                                x1 += 130.0f + gap;
                            } else {
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Confirm Swap", 115, false);
                                x1 += 130.0f + gap;
                            }
                        }
                    }
                } else if (lvlNum == 5) {
                    if (!act4.lvl45.isLevelComplete) {
                        if (strcmp(act4.lvl45.phase, "build") == 0) {
                            if (act4.lvl45.selectedChildIndex != -1) {
                                char btnLabel[64];
                                snprintf(btnLabel, sizeof(btnLabel), "Confirm (%s)", act4.lvl45.proposedSwapText);
                                buttons[buttonCount++] = BioButton(x1, row1Y, 190.0f, btnH, btnLabel, 116, true);
                                x1 += 190.0f + gap;
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Cancel Selection", 97);
                                x1 += 130.0f + gap;
                            } else {
                                buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Confirm Swap", 116, false);
                                x1 += 130.0f + gap;
                                buttons[buttonCount++] = BioButton(x1, row1Y, 140.0f, btnH, "No Swap Needed", 117, true);
                                x1 += 140.0f + gap;
                            }
                        } else if (strcmp(act4.lvl45.phase, "sort") == 0) {
                            if (act4.lvl45.subPhase == 0) {
                                buttons[buttonCount++] = BioButton(x1, row1Y, 150.0f, btnH, "Extract Maximum", 118, true);
                                x1 += 150.0f + gap;
                            } else if (act4.lvl45.subPhase == 1) {
                                if (act4.lvl45.selectedChildIndex != -1) {
                                    char btnLabel[64];
                                    snprintf(btnLabel, sizeof(btnLabel), "Confirm (%s)", act4.lvl45.proposedSwapText);
                                    buttons[buttonCount++] = BioButton(x1, row1Y, 190.0f, btnH, btnLabel, 119, true);
                                    x1 += 190.0f + gap;
                                    buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Cancel Selection", 97);
                                    x1 += 130.0f + gap;
                                } else {
                                    buttons[buttonCount++] = BioButton(x1, row1Y, 130.0f, btnH, "Confirm Swap", 119, false);
                                    x1 += 130.0f + gap;
                                }
                            }
                        }
                    }
                }
            } else {
                buttons[buttonCount++] = BioButton(x1, row1Y, 88.0f, btnH, "Insert", 10);
                x1 += 88.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 105.0f, btnH, "Extract Max", 40);
                x1 += 105.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 98.0f, btnH, "Build Heap", 41);
                x1 += 98.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 94.0f, btnH, "Heap Sort", 42);
                x1 += 94.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 88.0f, btnH, "Peek Max", 43);
                x1 += 88.0f + gap;
                buttons[buttonCount++] = BioButton(x1, row1Y, 96.0f, btnH, "Check Heap", 44);
                x1 += 96.0f + gap;
            }
        }

        // Far right Menu button in sandbox
        if (isScreen("sandbox")) {
            buttons[buttonCount++] = BioButton(1185.0f, row1Y, 80.0f, btnH, "< Menu", 200);
        }

        // ===== ROW 2: Only in Sandbox Mode (Clean Campaign Mode has no extra buttons) =====
        if (!isScreen("level_play")) {
            // Row 2 Left Side: Structure Operations
            float x2 = 15.0f;
            if (!inAct4) {
                buttons[buttonCount++] = BioButton(x2, row2Y, 92.0f, btnH, "Preorder", 1);
                x2 += 92.0f + gap;
                buttons[buttonCount++] = BioButton(x2, row2Y, 92.0f, btnH, "Inorder", 2);
                x2 += 92.0f + gap;
                buttons[buttonCount++] = BioButton(x2, row2Y, 92.0f, btnH, "Postorder", 3);
                x2 += 92.0f + gap;
                buttons[buttonCount++] = BioButton(x2, row2Y, 98.0f, btnH, "LevelOrder", 4);
                x2 += 98.0f + gap * 2.0f;

                if (inAct3) {
                    buttons[buttonCount++] = BioButton(x2, row2Y, 86.0f, btnH, "k-th (k=3)", 15);
                    x2 += 86.0f + gap;
                    buttons[buttonCount++] = BioButton(x2, row2Y, 78.0f, btnH, "Rank Pod", 16);
                    x2 += 78.0f + gap;
                }
            } else if (inAct4) {
                buttons[buttonCount++] = BioButton(x2, row2Y, 98.0f, btnH, "LevelOrder", 4);
                x2 += 98.0f + gap;
                buttons[buttonCount++] = BioButton(x2, row2Y, 102.0f, btnH, "Sift Down [0]", 45);
                x2 += 102.0f + gap;
                buttons[buttonCount++] = BioButton(x2, row2Y, 102.0f, btnH, "Sift Up [end]", 46);
                x2 += 102.0f + gap;
            }

            // Row 2 Right Side: Playback Controls (Sandbox only)
            float rx = 1265.0f;
            char speedLabel[16];
            snprintf(speedLabel, sizeof(speedLabel), "Spd: %.1fx", eventController.speedMultiplier);
            rx -= 74.0f;
            buttons[buttonCount++] = BioButton(rx, row2Y, 74.0f, btnH, speedLabel, 35);
            rx -= 74.0f + gap;
            buttons[buttonCount++] = BioButton(rx, row2Y, 74.0f, btnH, "Step (->)", 30);
            rx -= 84.0f + gap;
            buttons[buttonCount++] = BioButton(rx, row2Y, 84.0f, btnH, eventController.isPaused ? "Play" : "Pause", 28);
            rx -= 74.0f + gap;
            buttons[buttonCount++] = BioButton(rx, row2Y, 74.0f, btnH, "Step (<-)", 29);
        }

        // Result overlay buttons if level completed or failed
        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            float ox = 420.0f;
            float oy = 430.0f;
            buttons[buttonCount++] = BioButton(ox, oy, 130.0f, 38.0f, "Next Level >", 203, true);
            buttons[buttonCount++] = BioButton(ox + 150.0f, oy, 130.0f, 38.0f, "Retry", 204);
            buttons[buttonCount++] = BioButton(ox + 300.0f, oy, 130.0f, 38.0f, "Campaign Map", 205);
        } else if (isScreen("level_play") && strcmp(levelManager.playState, "failed") == 0) {
            float ox = 450.0f;
            float oy = 440.0f;
            buttons[buttonCount++] = BioButton(ox, oy, 160.0f, 42.0f, "Retry Level", 204);
            buttons[buttonCount++] = BioButton(ox + 180.0f, oy, 180.0f, 42.0f, "Go to Map", 205, true);
        }
    }

    // Act 1: Binary Tree Setup
    void setupDefaultAct1Tree() {
        setAct("act_1");
        eventController.clear();
        selectedNodeIndex = -1;

        binTree.destroyTree(binTree.root);
        binTree.root = new TreeNode(50);
        binTree.root->left = new TreeNode(25);
        binTree.root->right = new TreeNode(75);
        binTree.root->left->left = new TreeNode(12);
        binTree.root->left->right = new TreeNode(37);
        binTree.root->right->left = new TreeNode(62);
        binTree.root->right->right = new TreeNode(88);

        syncBinaryTreeVisuals();
        inputBox.setValue(50);
    }

    // Act 2: BST Setup
    void setupDefaultAct2BST() {
        setAct("act_2");
        eventController.clear();
        selectedNodeIndex = -1;

        while (!bst.isEmpty()) {
            bst.deleteNode(bst.getRoot()->key);
        }

        int initialKeys[] = {50, 30, 70, 20, 40, 60, 85};
        for (int i = 0; i < 7; i++) {
            bst.insert(initialKeys[i]);
        }

        syncBSTVisuals();
        inputBox.setValue(60);
    }

    // Act 3: AVL Tree Setup
    void setupDefaultAct3AVL() {
        setAct("act_3");
        eventController.clear();
        selectedNodeIndex = -1;

        avl.setEventLog(&eventLog);
        while (!avl.isEmpty()) {
            avl.deleteNode(avl.getRoot()->key);
        }
        eventLog.clear();

        int initialKeys[] = {30, 20, 40, 10, 25, 35, 50};
        for (int i = 0; i < 7; i++) {
            avl.insert(initialKeys[i]);
        }
        eventLog.clear();

        syncAVLVisuals();
        inputBox.setValue(25);
    }

    // Act 4: Max Heap Setup
    void setupDefaultAct4Heap() {
        setAct("act_4");
        eventController.clear();
        selectedNodeIndex = -1;

        heap.setEventLog(&eventLog);
        int initialVals[] = {90, 75, 80, 50, 60, 40, 70, 20, 30};
        heap.buildMaxHeap(initialVals, 9);
        eventLog.clear();

        syncHeapVisuals();
        inputBox.setValue(85);
    }

    int mapBinTreeNodeToVisual(TreeNode* node) {
        if (node == nullptr) return -1;
        int idx = layout.allocateNode(node->data);
        if (idx == -1) return -1;

        if (node->left != nullptr) {
            int leftIdx = mapBinTreeNodeToVisual(node->left);
            layout.nodes[idx].leftChildIndex = leftIdx;
            if (leftIdx != -1) layout.nodes[leftIdx].parentIndex = idx;
        }
        if (node->right != nullptr) {
            int rightIdx = mapBinTreeNodeToVisual(node->right);
            layout.nodes[idx].rightChildIndex = rightIdx;
            if (rightIdx != -1) layout.nodes[rightIdx].parentIndex = idx;
        }
        return idx;
    }

    void syncBinaryTreeVisuals() {
        layout.clear();
        mapBinTreeNodeToVisual(binTree.root);
        layout.updateLayout(1280.0f, true);
        selectedNodeIndex = -1;
    }

    void syncReconVisuals() {
        layout.clear();
        for (int s = 0; s < act1.lvl12.slotCount; s++) {
            if (act1.lvl12.slots[s].isOccupied) {
                int idx = layout.allocateNode(act1.lvl12.slots[s].placedValue);
                if (idx != -1) {
                    layout.nodes[idx].x = act1.lvl12.slots[s].x;
                    layout.nodes[idx].y = act1.lvl12.slots[s].y;
                    layout.nodes[idx].targetX = act1.lvl12.slots[s].x;
                    layout.nodes[idx].targetY = act1.lvl12.slots[s].y;
                }
            }
        }
        for (int s = 0; s < act1.lvl12.slotCount; s++) {
            if (!act1.lvl12.slots[s].isOccupied) continue;
            int pSlot = act1.lvl12.slots[s].parentSlot;
            if (pSlot != -1 && act1.lvl12.slots[pSlot].isOccupied) {
                int sVisual = -1, pVisual = -1;
                for (int i = 0; i < layout.nodeCount; i++) {
                    if (layout.nodes[i].active) {
                        if (layout.nodes[i].key == act1.lvl12.slots[s].placedValue) sVisual = i;
                        if (layout.nodes[i].key == act1.lvl12.slots[pSlot].placedValue) pVisual = i;
                    }
                }
                if (sVisual != -1 && pVisual != -1) {
                    layout.nodes[sVisual].parentIndex = pVisual;
                    if (act1.lvl12.slots[s].isLeft) {
                        layout.nodes[pVisual].leftChildIndex = sVisual;
                    } else {
                        layout.nodes[pVisual].rightChildIndex = sVisual;
                    }
                }
            }
        }
        selectedNodeIndex = -1;
    }

    int mapBSTNodeToVisual(BSTNode* node) {
        if (node == nullptr) return -1;
        int idx = layout.allocateNode(node->key);
        if (idx == -1) return -1;

        if (node->left != nullptr) {
            int leftIdx = mapBSTNodeToVisual(node->left);
            layout.nodes[idx].leftChildIndex = leftIdx;
            if (leftIdx != -1) layout.nodes[leftIdx].parentIndex = idx;
        }
        if (node->right != nullptr) {
            int rightIdx = mapBSTNodeToVisual(node->right);
            layout.nodes[idx].rightChildIndex = rightIdx;
            if (rightIdx != -1) layout.nodes[rightIdx].parentIndex = idx;
        }
        return idx;
    }

    void syncBSTVisuals() {
        layout.clear();
        mapBSTNodeToVisual(bst.getRoot());
        if (isScreen("level_play") && isAct("act_2") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 2) {
            computeAct2EmptySlots();
        } else {
            layout.updateLayout(1280.0f, true);
        }
        selectedNodeIndex = -1;
    }

    int mapAVLNodeToVisual(AVLNode* node) {
        if (node == nullptr) return -1;
        int idx = layout.allocateNode(node->key);
        if (idx == -1) return -1;

        layout.nodes[idx].balanceFactor = avl.balanceFactor(node);
        layout.nodes[idx].subtreeSize = avl.subtreeSize(node);

        if (node->left != nullptr) {
            int leftIdx = mapAVLNodeToVisual(node->left);
            layout.nodes[idx].leftChildIndex = leftIdx;
            if (leftIdx != -1) layout.nodes[leftIdx].parentIndex = idx;
        }
        if (node->right != nullptr) {
            int rightIdx = mapAVLNodeToVisual(node->right);
            layout.nodes[idx].rightChildIndex = rightIdx;
            if (rightIdx != -1) layout.nodes[rightIdx].parentIndex = idx;
        }
        return idx;
    }

    void syncAVLVisuals() {
        layout.clear();
        mapAVLNodeToVisual(avl.getRoot());
        layout.updateLayout(1280.0f, true);
        selectedNodeIndex = -1;
    }

    void syncAVLVisualsAnimated() {
        int previousKeys[TreeLayoutEngine::MAX_NODES];
        float previousX[TreeLayoutEngine::MAX_NODES];
        float previousY[TreeLayoutEngine::MAX_NODES];
        int previousCount = 0;
        for (int i = 0; i < layout.nodeCount && previousCount < TreeLayoutEngine::MAX_NODES; i++) {
            if (!layout.nodes[i].active) continue;
            previousKeys[previousCount] = layout.nodes[i].key;
            previousX[previousCount] = layout.nodes[i].x;
            previousY[previousCount] = layout.nodes[i].y;
            previousCount++;
        }

        layout.clear();
        mapAVLNodeToVisual(avl.getRoot());
        for (int i = 0; i < layout.nodeCount; i++) {
            if (!layout.nodes[i].active) continue;
            for (int j = 0; j < previousCount; j++) {
                if (layout.nodes[i].key == previousKeys[j]) {
                    layout.nodes[i].x = previousX[j];
                    layout.nodes[i].y = previousY[j];
                    break;
                }
            }
        }
        layout.updateLayout(1280.0f, false);
        selectedNodeIndex = -1;
    }

    void syncHeapVisuals() {
        layout.clear();
        int n = heap.getSize();
        const int* arr = heap.getArray();

        for (int i = 0; i < n; i++) {
            int idx = layout.allocateNode(arr[i]);
            if (idx == -1) break;
            layout.nodes[idx].heapIndex = i;
        }

        for (int i = 0; i < n; i++) {
            int leftChild = 2 * i + 1;
            int rightChild = 2 * i + 2;

            if (leftChild < n) {
                layout.nodes[i].leftChildIndex = leftChild;
                layout.nodes[leftChild].parentIndex = i;
            }
            if (rightChild < n) {
                layout.nodes[i].rightChildIndex = rightChild;
                layout.nodes[rightChild].parentIndex = i;
            }
        }

        layout.updateLayout(1280.0f, true);
        selectedNodeIndex = -1;
    }

    void resetAllVisualStates() {
        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].active) {
                layout.nodes[i].setState("normal");
            }
        }
    }

    // Apply Act 2 level-specific initial visual states after syncBSTVisuals()
    void applyAct2InitialVisuals(int lvlNum) {
        if (lvlNum == 1) {
            // Level 2.1: All nodes hidden (show '?') — mark as inactive until revealed
            for (int i = 0; i < layout.nodeCount; i++) {
                if (layout.nodes[i].active) {
                    layout.nodes[i].setState("inactive");
                }
            }
            // Root is the only clickable node — highlight it
            int rootIdx = layout.findRootIndex();
            if (rootIdx != -1) {
                layout.nodes[rootIdx].setState("searching");
            }
        } else if (lvlNum == 2) {
            // Level 2.2: All nodes visible and normal; root highlighted as current
            resetAllVisualStates();
            int rootIdx = layout.findRootIndex();
            if (rootIdx != -1) {
                layout.nodes[rootIdx].setState("highlighted");
            }
        } else if (lvlNum == 3) {
            // Level 2.3: Highlight target node to be deleted
            resetAllVisualStates();
            int targetIdx = findVisualNodeByKey(act2.lvl23.deleteKey);
            if (targetIdx != -1) {
                layout.nodes[targetIdx].setState("target");
            }
        } else if (lvlNum == 4) {
            // Level 2.4: All nodes normal before range query
            resetAllVisualStates();
        } else if (lvlNum == 5) {
            // Level 2.5 Stage A: empty tree, nothing to show
            resetAllVisualStates();
        }
    }

    // After Act 2 Level 2.4 range query, apply pruned/matched visual states
    void applyAct2RangeVisuals() {
        for (int i = 0; i < layout.nodeCount; i++) {
            if (!layout.nodes[i].active) continue;
            int k = layout.nodes[i].key;
            if (act2.isKeyPruned(k)) {
                layout.nodes[i].setState("inactive");
            } else if (act2.isKeyMatching(k)) {
                layout.nodes[i].setState("highlighted");
            } else {
                layout.nodes[i].setState("normal");
            }
        }
    }

    void applyAct4InitialVisuals(int lvlNum) {
        int n = heap.getSize();
        if (lvlNum == 1) {
            int parentIdx = act4.lvl41.currentSiftIndex;
            int leftIdx = 2 * parentIdx + 1;
            int rightIdx = 2 * parentIdx + 2;

            for (int i = 0; i < layout.nodeCount && i < n; i++) {
                if (!layout.nodes[i].active) continue;
                if (i == act4.lvl41.selectedChildIndex) {
                    layout.nodes[i].setState("target");
                } else if (i == parentIdx) {
                    layout.nodes[i].setState("highlighted");
                } else if (i == leftIdx || i == rightIdx) {
                    layout.nodes[i].setState("searching");
                } else {
                    layout.nodes[i].setState("idle");
                }
            }
        } else if (lvlNum == 2) {
            if (act4.lvl42.round == 1) {
                int parentIdx = act4.lvl42.currentSiftIndex;
                int leftIdx = 2 * parentIdx + 1;
                int rightIdx = 2 * parentIdx + 2;
                for (int i = 0; i < layout.nodeCount && i < n; i++) {
                    if (!layout.nodes[i].active) continue;
                    if (i == act4.lvl42.selectedChildIndex) {
                        layout.nodes[i].setState("target");
                    } else if (i == parentIdx) {
                        layout.nodes[i].setState("highlighted");
                    } else if (i == leftIdx || i == rightIdx) {
                        layout.nodes[i].setState("searching");
                    } else {
                        layout.nodes[i].setState("idle");
                    }
                }
            } else if (act4.lvl42.round == 2) {
                int childIdx = act4.lvl42.currentUpChildIndex;
                int parentIdx = act4.lvl42.currentUpParentIndex;
                for (int i = 0; i < layout.nodeCount && i < n; i++) {
                    if (!layout.nodes[i].active) continue;
                    if (i == childIdx) {
                        layout.nodes[i].setState("target");
                    } else if (i == parentIdx) {
                        layout.nodes[i].setState("highlighted");
                    } else {
                        layout.nodes[i].setState("idle");
                    }
                }
            }
        } else if (lvlNum == 3) {
            int activeSize = act4.lvl43.activeHeapSize;
            for (int i = 0; i < layout.nodeCount && i < n; i++) {
                if (!layout.nodes[i].active) continue;
                if (i >= activeSize) {
                    layout.nodes[i].setState("inactive");
                } else if (act4.lvl43.subPhase == 0) {
                    if (i == 0) layout.nodes[i].setState("target");
                    else layout.nodes[i].setState("idle");
                } else if (act4.lvl43.subPhase == 1) {
                    int pIdx = act4.lvl43.currentSiftIndex;
                    int leftIdx = 2 * pIdx + 1;
                    int rightIdx = 2 * pIdx + 2;
                    if (i == act4.lvl43.selectedChildIndex) {
                        layout.nodes[i].setState("target");
                    } else if (i == pIdx) {
                        layout.nodes[i].setState("highlighted");
                    } else if ((i == leftIdx || i == rightIdx) && i < activeSize) {
                        layout.nodes[i].setState("searching");
                    } else {
                        layout.nodes[i].setState("idle");
                    }
                }
            }
        } else if (lvlNum == 4) {
            int activeSize = act4.lvl44.activeHeapSize;
            for (int i = 0; i < layout.nodeCount && i < n; i++) {
                if (!layout.nodes[i].active) continue;
                if (i >= activeSize) {
                    layout.nodes[i].setState("inactive");
                } else if (act4.lvl44.subPhase == 0) {
                    if (i == 0) layout.nodes[i].setState("target");
                    else layout.nodes[i].setState("idle");
                } else if (act4.lvl44.subPhase == 1) {
                    int pIdx = act4.lvl44.currentSiftIndex;
                    int leftIdx = 2 * pIdx + 1;
                    int rightIdx = 2 * pIdx + 2;
                    if (i == act4.lvl44.selectedChildIndex) {
                        layout.nodes[i].setState("target");
                    } else if (i == pIdx) {
                        layout.nodes[i].setState("highlighted");
                    } else if ((i == leftIdx || i == rightIdx) && i < activeSize) {
                        layout.nodes[i].setState("searching");
                    } else {
                        layout.nodes[i].setState("idle");
                    }
                }
            }
        } else if (lvlNum == 5) {
            if (strcmp(act4.lvl45.phase, "build") == 0) {
                int parentIdx = act4.lvl45.currentSiftIndex;
                int leftIdx = 2 * parentIdx + 1;
                int rightIdx = 2 * parentIdx + 2;
                for (int i = 0; i < layout.nodeCount && i < n; i++) {
                    if (!layout.nodes[i].active) continue;
                    if (i == act4.lvl45.selectedChildIndex) {
                        layout.nodes[i].setState("target");
                    } else if (i == parentIdx) {
                        layout.nodes[i].setState("highlighted");
                    } else if (i == leftIdx || i == rightIdx) {
                        layout.nodes[i].setState("searching");
                    } else {
                        layout.nodes[i].setState("idle");
                    }
                }
            } else if (strcmp(act4.lvl45.phase, "sort") == 0) {
                int activeSize = act4.lvl45.activeHeapSize;
                for (int i = 0; i < layout.nodeCount && i < n; i++) {
                    if (!layout.nodes[i].active) continue;
                    if (i >= activeSize) {
                        layout.nodes[i].setState("inactive");
                    } else if (act4.lvl45.subPhase == 0) {
                        if (i == 0) layout.nodes[i].setState("target");
                        else layout.nodes[i].setState("idle");
                    } else if (act4.lvl45.subPhase == 1) {
                        int pIdx = act4.lvl45.currentSiftIndex;
                        int leftIdx = 2 * pIdx + 1;
                        int rightIdx = 2 * pIdx + 2;
                        if (i == act4.lvl45.selectedChildIndex) {
                            layout.nodes[i].setState("target");
                        } else if (i == pIdx) {
                            layout.nodes[i].setState("highlighted");
                        } else if ((i == leftIdx || i == rightIdx) && i < activeSize) {
                            layout.nodes[i].setState("searching");
                        } else {
                            layout.nodes[i].setState("idle");
                        }
                    }
                }
            } else if (strcmp(act4.lvl45.phase, "validate") == 0) {
                for (int i = 0; i < layout.nodeCount && i < n; i++) {
                    if (layout.nodes[i].active) layout.nodes[i].setState("target");
                }
            }
        }
    }

    // Helper: find visual node index by BST key
    int findVisualNodeByKey(int key) const {
        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].active && layout.nodes[i].key == key) return i;
        }
        return -1;
    }

    // Compute empty slots for Level 2.2 from BST NULL children using dynamic tree layout
    void computeAct2EmptySlots() {
        act2.lvl22.slotCount = 0;
        layout.updateLayoutWithNullSlots(1280.0f, act2.lvl22.slots, act2.lvl22.slotCount, 16, true);

        // Mark which slot is the target insertion slot for the insert key (only if level not complete)
        if (!act2.lvl22.isLevelComplete) {
            int insertKey = act2.lvl22.insertKey;
            for (int i = 0; i < act2.lvl22.slotCount; i++) {
                act2.lvl22.slots[i].isTarget = false;
            }
            // Determine correct slot: traverse BST to find where insertKey would go
            BSTNode* cur = bst.getRoot();
            int parentKey = -1;
            bool goLeft = false;
            while (cur != nullptr) {
                if (insertKey < cur->key) {
                    if (cur->left == nullptr) { parentKey = cur->key; goLeft = true; break; }
                    cur = cur->left;
                } else if (insertKey > cur->key) {
                    if (cur->right == nullptr) { parentKey = cur->key; goLeft = false; break; }
                    cur = cur->right;
                } else {
                    break; // duplicate
                }
            }
            if (parentKey != -1) {
                for (int i = 0; i < act2.lvl22.slotCount; i++) {
                    if (act2.lvl22.slots[i].parentKey == parentKey && act2.lvl22.slots[i].isLeft == goLeft) {
                        act2.lvl22.slots[i].isTarget = true;
                        break;
                    }
                }
            }
        }
    }

    void buildPreorderEvents(int idx) {
        if (idx == -1 || !layout.nodes[idx].active) return;
        eventLog.logVisit(idx, layout.nodes[idx].key);
        buildPreorderEvents(layout.nodes[idx].leftChildIndex);
        buildPreorderEvents(layout.nodes[idx].rightChildIndex);
    }

    void buildInorderEvents(int idx) {
        if (idx == -1 || !layout.nodes[idx].active) return;
        buildInorderEvents(layout.nodes[idx].leftChildIndex);
        eventLog.logVisit(idx, layout.nodes[idx].key);
        buildInorderEvents(layout.nodes[idx].rightChildIndex);
    }

    void buildPostorderEvents(int idx) {
        if (idx == -1 || !layout.nodes[idx].active) return;
        buildPostorderEvents(layout.nodes[idx].leftChildIndex);
        buildPostorderEvents(layout.nodes[idx].rightChildIndex);
        eventLog.logVisit(idx, layout.nodes[idx].key);
    }

    void buildLevelOrderEvents() {
        int rootIdx = layout.findRootIndex();
        if (rootIdx == -1) return;

        int queue[TreeLayoutEngine::MAX_NODES];
        int front = 0;
        int back = 0;
        queue[back++] = rootIdx;

        while (front < back) {
            int cur = queue[front++];
            eventLog.logVisit(cur, layout.nodes[cur].key);
            if (layout.nodes[cur].leftChildIndex != -1) queue[back++] = layout.nodes[cur].leftChildIndex;
            if (layout.nodes[cur].rightChildIndex != -1) queue[back++] = layout.nodes[cur].rightChildIndex;
        }
    }

    void collectPreorderSequence(int idx, char* buf, int maxLen, int& offset) {
        if (idx == -1 || !layout.nodes[idx].active || offset >= maxLen - 12) return;
        int written = 0;
        if (offset == 0) {
            written = snprintf(buf + offset, maxLen - offset, "%d", layout.nodes[idx].key);
        } else {
            written = snprintf(buf + offset, maxLen - offset, " -> %d", layout.nodes[idx].key);
        }
        if (written > 0) offset += written;
        collectPreorderSequence(layout.nodes[idx].leftChildIndex, buf, maxLen, offset);
        collectPreorderSequence(layout.nodes[idx].rightChildIndex, buf, maxLen, offset);
    }

    void collectInorderSequence(int idx, char* buf, int maxLen, int& offset) {
        if (idx == -1 || !layout.nodes[idx].active || offset >= maxLen - 12) return;
        collectInorderSequence(layout.nodes[idx].leftChildIndex, buf, maxLen, offset);
        if (offset >= maxLen - 12) return;
        int written = 0;
        if (offset == 0) {
            written = snprintf(buf + offset, maxLen - offset, "%d", layout.nodes[idx].key);
        } else {
            written = snprintf(buf + offset, maxLen - offset, " -> %d", layout.nodes[idx].key);
        }
        if (written > 0) offset += written;
        collectInorderSequence(layout.nodes[idx].rightChildIndex, buf, maxLen, offset);
    }

    void collectPostorderSequence(int idx, char* buf, int maxLen, int& offset) {
        if (idx == -1 || !layout.nodes[idx].active || offset >= maxLen - 12) return;
        collectPostorderSequence(layout.nodes[idx].leftChildIndex, buf, maxLen, offset);
        collectPostorderSequence(layout.nodes[idx].rightChildIndex, buf, maxLen, offset);
        if (offset >= maxLen - 12) return;
        int written = 0;
        if (offset == 0) {
            written = snprintf(buf + offset, maxLen - offset, "%d", layout.nodes[idx].key);
        } else {
            written = snprintf(buf + offset, maxLen - offset, " -> %d", layout.nodes[idx].key);
        }
        if (written > 0) offset += written;
    }

    void collectLevelOrderSequence(char* buf, int maxLen, int& offset) {
        int rootIdx = layout.findRootIndex();
        if (rootIdx == -1) return;

        int queue[TreeLayoutEngine::MAX_NODES];
        int front = 0;
        int back = 0;
        queue[back++] = rootIdx;

        while (front < back && offset < maxLen - 12) {
            int cur = queue[front++];
            int written = 0;
            if (offset == 0) {
                written = snprintf(buf + offset, maxLen - offset, "%d", layout.nodes[cur].key);
            } else {
                written = snprintf(buf + offset, maxLen - offset, " -> %d", layout.nodes[cur].key);
            }
            if (written > 0) offset += written;

            if (layout.nodes[cur].leftChildIndex != -1) queue[back++] = layout.nodes[cur].leftChildIndex;
            if (layout.nodes[cur].rightChildIndex != -1) queue[back++] = layout.nodes[cur].rightChildIndex;
        }
    }

    // Action Handlers
    void handleInsertAction() {
        takeSnapshot();
        int val = inputBox.getValue(50);

        if (isAct("act_2")) {
            if (bst.search(val)) {
                levelManager.scoreSystem.addAlarmStep(1);
                char buf[128];
                snprintf(buf, sizeof(buf), "BST REJECTED: Duplicate key %d exists! Prohibited by BST rules.", val);
                hud.setStatus(buf, "Duplicate insertion safely rejected without altering tree.");
                return;
            }

            eventLog.clear();
            BSTNode* cur = bst.getRoot();
            while (cur != nullptr) {
                eventLog.logCompare(cur->id, -1, cur->key, val);
                if (val < cur->key) cur = cur->left;
                else cur = cur->right;
            }

            bst.insert(val);
            eventLog.logInsert(-1, -1, 0, val);
            eventLog.logDone("BST insertion complete");

            syncBSTVisuals();
            eventController.loadFromEventLog(eventLog);
            eventController.start();

            levelManager.scoreSystem.addAlarmStep(2);
            levelManager.notifyAction("insert", val);

            char buf[96];
            snprintf(buf, sizeof(buf), "BST INSERT: Key %d grafted into neural hive.", val);
            hud.setStatus(buf, "Synaptic path traced and new pod illuminated.");
        } else if (isAct("act_3")) {
            if (avl.search(val)) {
                levelManager.scoreSystem.addAlarmStep(1);
                char buf[128];
                snprintf(buf, sizeof(buf), "AVL REJECTED: Duplicate key %d exists in vault.", val);
                hud.setStatus(buf, "AVL nodes must have unique keys.");
                return;
            }

            eventLog.clear();
            avl.setEventLog(&eventLog);
            avl.insert(val);

            syncAVLVisuals();
            eventController.loadFromEventLog(eventLog);
            eventController.start();

            levelManager.scoreSystem.addAlarmStep(2);
            levelManager.notifyAction("insert", val);

            char buf[128];
            snprintf(buf, sizeof(buf), "AVL INSERT: Key %d grafted. Height rebalanced (|BF| <= 1).", val);
            hud.setStatus(buf, "Balance factor and subtree size badges updated.");
        } else if (isAct("act_4")) {
            eventLog.clear();
            heap.setEventLog(&eventLog);
            heap.insert(val);

            syncHeapVisuals();
            eventController.loadFromEventLog(eventLog);
            eventController.start();

            levelManager.scoreSystem.addAlarmStep(2);
            levelManager.notifyAction("insert", val);

            char buf[96];
            snprintf(buf, sizeof(buf), "HEAP INSERT: Value %d appended and sifted up.", val);
            hud.setStatus(buf, "Synchronized: Parent >= children invariant restored.");
        }

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleDeleteAction() {
        takeSnapshot();
        int keyToDelete = inputBox.getValue(-1);
        if (keyToDelete == -1) {
            if (selectedNodeIndex != -1 && selectedNodeIndex < layout.nodeCount && layout.nodes[selectedNodeIndex].active) {
                keyToDelete = layout.nodes[selectedNodeIndex].key;
            }
        }

        if (isAct("act_2")) {
            if (bst.isEmpty()) {
                hud.setStatus("BST DELETE: Hive is empty. No pods to delete.");
                return;
            }
            if (keyToDelete == -1 || !bst.search(keyToDelete)) {
                for (int i = layout.nodeCount - 1; i >= 0; i--) {
                    if (layout.nodes[i].active && layout.nodes[i].leftChildIndex == -1 && layout.nodes[i].rightChildIndex == -1) {
                        keyToDelete = layout.nodes[i].key;
                        break;
                    }
                }
                if (keyToDelete == -1 && layout.nodeCount > 0 && layout.nodes[0].active) {
                    keyToDelete = layout.nodes[0].key;
                }
            }

            if (keyToDelete != -1 && bst.search(keyToDelete)) {
                eventLog.clear();
                int succVal = bst.successor(keyToDelete);
                if (succVal != -1) {
                    eventLog.logSwapValue(keyToDelete, succVal, keyToDelete, succVal);
                }
                eventLog.logUnlink(keyToDelete, -1, 0, keyToDelete);
                eventLog.logDone("BST deletion complete");

                bst.deleteNode(keyToDelete);
                syncBSTVisuals();
                selectedNodeIndex = -1;

                eventController.loadFromEventLog(eventLog);
                eventController.start();

                levelManager.scoreSystem.addAlarmStep(3);
                levelManager.notifyAction("delete", keyToDelete);

                char buf[96];
                snprintf(buf, sizeof(buf), "BST DELETE: Severed pod with key %d from hive.", keyToDelete);
                hud.setStatus(buf, "Neural connections reorganized per BST deletion rules.");
            }
        } else if (isAct("act_3")) {
            if (avl.isEmpty()) {
                hud.setStatus("AVL DELETE: Vault is empty. No pods to delete.");
                return;
            }
            if (keyToDelete == -1 || !avl.search(keyToDelete)) {
                for (int i = layout.nodeCount - 1; i >= 0; i--) {
                    if (layout.nodes[i].active && layout.nodes[i].leftChildIndex == -1 && layout.nodes[i].rightChildIndex == -1) {
                        keyToDelete = layout.nodes[i].key;
                        break;
                    }
                }
                if (keyToDelete == -1 && layout.nodeCount > 0 && layout.nodes[0].active) {
                    keyToDelete = layout.nodes[0].key;
                }
            }

            if (keyToDelete != -1 && avl.search(keyToDelete)) {
                eventLog.clear();
                avl.setEventLog(&eventLog);
                avl.deleteNode(keyToDelete);

                syncAVLVisuals();
                selectedNodeIndex = -1;

                eventController.loadFromEventLog(eventLog);
                eventController.start();

                levelManager.scoreSystem.addAlarmStep(3);
                levelManager.notifyAction("delete", keyToDelete);

                char buf[96];
                snprintf(buf, sizeof(buf), "AVL DELETE: Severed pod with key %d.", keyToDelete);
                hud.setStatus(buf, "Tree automatically rebalanced with rotations if needed.");
            }
        }

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleSearchAction() {
        int targetKey = inputBox.getValue(-1);
        if (targetKey == -1) {
            if (selectedNodeIndex != -1 && selectedNodeIndex < layout.nodeCount && layout.nodes[selectedNodeIndex].active) {
                targetKey = layout.nodes[selectedNodeIndex].key;
            } else if (layout.nodeCount > 0 && layout.nodes[0].active) {
                targetKey = layout.nodes[0].key;
            } else {
                hud.setStatus("SEARCH: Structure is empty.");
                return;
            }
        }

        resetAllVisualStates();
        eventLog.clear();

        if (isAct("act_2")) {
            BSTNode* cur = bst.getRoot();
            bool found = false;
            while (cur != nullptr) {
                eventLog.logCompare(cur->id, -1, cur->key, targetKey);
                if (targetKey == cur->key) {
                    found = true;
                    break;
                } else if (targetKey < cur->key) {
                    cur = cur->left;
                } else {
                    cur = cur->right;
                }
            }
            levelManager.scoreSystem.addAlarmStep(2);
            if (found) {
                eventLog.logVisit(cur->id, targetKey);
                eventLog.logDone("Search target pod located!");
                char buf[96];
                snprintf(buf, sizeof(buf), "SEARCH HIT: Pod with key %d located in BST!", targetKey);
                hud.setStatus(buf, "Synaptic path illuminated to target pod.");
                levelManager.notifyAction("search", targetKey);
            } else {
                eventLog.logDone("Target key not in BST.");
                char buf[96];
                snprintf(buf, sizeof(buf), "SEARCH MISS: Key %d does not exist in BST.", targetKey);
                hud.setStatus(buf, "Search reached dead-end leaf branch.");
            }
            eventController.loadFromEventLog(eventLog);
            eventController.start();
        } else if (isAct("act_3")) {
            bool found = avl.search(targetKey);
            for (int i = 0; i < layout.nodeCount; i++) {
                if (layout.nodes[i].active && layout.nodes[i].key == targetKey) {
                    layout.nodes[i].setState("highlighted");
                    selectedNodeIndex = i;
                    break;
                }
            }
            levelManager.scoreSystem.addAlarmStep(1);
            if (found) {
                char buf[96];
                snprintf(buf, sizeof(buf), "AVL SEARCH HIT: Key %d located in O(log N) time!", targetKey);
                hud.setStatus(buf, "Bioluminescent synaptic pod illuminated.");
                levelManager.notifyAction("search", targetKey);
            } else {
                char buf[96];
                snprintf(buf, sizeof(buf), "AVL SEARCH MISS: Key %d not found in AVL vault.", targetKey);
                hud.setStatus(buf, "Key outside current range.");
            }
        }

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleRangeAction() {
        if (!isAct("act_2")) return;

        resetAllVisualStates();
        int lo = 30;
        int hi = 70;
        int matchCount = 0;

        for (int i = 0; i < layout.nodeCount; i++) {
            if (!layout.nodes[i].active) continue;
            if (layout.nodes[i].key >= lo && layout.nodes[i].key <= hi) {
                layout.nodes[i].setState("highlighted");
                matchCount++;
            } else {
                layout.nodes[i].setState("inactive");
            }
        }

        levelManager.scoreSystem.addAlarmStep(2);
        levelManager.notifyAction("range", 0);

        char buf[96];
        snprintf(buf, sizeof(buf), "RANGE QUERY [%d, %d]: %d pods glowing in hive.", lo, hi, matchCount);
        hud.setStatus(buf, "Subtrees outside range dimmed to inactive purple membranes.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleAVLKthAction() {
        if (!isAct("act_3") || avl.isEmpty()) {
            hud.setStatus("ORDER STATISTIC: AVL Tree is empty.");
            return;
        }
        int total = avl.getRootSize();
        int k = (total >= 3) ? 3 : 1;
        int key = avl.kth(k);

        resetAllVisualStates();
        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].active && layout.nodes[i].key == key) {
                layout.nodes[i].setState("highlighted");
                selectedNodeIndex = i;
                inputBox.setValue(key);
                break;
            }
        }

        levelManager.scoreSystem.addAlarmStep(1);
        levelManager.notifyAction("kth", k);

        char buf[96];
        snprintf(buf, sizeof(buf), "AVL K-TH ELEMENT (k = %d): Key %d identified in O(log N)!", k, key);
        hud.setStatus(buf, "Located directly via subtreeSize badges without full traversal.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleAVLRankAction() {
        if (!isAct("act_3") || avl.isEmpty()) {
            hud.setStatus("RANK QUERY: AVL Tree is empty.");
            return;
        }
        int targetKey = inputBox.getValue(-1);
        if (targetKey == -1) {
            if (selectedNodeIndex != -1 && selectedNodeIndex < layout.nodeCount && layout.nodes[selectedNodeIndex].active) {
                targetKey = layout.nodes[selectedNodeIndex].key;
            } else if (avl.getRoot() != nullptr) {
                targetKey = avl.getRoot()->key;
            }
        }

        int r = avl.rank(targetKey);
        levelManager.scoreSystem.addAlarmStep(1);
        levelManager.notifyAction("rank", targetKey);

        char buf[96];
        snprintf(buf, sizeof(buf), "AVL RANK: Pod with key %d has Rank %d (1-based position).", targetKey, r);
        hud.setStatus(buf, "Computed using augmented subtree size in O(log N).");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void runRotationDemo(const char* rotType) {
        if (!isAct("act_3")) return;
        eventController.clear();
        selectedNodeIndex = -1;

        avl.setEventLog(&eventLog);
        while (!avl.isEmpty()) {
            avl.deleteNode(avl.getRoot()->key);
        }
        eventLog.clear();

        levelManager.scoreSystem.addAlarmStep(2);

        if (strcmp(rotType, "LL") == 0) {
            avl.insert(30);
            avl.insert(20);
            eventLog.clear();
            avl.insert(10);
            hud.setStatus("AVL DEMO (LL Rotation): Inserted 10 -> BF(30) = +2. Rebalanced via Right Rotation!",
                          "Pivot 20 promoted to root. Heights and badges balanced.");
            levelManager.notifyAction("demo_ll", 0);
        } else if (strcmp(rotType, "RR") == 0) {
            avl.insert(10);
            avl.insert(20);
            eventLog.clear();
            avl.insert(30);
            hud.setStatus("AVL DEMO (RR Rotation): Inserted 30 -> BF(10) = -2. Rebalanced via Left Rotation!",
                          "Pivot 20 promoted to root. Heights and badges balanced.");
            levelManager.notifyAction("demo_rr", 0);
        } else if (strcmp(rotType, "LR") == 0) {
            avl.insert(30);
            avl.insert(10);
            eventLog.clear();
            avl.insert(20);
            hud.setStatus("AVL DEMO (LR Rotation): Inserted 20 -> Left-Right Double Rotation executed!",
                          "Rotated left around 10, then right around 30. Pivot 20 promoted to root.");
            levelManager.notifyAction("demo_lr", 0);
        } else if (strcmp(rotType, "RL") == 0) {
            avl.insert(10);
            avl.insert(30);
            eventLog.clear();
            avl.insert(20);
            hud.setStatus("AVL DEMO (RL Rotation): Inserted 20 -> Right-Left Double Rotation executed!",
                          "Rotated right around 30, then left around 10. Pivot 20 promoted to root.");
            levelManager.notifyAction("demo_rl", 0);
        }

        syncAVLVisuals();
        eventController.loadFromEventLog(eventLog);
        eventController.start();

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleMirrorAction() {
        if (!isAct("act_1")) return;
        takeSnapshot();
        binTree.mirror(binTree.root);
        syncBinaryTreeVisuals();
        levelManager.scoreSystem.addAlarmStep(2);
        levelManager.notifyAction("mirror", 0);

        hud.setStatus("TREE MIRRORED: Inverted all left and right child connections.",
                      "Bilateral reflection applied across the central trunk.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleHeightDiameterAction() {
        if (!isAct("act_1")) return;
        int h = binTree.height(binTree.root);
        int d = binTree.diameter();

        resetAllVisualStates();
        for (int i = 0; i < layout.nodeCount; i++) {
            if (layout.nodes[i].active && (layout.nodes[i].leftChildIndex == -1 || layout.nodes[i].rightChildIndex == -1)) {
                layout.nodes[i].setState("highlighted");
            }
        }
        levelManager.scoreSystem.addAlarmStep(2);
        levelManager.notifyAction("height_diam", 0);

        char buf[96];
        snprintf(buf, sizeof(buf), "TREE METRICS: Height = %d levels, Diameter = %d edges.", h, d);
        hud.setStatus(buf, "Longest boundary branches illuminated.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleLCAPathAction() {
        if (!isAct("act_1")) return;
        int a = 12;
        int b = 37;
        if (selectedNodeIndex != -1 && selectedNodeIndex < layout.nodeCount && layout.nodes[selectedNodeIndex].active) {
            a = layout.nodes[selectedNodeIndex].key;
            b = 62;
        }

        int lcaVal = binTree.lca(a, b);
        int pathLen = binTree.pathLength(a, b);

        resetAllVisualStates();
        for (int i = 0; i < layout.nodeCount; i++) {
            if (!layout.nodes[i].active) continue;
            if (layout.nodes[i].key == lcaVal) {
                layout.nodes[i].setState("target");
            } else if (layout.nodes[i].key == a || layout.nodes[i].key == b) {
                layout.nodes[i].setState("highlighted");
            }
        }

        levelManager.scoreSystem.addAlarmStep(2);
        levelManager.notifyAction("lca", 0);

        char buf[128];
        snprintf(buf, sizeof(buf), "LCA & PATH: LCA(%d, %d) = %d | Path Distance = %d edges.", a, b, lcaVal, pathLen);
        hud.setStatus(buf, "LCA pod glows golden target; endpoints glow electric cyan.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleSymmetryAction() {
        if (!isAct("act_1")) return;
        bool sym = binTree.isSymmetric();
        levelManager.scoreSystem.addAlarmStep(1);
        if (sym) {
            hud.setStatus("SYMMETRY CHECK: Tree is strictly symmetric!", "Left and right subtrees are mirror images.");
        } else {
            hud.setStatus("SYMMETRY CHECK: Tree is asymmetric.", "Left and right subtrees have structural or value differences.");
        }
    }

    void handleReconstructAction() {
        if (!isAct("act_1")) return;
        takeSnapshot();
        int pre[] = {50, 25, 12, 37, 75, 62, 88};
        int in[]  = {12, 25, 37, 50, 62, 75, 88};
        binTree.reconstructFromPreIn(pre, in, 7);
        syncBinaryTreeVisuals();
        levelManager.scoreSystem.addAlarmStep(3);
        levelManager.notifyAction("reconstruct", 0);

        hud.setStatus("TREE RECONSTRUCTION: Rebuilt 7-node tree from Preorder + Inorder sequences.",
                      "Original neural blueprint regenerated perfectly.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleHeapExtractAction() {
        if (!isAct("act_4") || heap.isEmpty()) {
            hud.setStatus("EXTRACT MAX: Heap is empty. No elements to extract.");
            return;
        }
        takeSnapshot();
        eventLog.clear();
        heap.setEventLog(&eventLog);
        int maxVal = heap.extractMax();

        syncHeapVisuals();
        eventController.loadFromEventLog(eventLog);
        eventController.start();

        levelManager.scoreSystem.addAlarmStep(3);
        levelManager.notifyAction("extract_max", 0);

        char buf[96];
        snprintf(buf, sizeof(buf), "EXTRACT MAX: Retrieved highest priority value %d.", maxVal);
        hud.setStatus(buf, "Root severed, last element promoted and sifted down.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleHeapBuildAction() {
        if (!isAct("act_4")) return;
        takeSnapshot();
        eventLog.clear();
        heap.setEventLog(&eventLog);
        int rawArr[] = {15, 65, 30, 85, 45, 95, 20, 55};
        heap.buildMaxHeap(rawArr, 8);

        syncHeapVisuals();
        eventController.loadFromEventLog(eventLog);
        eventController.start();

        levelManager.scoreSystem.addAlarmStep(4);
        levelManager.notifyAction("build_heap", 0);

        hud.setStatus("BUILD MAX HEAP: Floyd's algorithm executed in O(N) time.",
                      "Internal parent nodes sifted down from lowest levels to root.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleHeapSortAction() {
        if (!isAct("act_4") || heap.isEmpty()) {
            hud.setStatus("HEAP SORT: Heap is empty.");
            return;
        }
        takeSnapshot();
        eventLog.clear();
        heap.setEventLog(&eventLog);
        heap.heapSort();

        syncHeapVisuals();
        eventController.loadFromEventLog(eventLog);
        eventController.start();

        levelManager.scoreSystem.addAlarmStep(5);
        levelManager.notifyAction("heap_sort", 0);

        hud.setStatus("HEAP SORT: Sorting sequence animated via repeated extract-max.",
                      "Synchronized 1D array displays extracted elements in final sorted order.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleHeapPeekAction() {
        if (!isAct("act_4") || heap.isEmpty()) {
            hud.setStatus("PEEK MAX: Heap is empty.");
            return;
        }
        int maxVal = heap.peekMax();
        selectedNodeIndex = 0;
        char buf[96];
        snprintf(buf, sizeof(buf), "PEEK MAX: Priority root element is %d (Index [0]).", maxVal);
        hud.setStatus(buf, "Accessed in O(1) time at index 0 of sequential array.");
    }

    void handleHeapCheckAction() {
        if (!isAct("act_4")) return;
        bool valid = heap.isMaxHeap();
        if (valid) {
            hud.setStatus("HEAP VALIDATION: Valid Max-Heap! Invariant satisfied: parent >= children.",
                          "Priority property verified across all active cells.");
        } else {
            hud.setStatus("HEAP VALIDATION: Array is sorted or not currently in heap order.");
        }
    }

    void handleHeapSiftDown() {
        if (!isAct("act_4") || heap.isEmpty()) return;
        takeSnapshot();
        eventLog.clear();
        heap.setEventLog(&eventLog);
        heap.siftDown(0);

        syncHeapVisuals();
        eventController.loadFromEventLog(eventLog);
        eventController.start();

        levelManager.scoreSystem.addAlarmStep(2);
        levelManager.notifyAction("sift_down", 0);
        hud.setStatus("SIFT DOWN [0]: Root pod sifted down to maintain max-heap invariant.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleHeapSiftUp() {
        if (!isAct("act_4") || heap.isEmpty()) return;
        int lastIdx = heap.getSize() - 1;
        if (lastIdx <= 0) return;

        takeSnapshot();
        eventLog.clear();
        heap.setEventLog(&eventLog);
        heap.siftUp(lastIdx);

        syncHeapVisuals();
        eventController.loadFromEventLog(eventLog);
        eventController.start();

        levelManager.scoreSystem.addAlarmStep(2);
        levelManager.notifyAction("sift_up", 0);
        hud.setStatus("SIFT UP [end]: Last pod sifted up to maintain max-heap invariant.");

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    int mapAct25AVLNodeToVisual(AVLNode* node) {
        if (node == nullptr) return -1;
        int idx = layout.allocateNode(node->key);
        if (idx == -1) return -1;
        layout.nodes[idx].balanceFactor = act2.lvl25.avlTree.balanceFactor(node);
        layout.nodes[idx].subtreeSize = act2.lvl25.avlTree.subtreeSize(node);
        if (node->left != nullptr) {
            int li = mapAct25AVLNodeToVisual(node->left);
            layout.nodes[idx].leftChildIndex = li;
            if (li != -1) layout.nodes[li].parentIndex = idx;
        }
        if (node->right != nullptr) {
            int ri = mapAct25AVLNodeToVisual(node->right);
            layout.nodes[idx].rightChildIndex = ri;
            if (ri != -1) layout.nodes[ri].parentIndex = idx;
        }
        return idx;
    }

    void completeAct25AndAdvance() {
        // Stage B presents the lesson; progression happens only after the player's choice.
        act2.completeLevel25(levelManager.scoreSystem);
        levelManager.notifyAction("boss", 0);

        if (strcmp(levelManager.playState, "victory") != 0 && levelManager.getCurrentLevel()) {
            strncpy(levelManager.playState, "victory", sizeof(levelManager.playState) - 1);
            const int curIdx = levelManager.currentLevelIndex;
            if (curIdx >= 0 && curIdx < LevelManager::TOTAL_LEVELS) {
                levelManager.levels[curIdx].isCompleted = true;
                levelManager.levels[curIdx].starsEarned = act2.lvl25.insertionRecord.stars;
                levelManager.levels[curIdx].bestScore = act2.lvl25.insertionRecord.score;
                levelManager.unlockNextLevel();
            }
        }

        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);

        const int nextIdx = levelManager.currentLevelIndex + 1;
        if (nextIdx < LevelManager::TOTAL_LEVELS && levelManager.levels[nextIdx].isUnlocked) {
            loadCampaignLevel(nextIdx);
        } else {
            setScreenState("campaign_map");
            setupButtons();
        }
    }

    void handleAct25StageC() {
        // Stage C: Build AVL from same values and show educational explanation
        layout.clear();
        mapAct25AVLNodeToVisual(act2.lvl25.avlTree.getRoot());
        layout.updateLayout(1280.0f, true);

        setupButtons();
        hud.setStatus("SEARCH IMPOSSIBLE: The skewed BST needs 7 comparisons to reach 70, but the alarm allows only 3.",
                      "This unbalanced BST is O(n); choose Advance to Act 3 to continue.");
    }

    void handleButtonClick(int actionId) {
        // Navigation actions
        if (actionId == 210) { // Campaign Mode
            setScreenState("campaign_map");
            setupButtons();
            return;
        } else if (actionId == 211) { // Sandbox Mode
            setScreenState("sandbox");
            setupDefaultAct1Tree();
            setupButtons();
            hud.setStatus("SANDBOX MODE: Free-form Neural Laboratory.",
                          "Freely switch acts and experiment with any data structure without level limits.");
            return;
        } else if (actionId == 212) { // Endless Mode
            setScreenState("endless");
            setupDefaultAct3AVL();
            endlessTimer = 0.0f;
            endlessWave = 1;
            setupButtons();
            hud.setStatus("ENDLESS MODE: Continuous Neural Swarm Defense.",
                          "Maintain valid AVL tree properties under continuous insertions!");
            return;
        } else if (actionId == 213) { // Quit
            exit(0);
        } else if (actionId == 200) { // Back to Menu
            setScreenState("menu");
            setupButtons();
            return;
        } else if (actionId == 201) { // Hint
            handleHint();
            return;
        } else if (actionId == 202) { // Undo
            handleUndo();
            return;
        } else if (actionId == 203) { // Next Level
            if (isScreen("level_play") && isAct("act_2") && act2.lvl25.stage == 2) {
                completeAct25AndAdvance();
                return;
            }
            int nextIdx = levelManager.currentLevelIndex + 1;
            if (nextIdx < LevelManager::TOTAL_LEVELS && levelManager.levels[nextIdx].isUnlocked) {
                loadCampaignLevel(nextIdx);
            } else {
                setScreenState("campaign_map");
                setupButtons();
            }
            return;
        } else if (actionId == 204) { // Retry Level
            loadCampaignLevel(levelManager.currentLevelIndex);
            return;
        } else if (actionId == 205) { // Campaign Map
            setScreenState("campaign_map");
            setupButtons();
            return;
        }

        // Campaign Map Act tabs (311 to 314)
        if (actionId >= 311 && actionId <= 314) {
            campaignSelectedAct = actionId - 310;
            setupButtons();
            return;
        }

        // Campaign Map Level selection (301 to 305)
        if (actionId >= 301 && actionId <= 305) {
            int selectedLvlIdx = (campaignSelectedAct - 1) * 5 + (actionId - 301);
            if (levelManager.levels[selectedLvlIdx].isUnlocked) {
                loadCampaignLevel(selectedLvlIdx);
            } else {
                hud.setStatus("CHAMBER LOCKED: Complete preceding vaults to unlock this level.");
            }
            return;
        }

        // Act Switchers
        switch (actionId) {
            case 101: setupDefaultAct1Tree(); setupButtons(); break;
            case 102: setupDefaultAct2BST(); setupButtons(); break;
            case 103: setupDefaultAct3AVL(); setupButtons(); break;
            case 104: setupDefaultAct4Heap(); setupButtons(); break;

            case 50: inputBox.adjustValue(-5); break;
            case 51: inputBox.adjustValue(+5); break;

            case 1: // Preorder
            {
                resetAllVisualStates();
                eventLog.clear();
                buildPreorderEvents(layout.findRootIndex());
                eventLog.logDone("Preorder traversal complete");
                eventController.loadFromEventLog(eventLog);
                eventController.start();
                levelManager.scoreSystem.addAlarmStep(layout.nodeCount);
                levelManager.notifyAction("preorder", 0);
                char seqBuf[128] = {0};
                int offset = 0;
                collectPreorderSequence(layout.findRootIndex(), seqBuf, sizeof(seqBuf), offset);
                char buf[160];
                snprintf(buf, sizeof(buf), "PREORDER: %s", seqBuf);
                hud.setStatus(buf, "Impulse order: Root -> Left -> Right.");
                break;
            }
            case 2: // Inorder
            {
                resetAllVisualStates();
                eventLog.clear();
                buildInorderEvents(layout.findRootIndex());
                eventLog.logDone("Inorder traversal complete");
                eventController.loadFromEventLog(eventLog);
                eventController.start();
                levelManager.scoreSystem.addAlarmStep(layout.nodeCount);
                levelManager.notifyAction("inorder", 0);
                char seqBuf[128] = {0};
                int offset = 0;
                collectInorderSequence(layout.findRootIndex(), seqBuf, sizeof(seqBuf), offset);
                char buf[160];
                snprintf(buf, sizeof(buf), "INORDER (Sorted): %s", seqBuf);
                hud.setStatus(buf, "Impulse order: Left -> Root -> Right (Strictly sorted order).");
                break;
            }
            case 3: // Postorder
            {
                resetAllVisualStates();
                eventLog.clear();
                buildPostorderEvents(layout.findRootIndex());
                eventLog.logDone("Postorder traversal complete");
                eventController.loadFromEventLog(eventLog);
                eventController.start();
                levelManager.scoreSystem.addAlarmStep(layout.nodeCount);
                levelManager.notifyAction("postorder", 0);
                char seqBuf[128] = {0};
                int offset = 0;
                collectPostorderSequence(layout.findRootIndex(), seqBuf, sizeof(seqBuf), offset);
                char buf[160];
                snprintf(buf, sizeof(buf), "POSTORDER: %s", seqBuf);
                hud.setStatus(buf, "Impulse order: Left -> Right -> Root.");
                break;
            }
            case 4: // Level Order
            {
                resetAllVisualStates();
                eventLog.clear();
                buildLevelOrderEvents();
                eventLog.logDone("Level-order traversal complete");
                eventController.loadFromEventLog(eventLog);
                eventController.start();
                levelManager.scoreSystem.addAlarmStep(layout.nodeCount);
                levelManager.notifyAction("level_order", 0);
                char seqBuf[128] = {0};
                int offset = 0;
                collectLevelOrderSequence(seqBuf, sizeof(seqBuf), offset);
                char buf[160];
                snprintf(buf, sizeof(buf), "LEVEL ORDER: %s", seqBuf);
                hud.setStatus(buf, "Impulse order: Breadth-First across depth layers.");
                break;
            }
            case 5: handleMirrorAction(); break;
            case 6: handleHeightDiameterAction(); break;
            case 7: handleLCAPathAction(); break;
            case 8: handleSymmetryAction(); break;
            case 9: handleReconstructAction(); break;

            case 52:
            {
                if (isScreen("level_play") && isAct("act_1") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 5) {
                    char fb[160] = {0};
                    bool ok = act1.handleAutoMirror(levelManager.scoreSystem, fb, sizeof(fb));
                    syncBinaryTreeVisuals();
                    hud.setStatus(fb, "Bilateral Reflection (Auto)");
                    if (ok) {
                        levelManager.notifyAction("mirror", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                        setupButtons();
                    }
                }
                break;
            }
            case 60:
            {
                if (isScreen("level_play") && isAct("act_1") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 3) {
                    char fb[160] = {0};
                    int val = inputBox.getValue(-1);
                    bool ok = act1.submitLevel13Input(val, levelManager.scoreSystem, fb, sizeof(fb));
                    hud.setStatus(fb, "Hive Metrics Analysis");
                    if (ok) {
                        inputBox.setValue(0);
                        if (act1.lvl13.isLevelComplete) {
                            levelManager.notifyAction("height_diam", 0);
                            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                            setupButtons();
                        }
                    } else {
                        if (levelManager.scoreSystem.isAlarmBreached()) {
                            levelManager.notifyAction("fail", 0);
                            setupButtons();
                        }
                    }
                }
                break;
            }

            case 70: // Level 2.1: Target Not Found
            {
                if (isScreen("level_play") && isAct("act_2")) {
                    char fb[160] = {0};
                    bool ok = act2.handleLevel21TargetNotFound(levelManager.scoreSystem, fb, sizeof(fb));
                    hud.setStatus(fb, "Target Not Found Verification");
                    if (ok) {
                        int tgt = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->targetValue : 0;
                        levelManager.notifyAction("search", tgt);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                        setupButtons();
                    } else {
                        if (levelManager.scoreSystem.isAlarmBreached()) {
                            levelManager.notifyAction("fail", 0);
                            setupButtons();
                        }
                    }
                }
                break;
            }

            case 71: // Level 2.3: Case Leaf
            case 72: // Level 2.3: Case One Child
            case 73: // Level 2.3: Case Two Children
            {
                if (isScreen("level_play") && isAct("act_2") && strcmp(act2.lvl23.phase, "case_select") == 0) {
                    int caseChoice = (actionId == 71) ? 0 : ((actionId == 72) ? 1 : 2);
                    char fb[160] = {0};
                    int delKey = act2.lvl23.deleteKey;
                    bool ok = act2.selectDeletionCase(caseChoice, fb, sizeof(fb));
                    hud.setStatus(fb, "BST Deletion Case Selection");
                    if (ok) {
                        if (caseChoice == 0) {
                            // Stage 1: Leaf deletion (20)
                            eventLog.clear();
                            int vIdx = findVisualNodeByKey(delKey);
                            if (vIdx != -1) eventLog.logUnlink(vIdx, -1, 0, delKey);
                            eventLog.logDone("Stage 1 (Leaf): Node 20 unlinked and removed");
                            eventController.loadFromEventLog(eventLog);
                            eventController.start();

                            if (bst.search(delKey)) bst.deleteNode(delKey);
                            syncBSTVisuals();
                            setupButtons();
                        } else if (caseChoice == 1) {
                            // Stage 2: One-child deletion (60)
                            eventLog.clear();
                            int vIdx = findVisualNodeByKey(delKey);
                            int cIdx = findVisualNodeByKey(65);
                            if (vIdx != -1) eventLog.logUnlink(vIdx, -1, 0, delKey);
                            if (cIdx != -1) eventLog.logVisit(cIdx, 65);
                            eventLog.logDone("Stage 2 (One Child): Node 60 deleted, child 65 promoted to 70");
                            eventController.loadFromEventLog(eventLog);
                            eventController.start();

                            if (bst.search(delKey)) bst.deleteNode(delKey);
                            syncBSTVisuals();
                            setupButtons();
                        } else if (caseChoice == 2) {
                            // Stage 3: Two-child case (50) -> highlight target 50 and successor 60
                            int targetIdx = findVisualNodeByKey(delKey);
                            if (targetIdx != -1) {
                                layout.nodes[targetIdx].setState("target");
                            }
                            int succIdx = findVisualNodeByKey(60);
                            if (succIdx != -1) {
                                layout.nodes[succIdx].setState("highlighted");
                            }
                            setupButtons();
                        }
                    }
                }
                break;
            }

            case 77: // Level 2.3: Continue to next stage
            {
                if (isScreen("level_play") && isAct("act_2") && strcmp(act2.lvl23.phase, "stage_cleared") == 0) {
                    act2.advanceLevel23Stage();
                    syncBSTVisuals();
                    applyAct2InitialVisuals(3);
                    setupButtons();
                    hud.setStatus(act2.lvl23.statusMsg, "BST Deletion");
                }
                break;
            }

            case 78: // Level 2.3: Confirm Successor (60) for Two Children
            {
                if (isScreen("level_play") && isAct("act_2") && strcmp(act2.lvl23.phase, "successor_select") == 0) {
                    char fb[160] = {0};
                    bool ok = act2.handleLevel23SuccessorClick(60, levelManager.scoreSystem, fb, sizeof(fb));
                    hud.setStatus(fb, "Inorder Successor Swap");
                    if (ok) {
                        eventLog.clear();
                        int n50 = findVisualNodeByKey(50);
                        int n60 = findVisualNodeByKey(60);
                        if (n50 != -1 && n60 != -1) {
                            eventLog.logSwapValue(n50, n60, 50, 60);
                            eventLog.logUnlink(n60, -1, 0, 50);
                        }
                        eventLog.logDone("Stage 3: Successor 60 swapped with root 50, original successor deleted");
                        eventController.loadFromEventLog(eventLog);
                        eventController.start();

                        if (bst.search(50)) bst.deleteNode(50);
                        syncBSTVisuals();
                        levelManager.notifyAction("delete", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                        setupButtons();
                    }
                }
                break;
            }

            case 74: // Level 2.4: Start Raid [30, 65]
            {
                if (isScreen("level_play") && isAct("act_2") && !act2.lvl24.isExecuted) {
                    char fb[160] = {0};
                    bool ok = act2.executeRangeQuery(levelManager.scoreSystem, fb, sizeof(fb));
                    hud.setStatus(fb, "BST Range Raid [30, 65]");
                    if (ok) {
                        eventLog.clear();
                        int n50 = findVisualNodeByKey(50);
                        int n30 = findVisualNodeByKey(30);
                        int n20 = findVisualNodeByKey(20);
                        int n40 = findVisualNodeByKey(40);
                        int n70 = findVisualNodeByKey(70);
                        int n60 = findVisualNodeByKey(60);
                        int n65 = findVisualNodeByKey(65);
                        int n80 = findVisualNodeByKey(80);

                        eventLog.logVisit(n50, 50);
                        eventLog.logCompare(n30, -1, 30, 30);
                        eventLog.logHighlightSubtree(n20, "pruned_left");
                        eventLog.logVisit(n30, 30);
                        eventLog.logVisit(n40, 40);
                        eventLog.logCompare(n70, -1, 70, 65);
                        eventLog.logVisit(n70, 70);
                        eventLog.logVisit(n60, 60);
                        eventLog.logVisit(n65, 65);
                        eventLog.logHighlightSubtree(n80, "pruned_right");
                        eventLog.logDone("Range Raid [30, 65] with subtree pruning complete");

                        eventController.loadFromEventLog(eventLog);
                        eventController.start();

                        applyAct2RangeVisuals();
                        levelManager.notifyAction("range", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                        setupButtons();
                    }
                }
                break;
            }

            case 75: // Level 2.5: Boss Insert Next
            {
                if (isScreen("level_play") && isAct("act_2") && act2.lvl25.stage == 0) {
                    char fb[160] = {0};
                    int idx = act2.lvl25.insertedCount;
                    act2.handleLevel25SelectArrayValue(idx, fb, sizeof(fb));
                    bool ok = act2.handleLevel25SlotClick(0, levelManager.scoreSystem, fb, sizeof(fb));
                    syncBSTVisuals();
                    hud.setStatus(fb, "Boss Stage A: Sorted Insertion");
                    if (ok) {
                        if (act2.lvl25.stage == 1) {
                            setupButtons();
                            hud.setStatus("STAGE B: Search Challenge! Search for pod 70 in the skewed corridor.",
                                          "Notice how many comparisons it takes vs O(log n) budget.");
                        } else {
                            setupButtons();
                        }
                    } else {
                        if (levelManager.scoreSystem.isAlarmBreached()) {
                            levelManager.notifyAction("fail", 0);
                            setupButtons();
                        }
                    }
                }
                break;
            }

            case 76: // Level 2.5: Boss Search Next
            {
                if (isScreen("level_play") && isAct("act_2") && act2.lvl25.stage == 1) {
                    char fb[160] = {0};
                    int val = act2.lvl25.currentSearchNodeKey;
                    bool ok = act2.handleLevel25SearchClick(val, levelManager.scoreSystem, fb, sizeof(fb));
                    int vIdx = findVisualNodeByKey(val);
                    if (vIdx != -1) layout.nodes[vIdx].setState("highlighted");
                    hud.setStatus(fb, "Boss Stage B: Skewed Search");
                    if (ok) {
                        if (act2.lvl25.stage == 2) {
                            handleAct25StageC();
                        } else {
                            setupButtons();
                        }
                    }
                }
                break;
            }

            case 10: handleInsertAction(); break;
            case 11: handleSearchAction(); break;
            case 12: handleDeleteAction(); break;
            case 13: handleRangeAction(); break;

            case 31: runRotationDemo("LL"); break;
            case 32: runRotationDemo("RR"); break;
            case 33: runRotationDemo("LR"); break;
            case 34: runRotationDemo("RL"); break;

            // ===================== ACT 3 CAMPAIGN BUTTON HANDLERS =====================
            case 84: // Level 3.4: Predict no_rotation
            {
                if (isScreen("level_play") && isAct("act_3")) {
                    char fb[160] = {0};
                    act3.handleLevel34Prediction("no_rotation", levelManager.scoreSystem, fb, sizeof(fb));
                    syncAVLVisuals();
                    hud.setStatus(fb, "Prediction: no_rotation");
                    if (act3.lvl34.isLevelComplete) {
                        levelManager.notifyAction("insert", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 85: // Level 3.4: Predict LL
            {
                if (isScreen("level_play") && isAct("act_3")) {
                    char fb[160] = {0};
                    act3.handleLevel34Prediction("ll", levelManager.scoreSystem, fb, sizeof(fb));
                    syncAVLVisuals();
                    hud.setStatus(fb, "Prediction: LL");
                    if (act3.lvl34.isLevelComplete) {
                        levelManager.notifyAction("insert", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 86: // Level 3.4: Predict RR
            {
                if (isScreen("level_play") && isAct("act_3")) {
                    char fb[160] = {0};
                    act3.handleLevel34Prediction("rr", levelManager.scoreSystem, fb, sizeof(fb));
                    syncAVLVisuals();
                    hud.setStatus(fb, "Prediction: RR");
                    if (act3.lvl34.isLevelComplete) {
                        levelManager.notifyAction("insert", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 87: // Level 3.4: Predict LR
            {
                if (isScreen("level_play") && isAct("act_3")) {
                    char fb[160] = {0};
                    act3.handleLevel34Prediction("lr", levelManager.scoreSystem, fb, sizeof(fb));
                    syncAVLVisuals();
                    hud.setStatus(fb, "Prediction: LR");
                    if (act3.lvl34.isLevelComplete) {
                        levelManager.notifyAction("insert", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 88: // Level 3.4: Predict RL
            {
                if (isScreen("level_play") && isAct("act_3")) {
                    char fb[160] = {0};
                    act3.handleLevel34Prediction("rl", levelManager.scoreSystem, fb, sizeof(fb));
                    syncAVLVisuals();
                    hud.setStatus(fb, "Prediction: RL");
                    if (act3.lvl34.isLevelComplete) {
                        levelManager.notifyAction("insert", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 89: // Level 3.5: Claim Treasure
            {
                if (isScreen("level_play") && isAct("act_3")) {
                    char fb[160] = {0};
                    bool ok = act3.handleLevel35Claim(levelManager.scoreSystem, fb, sizeof(fb));
                    hud.setStatus(fb, "Order-Statistic Claim");
                    if (ok && act3.lvl35.isLevelComplete) {
                        levelManager.notifyAction("boss", 50);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            // ===================== ACT 4 CAMPAIGN BUTTON HANDLERS =====================
            case 95: // Level 4.1: Confirm Swap
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel41ConfirmSwap(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(1);
                    hud.setStatus(fb, "Bottom-Up Heap Construction");
                    if (ok && act4.lvl41.isLevelComplete) {
                        levelManager.notifyAction("build_heap", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 96: // Level 4.1: No Swap Needed
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel41NoSwapNeeded(levelManager.scoreSystem, fb, sizeof(fb));
                    applyAct4InitialVisuals(1);
                    hud.setStatus(fb, "Inspect Parent & Children");
                    if (ok && act4.lvl41.isLevelComplete) {
                        levelManager.notifyAction("build_heap", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 97: // Act 4: Cancel Selection
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                    act4.cancelSelection();
                    applyAct4InitialVisuals(lvlNum);
                    hud.setStatus("Selection cleared. Select the larger child or choose operation.");
                    setupButtons();
                }
                break;
            }
            case 110: // Level 4.2: Confirm Swap (Round A)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel42ConfirmSwap(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(2);
                    hud.setStatus(fb, "Priority Repair (Sift-Down)");
                    if (ok && act4.lvl42.isLevelComplete) {
                        levelManager.notifyAction("repair", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 111: // Level 4.2: Move Up (Round B)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel42MoveUp(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(2);
                    hud.setStatus(fb, "Priority Repair (Sift-Up)");
                    if (ok && act4.lvl42.isLevelComplete) {
                        levelManager.notifyAction("repair", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 112: // Level 4.3: Extract Maximum
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel43ExtractMax(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(3);
                    hud.setStatus(fb, "Apex Extraction");
                    if (ok && act4.lvl43.isLevelComplete) {
                        levelManager.notifyAction("extract_max", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 113: // Level 4.3: Confirm Swap (Heapify)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel43ConfirmSwap(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(3);
                    hud.setStatus(fb, "Heapify Active Heap");
                    if (ok && act4.lvl43.isLevelComplete) {
                        levelManager.notifyAction("extract_max", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 114: // Level 4.4: Extract Maximum (Race)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel44ExtractMax(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(4);
                    hud.setStatus(fb, "Race Against Guards");
                    if (ok && act4.lvl44.isLevelComplete) {
                        levelManager.notifyAction("race", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 115: // Level 4.4: Confirm Swap (Race)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel44ConfirmSwap(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(4);
                    hud.setStatus(fb, "Race: Heapify Active Heap");
                    if (ok && act4.lvl44.isLevelComplete) {
                        levelManager.notifyAction("race", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 116: // Level 4.5: Confirm Swap Phase A (Build)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel45ConfirmSwap(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(5);
                    hud.setStatus(fb, "Phase A: Build Max Heap");
                    setupButtons();
                    (void)ok;
                }
                break;
            }
            case 117: // Level 4.5: No Swap Needed Phase A (Build)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel45NoSwapNeeded(levelManager.scoreSystem, fb, sizeof(fb));
                    applyAct4InitialVisuals(5);
                    hud.setStatus(fb, "Phase A: Verify Parent");
                    setupButtons();
                    (void)ok;
                }
                break;
            }
            case 118: // Level 4.5: Extract Maximum Phase B (Sort)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel45ExtractMax(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(5);
                    hud.setStatus(fb, "Phase B: Extract Max");
                    if (ok && act4.lvl45.isLevelComplete) {
                        levelManager.notifyAction("boss", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            case 119: // Level 4.5: Confirm Swap Phase B (Sort)
            {
                if (isScreen("level_play") && isAct("act_4")) {
                    char fb[160] = {0};
                    bool ok = act4.handleLevel45ConfirmSwap(levelManager.scoreSystem, fb, sizeof(fb));
                    syncHeapVisuals();
                    applyAct4InitialVisuals(5);
                    hud.setStatus(fb, "Phase B: Sift Down Root");
                    if (ok && act4.lvl45.isLevelComplete) {
                        levelManager.notifyAction("boss", 0);
                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                    }
                    setupButtons();
                }
                break;
            }
            // ===========================================================================

            case 15: handleAVLKthAction(); break;
            case 16: handleAVLRankAction(); break;

            case 40: handleHeapExtractAction(); break;
            case 41: handleHeapBuildAction(); break;
            case 42: handleHeapSortAction(); break;
            case 43: handleHeapPeekAction(); break;
            case 44: handleHeapCheckAction(); break;
            case 45: handleHeapSiftDown(); break;
            case 46: handleHeapSiftUp(); break;

            case 20:
                if (isAct("act_1")) setupDefaultAct1Tree();
                else if (isAct("act_2")) setupDefaultAct2BST();
                else if (isAct("act_3")) setupDefaultAct3AVL();
                else if (isAct("act_4")) setupDefaultAct4Heap();
                break;

            case 28:
                eventController.togglePause();
                setupButtons();
                break;
            case 29:
                eventController.stepBackward(layout);
                break;
            case 30:
                eventController.stepForward(layout);
                break;
            case 35:
                eventController.cycleSpeed();
                setupButtons();
                break;
        }

        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
            setupButtons();
        }
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (event.type == sf::Event::TextEntered) {
            if (!isScreen("menu") && !isScreen("campaign_map")) {
                inputBox.handleTextEntered(event.text.unicode);
            }
        } else if (event.type == sf::Event::MouseMoved) {
            float mx = static_cast<float>(event.mouseMove.x);
            float my = static_cast<float>(event.mouseMove.y);
            for (int i = 0; i < buttonCount; i++) {
                buttons[i].update(mx, my);
            }

            if (isPanning) {
                sf::Vector2i mouseDelta = sf::Vector2i(event.mouseMove.x, event.mouseMove.y) - panStartPos;
                viewCenter.x -= mouseDelta.x * zoomLevel;
                viewCenter.y -= mouseDelta.y * zoomLevel;
                cameraView.setCenter(viewCenter);
                panStartPos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            }

            if (isDraggingAVLNode && draggedAVLNodeIndex >= 0 && draggedAVLNodeIndex < layout.nodeCount &&
                layout.nodes[draggedAVLNodeIndex].active) {
                sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), cameraView);
                VisualNode& draggedNode = layout.nodes[draggedAVLNodeIndex];
                draggedNode.x = worldPos.x;
                draggedNode.y = worldPos.y;
                draggedNode.targetX = worldPos.x;
                draggedNode.targetY = worldPos.y;
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            float mx = static_cast<float>(event.mouseButton.x);
            float my = static_cast<float>(event.mouseButton.y);

            if (event.mouseButton.button == sf::Mouse::Left) {
                inputBox.handleClick(mx, my);

                // Manual AVL repair levels use horizontal pivot drags to choose rotations.
                if (isScreen("level_play") && isAct("act_3") && levelManager.getCurrentLevel() &&
                    !isAct3StageTransitionPending && my > 125.0f && my < 680.0f) {
                    const int lvlNum = levelManager.getCurrentLevel()->levelNumber;
                    const bool canDragRotation =
                        (lvlNum == 2 && !act3.lvl32.isLevelComplete) ||
                        (lvlNum == 3 && !act3.lvl33.isLevelComplete) ||
                        (lvlNum == 5 && act3.lvl35.phase == 0);
                    if (canDragRotation) {
                        sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), cameraView);
                        int nodeIdx = layout.findNodeAt(worldPos.x, worldPos.y);
                        if (nodeIdx != -1) {
                            isDraggingAVLNode = true;
                            draggedAVLNodeIndex = nodeIdx;
                            draggedAVLNodeKey = layout.nodes[nodeIdx].key;
                            draggedAVLLevelNumber = lvlNum;
                            avlDragStartX = worldPos.x;
                            selectedNodeIndex = nodeIdx;
                            layout.nodes[nodeIdx].setState("swapping");
                            char prompt[128];
                            int pivotKey = lvlNum == 2 ? act3.lvl32.pivotKey :
                                           (lvlNum == 3 ? act3.lvl33.pivotKey : 50);
                            snprintf(prompt, sizeof(prompt), "Drag unbalanced pivot pod %d left or right to rotate.", pivotKey);
                            hud.setStatus(prompt, "Release the mouse to apply the rotation.");
                        }
                    }
                }

                bool clickedButton = false;
                for (int i = 0; i < buttonCount; i++) {
                    if (buttons[i].contains(mx, my)) {
                        buttons[i].isPressed = true;
                        handleButtonClick(buttons[i].actionId);
                        clickedButton = true;
                        break;
                    }
                }

                // Heap array cell clicks in Act 4
                if (!clickedButton && isAct("act_4") && (isScreen("level_play") || isScreen("sandbox"))) {
                    int clickedCell = renderer.findHeapArrayCellAt(mx, my, heap.getSize());
                    if (clickedCell != -1) {
                        selectedNodeIndex = clickedCell;
                        inputBox.setValue(heap.get(clickedCell));
                        char buf[96];
                        snprintf(buf, sizeof(buf), "SYNCHRONIZED ARRAY CELL: Index [%d] holds Value %d.",
                                 clickedCell, heap.get(clickedCell));
                        hud.setStatus(buf, "Corresponding tree pod illuminated in neural chamber.");
                        clickedButton = true;
                    }
                }

                // Level 1.2 Token Tray clicks
                if (!clickedButton && isScreen("level_play") && isAct("act_1") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 2) {
                    if (my >= 600.0f && my <= 650.0f) {
                        for (int i = 0; i < act1.lvl12.tokenCount; i++) {
                            float tx = act1.lvl12.tokens[i].trayX;
                            float ty = act1.lvl12.tokens[i].trayY;
                            if (mx >= tx - 25.0f && mx <= tx + 25.0f && my >= ty - 20.0f && my <= ty + 20.0f) {
                                if (!act1.lvl12.tokens[i].placed) {
                                    act1.selectToken(i);
                                    char buf[96];
                                    snprintf(buf, sizeof(buf), "Selected Token %d. Now click an available slot in the tree.", act1.lvl12.tokens[i].value);
                                    hud.setStatus(buf, act1.lvl12.stageName);
                                    clickedButton = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                // Level 1.2 Slot clicks
                if (!clickedButton && isScreen("level_play") && isAct("act_1") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 2) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), cameraView);
                    for (int s = 0; s < act1.lvl12.slotCount; s++) {
                        if (!act1.lvl12.slots[s].isAvailable || act1.lvl12.slots[s].isOccupied) continue;
                        float sx = act1.lvl12.slots[s].x;
                        float sy = act1.lvl12.slots[s].y;
                        float distSq = (worldPos.x - sx) * (worldPos.x - sx) + (worldPos.y - sy) * (worldPos.y - sy);
                        if (distSq <= 30.0f * 30.0f) {
                            char fb[160] = {0};
                            bool ok = act1.placeTokenInSlot(s, levelManager.scoreSystem, fb, sizeof(fb));
                            if (ok) {
                                syncReconVisuals();
                                if (act1.lvl12.isLevelComplete) {
                                    levelManager.notifyAction("reconstruct", 0);
                                    SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                    setupButtons();
                                }
                            } else {
                                if (levelManager.scoreSystem.isAlarmBreached()) {
                                    levelManager.notifyAction("fail", 0);
                                    setupButtons();
                                }
                            }
                            hud.setStatus(fb, act1.lvl12.stageName);
                            clickedButton = true;
                            break;
                        }
                    }
                }

                // Level 2.2 Empty Slot clicks
                if (!clickedButton && isScreen("level_play") && isAct("act_2") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 2) {
                    computeAct2EmptySlots();
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), cameraView);
                    for (int s = 0; s < act2.lvl22.slotCount; s++) {
                        float sx = act2.lvl22.slots[s].x;
                        float sy = act2.lvl22.slots[s].y;
                        float distSq = (worldPos.x - sx) * (worldPos.x - sx) + (worldPos.y - sy) * (worldPos.y - sy);
                        if (distSq <= 28.0f * 28.0f) {
                            char fb[160] = {0};
                            bool ok = act2.handleLevel22SlotClick(s, levelManager.scoreSystem, fb, sizeof(fb));
                            if (ok) {
                                syncBSTVisuals();
                                applyAct2InitialVisuals(2);
                                levelManager.notifyAction("insert", act2.lvl22.insertKey);
                                SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                setupButtons();
                            } else {
                                if (levelManager.scoreSystem.isAlarmBreached()) {
                                    levelManager.notifyAction("fail", 0);
                                    setupButtons();
                                }
                            }
                            hud.setStatus(fb, "Stash Loot: Insertion Position");
                            clickedButton = true;
                            break;
                        }
                    }
                }

                // Heap array cell clicks in Act 4
                if (!clickedButton && isAct("act_4") && (isScreen("level_play") || isScreen("sandbox"))) {
                    float cellW = 54.0f;
                    float cellH = 44.0f;
                    float gap = 8.0f;
                    int hSize = heap.getSize();
                    float totalW = hSize * cellW + (hSize - 1) * gap;
                    float startX = 640.0f - totalW * 0.5f;
                    if (startX < 30.0f) startX = 30.0f;
                    float startY = 595.0f + 6.0f;

                    if (my >= startY && my <= startY + cellH && mx >= startX && mx <= startX + totalW) {
                        int cellIdx = static_cast<int>((mx - startX) / (cellW + gap));
                        if (cellIdx >= 0 && cellIdx < hSize) {
                            float cellLeft = startX + cellIdx * (cellW + gap);
                            if (mx >= cellLeft && mx <= cellLeft + cellW) {
                                selectedNodeIndex = cellIdx;
                                if (isScreen("level_play")) {
                                    int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 0;
                                    char fb[160] = {0};
                                    bool ok = false;
                                    if (lvlNum == 1) {
                                        ok = act4.handleLevel41NodeClick(cellIdx, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 2) {
                                        ok = act4.handleLevel42NodeClick(cellIdx, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 3) {
                                        ok = act4.handleLevel43NodeClick(cellIdx, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 4) {
                                        ok = act4.handleLevel44NodeClick(cellIdx, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 5) {
                                        ok = act4.handleLevel45NodeClick(cellIdx, levelManager.scoreSystem, fb, sizeof(fb));
                                    }
                                    applyAct4InitialVisuals(lvlNum);
                                    if (!ok && levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                    hud.setStatus(fb, "Act 4: Max Heap");
                                    setupButtons();
                                    clickedButton = true;
                                }
                            }
                        }
                    }
                }

                // Tree pod clicks
                if (!clickedButton && !isDraggingAVLNode && !isAct3StageTransitionPending && my > 125.0f && my < 680.0f && (isScreen("level_play") || isScreen("sandbox") || isScreen("endless"))) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), cameraView);
                    int clickedNode = layout.findNodeAt(worldPos.x, worldPos.y);
                    if (clickedNode != -1) {
                        selectedNodeIndex = clickedNode;
                        int val = layout.nodes[clickedNode].key;
                        inputBox.setValue(val);

                        if (isScreen("level_play") && isAct("act_1")) {
                            int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                            char fb[160] = {0};

                            if (lvlNum == 1) {
                                bool ok = act1.handleLevel11Click(val, levelManager.scoreSystem, fb, sizeof(fb));
                                if (ok) {
                                    layout.nodes[clickedNode].setState("highlighted");
                                    if (act1.lvl11.isLevelComplete) {
                                        levelManager.notifyAction("inorder", 0);
                                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                        setupButtons();
                                    } else if (act1.lvl11.visitedCount == 0) {
                                        syncBinaryTreeVisuals();
                                    }
                                } else {
                                    layout.nodes[clickedNode].setState("inactive");
                                    if (levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                }
                                hud.setStatus(fb, act1.lvl11.stages[act1.lvl11.currentStage].displayName);
                            } else if (lvlNum == 3) {
                                bool ok = act1.handleLevel13NodeClick(val, levelManager.scoreSystem, fb, sizeof(fb));
                                if (ok) {
                                    resetAllVisualStates();
                                    layout.nodes[clickedNode].setState("highlighted");
                                    inputBox.setValue(0);
                                } else {
                                    if (levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                }
                                hud.setStatus(fb, "Bottom-Up Height Analysis");
                            } else if (lvlNum == 4) {
                                bool ok = act1.handleLevel14Click(val, levelManager.scoreSystem, fb, sizeof(fb));
                                if (ok) {
                                    if (strcmp(act1.lvl14.phase, "trace_path") == 0) {
                                        layout.nodes[clickedNode].setState("target");
                                    } else {
                                        layout.nodes[clickedNode].setState("highlighted");
                                    }
                                    if (act1.lvl14.isLevelComplete) {
                                        levelManager.notifyAction("lca", 0);
                                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                        setupButtons();
                                    }
                                } else {
                                    if (levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                }
                                hud.setStatus(fb, "Escape Route Meeting Point");
                            } else if (lvlNum == 5) {
                                bool ok = act1.handleLevel15Swap(val, levelManager.scoreSystem, fb, sizeof(fb));
                                if (ok) {
                                    syncBinaryTreeVisuals();
                                    if (act1.lvl15.isLevelComplete) {
                                        levelManager.notifyAction("mirror", 0);
                                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                        setupButtons();
                                    }
                                } else {
                                    if (levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                }
                                hud.setStatus(fb, "Bilateral Reflection (Mirror)");
                            }
                        } else if (isScreen("level_play") && isAct("act_2")) {
                            // ======== ACT 2 NODE CLICK ROUTING ========
                            int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
                            char fb[160] = {0};

                            if (lvlNum == 1) {
                                // Level 2.1: Search Heist — only the current node is clickable
                                bool ok = act2.handleLevel21Click(val, levelManager.scoreSystem, fb, sizeof(fb));
                                if (ok) {
                                    // Reveal the clicked node
                                    layout.nodes[clickedNode].setState("highlighted");
                                    // Mark previous nodes as visited (sorted)
                                    for (int pi = 0; pi < act2.lvl21.pathCount - 1; pi++) {
                                        int pIdx = findVisualNodeByKey(act2.lvl21.path[pi]);
                                        if (pIdx != -1) layout.nodes[pIdx].setState("sorted");
                                    }
                                    if (act2.lvl21.isLevelComplete) {
                                        layout.nodes[clickedNode].setState("target");
                                        levelManager.notifyAction("search", val);
                                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                        setupButtons();
                                    } else if (!act2.lvl21.isNullReached) {
                                        // Highlight the next node to click
                                        int nextIdx = findVisualNodeByKey(act2.lvl21.currentNodeKey);
                                        if (nextIdx != -1) layout.nodes[nextIdx].setState("searching");
                                        setupButtons(); // might show Target Not Found
                                    } else {
                                        // NULL reached — show Target Not Found button
                                        setupButtons();
                                    }
                                } else {
                                    layout.nodes[clickedNode].setState("inactive");
                                    if (levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                }
                                hud.setStatus(fb, "BST Search: Follow comparisons");

                            } else if (lvlNum == 2) {
                                // Level 2.2: Stash Loot — node clicks advance traversal path
                                bool ok = act2.handleLevel22NodeClick(val, levelManager.scoreSystem, fb, sizeof(fb));
                                if (ok) {
                                    layout.nodes[clickedNode].setState("highlighted");
                                    if (act2.lvl22.isLevelComplete) {
                                        syncBSTVisuals();
                                        applyAct2InitialVisuals(2);
                                        levelManager.notifyAction("insert", act2.lvl22.insertKey);
                                        SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                        setupButtons();
                                    }
                                } else {
                                    layout.nodes[clickedNode].setState("inactive");
                                    if (levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                }
                                hud.setStatus(fb, "BST Insert: Navigate to empty slot");

                            } else if (lvlNum == 3) {
                                // Level 2.3: Steal and Collapse
                                if (strcmp(act2.lvl23.phase, "case_select") == 0) {
                                    if (val == act2.lvl23.deleteKey) {
                                        layout.nodes[clickedNode].setState("target");
                                        hud.setStatus("Target pod selected! Select deletion case: Leaf Node, One Child, Two Children.", "BST Deletion");
                                    } else {
                                        hud.setStatus("Target pod is highlighted in red. Select its deletion case below.", "BST Deletion");
                                    }
                                } else if (strcmp(act2.lvl23.phase, "successor_select") == 0) {
                                    // Clicking the inorder successor 60
                                    if (val == act2.lvl23.expectedSuccessor) {
                                        handleButtonClick(78); // Confirm Successor
                                    } else {
                                        layout.nodes[clickedNode].setState("inactive");
                                        hud.setStatus("Not the inorder successor! The successor is the smallest node in the right subtree (60).", "BST Deletion");
                                    }
                                } else if (strcmp(act2.lvl23.phase, "complete") == 0) {
                                    hud.setStatus("All 3 deletion cases completed! Level cleared.", "BST Deletion Complete");
                                }

                            } else if (lvlNum == 5) {
                                // Level 2.5 Stage B: search clicks
                                if (act2.lvl25.stage == 1) {
                                    bool ok = act2.handleLevel25SearchClick(val, levelManager.scoreSystem, fb, sizeof(fb));
                                    if (ok) {
                                        layout.nodes[clickedNode].setState("highlighted");
                                        if (act2.lvl25.stage == 2) {
                                            // Transitioned to Stage C
                                            handleAct25StageC();
                                        } else {
                                            setupButtons();
                                        }
                                    } else {
                                        layout.nodes[clickedNode].setState("inactive");
                                    }
                                    hud.setStatus(fb, "Boss Stage B: Search the skewed BST");
                                }
                            } else {
                                // Fallback: just show pod info
                                char buf[96];
                                snprintf(buf, sizeof(buf), "SELECTED BST POD: Key %d.", val);
                                hud.setStatus(buf, "");
                            }
                        } else {
                            char fb[160] = {0};
                            if (isAct("act_3") && isScreen("level_play")) {
                                int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 0;
                                if (lvlNum == 1) {
                                    // Level 3.1: click unbalanced nodes
                                    bool ok = act3.handleLevel31NodeClick(val, levelManager.scoreSystem, fb, sizeof(fb));
                                    if (ok) {
                                        layout.nodes[clickedNode].setState("highlighted");
                                        if (act3.lvl31.isLevelComplete) {
                                            levelManager.notifyAction("search", 0);
                                            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                                            setupButtons();
                                        }
                                    } else {
                                        layout.nodes[clickedNode].setState("inactive");
                                        if (levelManager.scoreSystem.isAlarmBreached()) {
                                            levelManager.notifyAction("fail", 0);
                                            setupButtons();
                                        }
                                    }
                                    hud.setStatus(fb, "Find all unbalanced nodes (|BF| > 1)");
                                } else if (lvlNum == 5 && act3.lvl35.phase == 1) {
                                    // Level 3.5 Phase B: follow order-statistic search path
                                    bool ok = act3.handleLevel35SearchClick(val, levelManager.scoreSystem, fb, sizeof(fb));
                                    if (ok) {
                                        layout.nodes[clickedNode].setState("highlighted");
                                        setupButtons();
                                    } else {
                                        layout.nodes[clickedNode].setState("inactive");
                                        if (levelManager.scoreSystem.isAlarmBreached()) {
                                            levelManager.notifyAction("fail", 0);
                                            setupButtons();
                                        }
                                    }
                                    hud.setStatus(fb, "Phase B: Follow order-statistic path");
                                } else {
                                    char buf[128];
                                    snprintf(buf, sizeof(buf), "SELECTED AVL POD: Key %d (BF: %+d, SubtreeSize: %d)",
                                             layout.nodes[clickedNode].key,
                                             layout.nodes[clickedNode].balanceFactor,
                                             layout.nodes[clickedNode].subtreeSize);
                                    hud.setStatus(buf, "Badges show balance factor and order-statistic subtree size.");
                                }
                            } else {
                            char buf[96];
                            if (isAct("act_3")) {
                                snprintf(buf, sizeof(buf), "SELECTED AVL POD: Key %d (BF: %+d, SubtreeSize: %d)",
                                         layout.nodes[clickedNode].key,
                                         layout.nodes[clickedNode].balanceFactor,
                                         layout.nodes[clickedNode].subtreeSize);
                                hud.setStatus(buf, "Badges show balance factor and order-statistic subtree size.");
                            } else if (isAct("act_4")) {
                                if (isScreen("level_play")) {
                                    int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 0;
                                    char fb[160] = {0};
                                    bool ok = false;
                                    if (lvlNum == 1) {
                                        ok = act4.handleLevel41NodeClick(clickedNode, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 2) {
                                        ok = act4.handleLevel42NodeClick(clickedNode, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 3) {
                                        ok = act4.handleLevel43NodeClick(clickedNode, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 4) {
                                        ok = act4.handleLevel44NodeClick(clickedNode, levelManager.scoreSystem, fb, sizeof(fb));
                                    } else if (lvlNum == 5) {
                                        ok = act4.handleLevel45NodeClick(clickedNode, levelManager.scoreSystem, fb, sizeof(fb));
                                    }
                                    applyAct4InitialVisuals(lvlNum);
                                    if (!ok && levelManager.scoreSystem.isAlarmBreached()) {
                                        levelManager.notifyAction("fail", 0);
                                        setupButtons();
                                    }
                                    hud.setStatus(fb, "Act 4: Max Heap");
                                    setupButtons();
                                } else {
                                    snprintf(buf, sizeof(buf), "SELECTED HEAP POD: Value %d at Tree/Array Index [%d].",
                                             layout.nodes[clickedNode].key, clickedNode);
                                    hud.setStatus(buf, "Highlighted corresponding cell in 1D memory buffer below.");
                                }
                            } else {
                                snprintf(buf, sizeof(buf), "SELECTED POD: Key %d at index %d.", layout.nodes[clickedNode].key, clickedNode);
                                hud.setStatus(buf, "Click 'Search', 'Delete', or traversals to operate on this pod.");
                            }
                            } // end else (not act_3 level_play)
                        }
                    } else {
                        selectedNodeIndex = -1;
                    }
                }
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                isPanning = true;
                panStartPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left && isDraggingAVLNode) {
                sf::Vector2f releasePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), cameraView);
                const float dragDistance = releasePos.x - avlDragStartX;
                const int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 0;
                const int pivotKey = lvlNum == 2 ? act3.lvl32.pivotKey :
                                     (lvlNum == 3 ? act3.lvl33.pivotKey : 50);
                char fb[160] = {0};

                if (draggedAVLLevelNumber != lvlNum || draggedAVLNodeKey != pivotKey) {
                    snprintf(fb, sizeof(fb), "Drag unbalanced pivot pod %d to perform the rotation.", pivotKey);
                    syncAVLVisuals();
                } else if (dragDistance > 24.0f || dragDistance < -24.0f) {
                    const char* direction = dragDistance > 0.0f ? "right" : "left";
                    bool ok = false;
                    if (lvlNum == 2) {
                        ok = act3.handleLevel32Rotation(direction, levelManager.scoreSystem, fb, sizeof(fb));
                    } else if (lvlNum == 3) {
                        ok = act3.handleLevel33CaseSelect(dragDistance > 0.0f ? "lr" : "rl",
                                                         levelManager.scoreSystem, fb, sizeof(fb));
                    } else if (lvlNum == 5) {
                        ok = act3.handleLevel35Repair(direction, levelManager.scoreSystem, fb, sizeof(fb));
                    }

                    if (ok) {
                        syncAVLVisualsAnimated();
                        if (lvlNum == 2 && act3.lvl32.isLevelComplete) {
                            levelManager.notifyAction("rotate", 0);
                            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                        } else if (lvlNum == 3 && act3.lvl33.isLevelComplete) {
                            levelManager.notifyAction("rotate", 0);
                            SaveManager::saveProgress("levels/savegame.dat", levelManager.levels, LevelManager::TOTAL_LEVELS);
                        } else {
                            isAct3StageTransitionPending = true;
                            pendingAct3StageLevel = lvlNum;
                            act3StageTransitionTimer = 0.0f;
                        }
                    } else {
                        syncAVLVisuals();
                    }
                    if (!ok && levelManager.scoreSystem.isAlarmBreached()) {
                        levelManager.notifyAction("fail", 0);
                        setupButtons();
                    }
                } else {
                    snprintf(fb, sizeof(fb), "Drag pivot pod %d clearly left or right to choose a rotation.", pivotKey);
                    syncAVLVisuals();
                }

                hud.setStatus(fb, "Drag the unbalanced pivot left or right to rotate.");
                isDraggingAVLNode = false;
                draggedAVLNodeIndex = -1;
                draggedAVLNodeKey = -1;
                draggedAVLLevelNumber = 0;
                setupButtons();
            }
            for (int i = 0; i < buttonCount; i++) {
                buttons[i].isPressed = false;
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                isPanning = false;
            }
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                zoomLevel *= 0.9f;
                cameraView.zoom(0.9f);
            } else if (event.mouseWheelScroll.delta < 0) {
                zoomLevel *= 1.1f;
                cameraView.zoom(1.1f);
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                eventController.togglePause();
                setupButtons();
            } else if (event.key.code == sf::Keyboard::Right) {
                eventController.stepForward(layout);
            } else if (event.key.code == sf::Keyboard::Left) {
                eventController.stepBackward(layout);
            } else if (event.key.code == sf::Keyboard::Z) {
                handleUndo();
            } else if (event.key.code == sf::Keyboard::H) {
                handleHint();
            } else if (event.key.code == sf::Keyboard::Escape) {
                if (isScreen("level_play")) {
                    setScreenState("campaign_map");
                    setupButtons();
                } else if (isScreen("campaign_map") || isScreen("sandbox") || isScreen("endless")) {
                    setScreenState("menu");
                    setupButtons();
                }
            } else if (event.key.code == sf::Keyboard::Enter) {
                if (isScreen("level_play") && isAct("act_1") && levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == 3) {
                    handleButtonClick(60);
                }
            } else if (event.key.code == sf::Keyboard::R) {
                viewCenter = sf::Vector2f(640.0f, 360.0f);
                cameraView.setCenter(viewCenter);
                cameraView.setSize(1280.0f, 720.0f);
                zoomLevel = 1.0f;
            }
        }
    }

    void update(float dt) {
        renderer.updateSpores(dt);
        layout.updatePhysics(dt);
        eventController.update(dt, layout);

        if (isAct3StageTransitionPending && isScreen("level_play") && isAct("act_3") &&
            levelManager.getCurrentLevel() && levelManager.getCurrentLevel()->levelNumber == pendingAct3StageLevel) {
            act3StageTransitionTimer += dt;
            if (act3StageTransitionTimer >= 0.8f) {
                if (pendingAct3StageLevel == 2) {
                    act3.setupLevel32Stage(1);
                    syncAVLVisualsAnimated();
                    hud.setStatus(act3.lvl32.statusMsg, "Drag the unbalanced pivot pod 10 left for the RR rotation.");
                } else if (pendingAct3StageLevel == 3) {
                    act3.setupLevel33Stage(1);
                    syncAVLVisualsAnimated();
                    hud.setStatus(act3.lvl33.statusMsg, "Drag the unbalanced pivot pod 10 left for the RL double rotation.");
                }
                isAct3StageTransitionPending = false;
                pendingAct3StageLevel = 0;
            }
        } else if (isAct3StageTransitionPending) {
            isAct3StageTransitionPending = false;
            pendingAct3StageLevel = 0;
        }

        if (!isScreen("menu") && !isScreen("campaign_map")) {
            inputBox.update(0, 0, dt);
        }

        // Sync alarm and score
        if (isScreen("level_play")) {
            hud.alarmSteps = levelManager.scoreSystem.alarmSteps;
            hud.alarmCapacity = levelManager.scoreSystem.alarmCapacity;
            hud.score = levelManager.scoreSystem.currentScore;

            if (levelManager.scoreSystem.isAlarmBreached() && strcmp(levelManager.playState, "in_progress") == 0) {
                strncpy(levelManager.playState, "failed", sizeof(levelManager.playState) - 1);
                setupButtons();
            }
        }

        hud.setAnimStatus(eventController.getState(), eventController.speedMultiplier);

        // Endless mode wave generator
        if (isScreen("endless")) {
            endlessTimer += dt;
            if (endlessTimer >= 4.0f) {
                endlessTimer = 0.0f;
                endlessWave++;
                int rVal = rand() % 90 + 10;
                avl.insert(rVal);
                syncAVLVisuals();
                hud.addAlarmStep(1);
                char buf[96];
                snprintf(buf, sizeof(buf), "ENDLESS WAVE %d: Inserted swarm key %d. Keep AVL balanced!", endlessWave, rVal);
                hud.setStatus(buf, "Automatic rebalancing maintained.");
            }
        }
    }

    void render(sf::RenderWindow& window) {
        renderer.renderBackground(window);
        sf::View defaultView = window.getDefaultView();

        if (isScreen("menu")) {
            // Main Menu Screen
            window.setView(defaultView);

            sf::Text title;
            title.setFont(renderer.font);
            title.setString("VAULT HEIST: THE LIVING VAULT");
            title.setCharacterSize(38);
            title.setStyle(sf::Text::Bold);
            title.setFillColor(sf::Color(64, 224, 208));
            sf::FloatRect tb = title.getLocalBounds();
            title.setOrigin(tb.left + tb.width * 0.5f, tb.top + tb.height * 0.5f);
            title.setPosition(640.0f, 130.0f);
            window.draw(title);

            sf::Text subtitle;
            subtitle.setFont(renderer.font);
            subtitle.setString("Phase 4: Campaign, 20 Chambers, Alarm Defense, Sandbox & Endless Mode");
            subtitle.setCharacterSize(16);
            subtitle.setFillColor(sf::Color(210, 240, 255));
            sf::FloatRect sb = subtitle.getLocalBounds();
            subtitle.setOrigin(sb.left + sb.width * 0.5f, sb.top + sb.height * 0.5f);
            subtitle.setPosition(640.0f, 185.0f);
            window.draw(subtitle);

            for (int i = 0; i < buttonCount; i++) {
                buttons[i].render(window, renderer.font);
            }

            sf::Text footer;
            footer.setFont(renderer.font);
            footer.setString("Controls: Space: Pause | Left/Right: Step | Z: Undo | H: Hint | Esc: Menu | R: Reset Camera");
            footer.setCharacterSize(13);
            footer.setFillColor(sf::Color(140, 150, 170));
            sf::FloatRect fb = footer.getLocalBounds();
            footer.setOrigin(fb.left + fb.width * 0.5f, fb.top + fb.height * 0.5f);
            footer.setPosition(640.0f, 660.0f);
            window.draw(footer);
            return;
        }

        if (isScreen("campaign_map")) {
            // Campaign Map Screen
            window.setView(defaultView);

            sf::Text title;
            title.setFont(renderer.font);
            title.setString("CAMPAIGN MAP - VAULT INFILTRATION");
            title.setCharacterSize(26);
            title.setStyle(sf::Text::Bold);
            title.setFillColor(sf::Color(64, 224, 208));
            sf::FloatRect tb = title.getLocalBounds();
            title.setOrigin(tb.left + tb.width * 0.5f, tb.top + tb.height * 0.5f);
            title.setPosition(640.0f, 75.0f);
            window.draw(title);

            // Render level preview cards
            float lx = 140.0f;
            float ly = 230.0f;
            float lw = 190.0f;
            float lh = 170.0f;
            float lgap = 15.0f;

            int startLvl = (campaignSelectedAct - 1) * 5;
            for (int i = 0; i < 5; i++) {
                int lvlIdx = startLvl + i;
                const LevelDef& ldef = levelManager.levels[lvlIdx];
                float cx = lx + i * (lw + lgap);

                sf::RectangleShape card(sf::Vector2f(lw, lh));
                card.setPosition(cx, ly);
                if (ldef.isUnlocked) {
                    card.setFillColor(sf::Color(25, 12, 35, 230));
                    card.setOutlineColor(ldef.isCompleted ? sf::Color(64, 224, 208) : sf::Color(139, 0, 80));
                } else {
                    card.setFillColor(sf::Color(15, 8, 20, 240));
                    card.setOutlineColor(sf::Color(60, 40, 60));
                }
                card.setOutlineThickness(1.5f);
                window.draw(card);

                // Objective text inside card
                sf::Text desc;
                desc.setFont(renderer.font);
                char descBuf[128];
                snprintf(descBuf, sizeof(descBuf), "Obj: %s", ldef.requiredAction);
                desc.setString(descBuf);
                desc.setCharacterSize(12);
                desc.setFillColor(ldef.isUnlocked ? sf::Color(220, 240, 255) : sf::Color(120, 120, 130));
                desc.setPosition(cx + 10.0f, ly + 55.0f);
                window.draw(desc);

                char optBuf[64];
                snprintf(optBuf, sizeof(optBuf), "Optimal: %d steps", ldef.optimalSteps);
                sf::Text optText;
                optText.setFont(renderer.font);
                optText.setString(optBuf);
                optText.setCharacterSize(11);
                optText.setFillColor(sf::Color(64, 224, 208));
                optText.setPosition(cx + 10.0f, ly + 80.0f);
                window.draw(optText);

                char scoreBuf[64];
                snprintf(scoreBuf, sizeof(scoreBuf), "Best: %d pts", ldef.bestScore);
                sf::Text scText;
                scText.setFont(renderer.font);
                scText.setString(scoreBuf);
                scText.setCharacterSize(11);
                scText.setFillColor(sf::Color(255, 215, 0));
                scText.setPosition(cx + 10.0f, ly + 100.0f);
                window.draw(scText);
            }

            for (int i = 0; i < buttonCount; i++) {
                buttons[i].render(window, renderer.font);
            }
            return;
        }

        // Gameplay (Level Play, Sandbox, Endless)
        window.setView(cameraView);

        bool showAVLBadges = isAct("act_3");
        renderer.renderTree(window, layout, selectedNodeIndex, showAVLBadges);

        // Act 1 Special Renderers (Blueprint slots & tokens for Level 1.2, height badges for Level 1.3)
        if (isScreen("level_play") && isAct("act_1") && levelManager.getCurrentLevel()) {
            int lvlNum = levelManager.getCurrentLevel()->levelNumber;
            if (lvlNum == 2) {
                // Connecting dashed lines from parent to available empty child slots
                for (int s = 0; s < act1.lvl12.slotCount; s++) {
                    if (act1.lvl12.slots[s].isAvailable && !act1.lvl12.slots[s].isOccupied) {
                        int p = act1.lvl12.slots[s].parentSlot;
                        if (p != -1 && act1.lvl12.slots[p].isOccupied) {
                            float px = act1.lvl12.slots[p].x;
                            float py = act1.lvl12.slots[p].y;
                            float cx = act1.lvl12.slots[s].x;
                            float cy = act1.lvl12.slots[s].y;

                            sf::Vertex line[] = {
                                sf::Vertex(sf::Vector2f(px, py), sf::Color(64, 224, 208, 140)),
                                sf::Vertex(sf::Vector2f(cx, cy), sf::Color(64, 224, 208, 240))
                            };
                            window.draw(line, 2, sf::Lines);
                        }
                    }
                }

                // Render available empty child slots
                for (int s = 0; s < act1.lvl12.slotCount; s++) {
                    if (act1.lvl12.slots[s].isAvailable && !act1.lvl12.slots[s].isOccupied) {
                        float sx = act1.lvl12.slots[s].x;
                        float sy = act1.lvl12.slots[s].y;

                        sf::CircleShape slotCircle(24.0f);
                        slotCircle.setOrigin(24.0f, 24.0f);
                        slotCircle.setPosition(sx, sy);
                        slotCircle.setFillColor(sf::Color(20, 42, 58, 220));
                        slotCircle.setOutlineColor(sf::Color(64, 224, 208));
                        slotCircle.setOutlineThickness(2.0f);
                        window.draw(slotCircle);

                        sf::Text label;
                        label.setFont(renderer.font);
                        if (s == 0) {
                            label.setString("ROOT");
                            label.setCharacterSize(10);
                        } else if (act1.lvl12.slots[s].isLeft) {
                            label.setString("LEFT");
                            label.setCharacterSize(10);
                        } else {
                            label.setString("RIGHT");
                            label.setCharacterSize(9);
                        }
                        label.setStyle(sf::Text::Bold);
                        label.setFillColor(sf::Color(64, 255, 220));
                        sf::FloatRect lb = label.getLocalBounds();
                        label.setOrigin(lb.left + lb.width * 0.5f, lb.top + lb.height * 0.5f);
                        label.setPosition(sx, sy);
                        window.draw(label);
                    }
                }

                window.setView(defaultView);
                // Sequence bar at top
                sf::Text seqText;
                seqText.setFont(renderer.font);
                char seqBuf[160];
                if (act1.lvl12.currentStage == 0) {
                    snprintf(seqBuf, sizeof(seqBuf), "Preorder: [50, 25, 12, 37, 75, 62, 88]   |   Inorder: [12, 25, 37, 50, 62, 75, 88]");
                } else {
                    snprintf(seqBuf, sizeof(seqBuf), "Postorder: [10, 30, 20, 50, 70, 60, 40]  |   Inorder: [10, 20, 30, 40, 50, 60, 70]");
                }
                seqText.setString(seqBuf);
                seqText.setCharacterSize(14);
                seqText.setStyle(sf::Text::Bold);
                seqText.setFillColor(sf::Color(255, 215, 0));
                sf::FloatRect stb = seqText.getLocalBounds();
                seqText.setOrigin(stb.left + stb.width * 0.5f, stb.top + stb.height * 0.5f);
                seqText.setPosition(640.0f, 96.0f);
                window.draw(seqText);

                // Token tray at bottom
                for (int i = 0; i < act1.lvl12.tokenCount; i++) {
                    const ReconToken& tok = act1.lvl12.tokens[i];
                    sf::RectangleShape box(sf::Vector2f(50.0f, 36.0f));
                    box.setOrigin(25.0f, 18.0f);
                    box.setPosition(tok.trayX, tok.trayY);

                    if (tok.placed) {
                        box.setFillColor(sf::Color(15, 20, 30, 120));
                        box.setOutlineColor(sf::Color(60, 70, 80));
                        box.setOutlineThickness(1.0f);
                    } else if (act1.lvl12.selectedTokenIndex == i) {
                        box.setFillColor(sf::Color(40, 90, 120, 240));
                        box.setOutlineColor(sf::Color(255, 215, 0));
                        box.setOutlineThickness(2.5f);
                    } else {
                        box.setFillColor(sf::Color(20, 45, 65, 220));
                        box.setOutlineColor(sf::Color(64, 224, 208));
                        box.setOutlineThickness(1.5f);
                    }
                    window.draw(box);

                    char valStr[12];
                    snprintf(valStr, sizeof(valStr), "%d", tok.value);
                    sf::Text valText;
                    valText.setFont(renderer.font);
                    valText.setString(valStr);
                    valText.setCharacterSize(15);
                    valText.setStyle(sf::Text::Bold);
                    valText.setFillColor(tok.placed ? sf::Color(100, 110, 120) : sf::Color::White);
                    sf::FloatRect vb = valText.getLocalBounds();
                    valText.setOrigin(vb.left + vb.width * 0.5f, vb.top + vb.height * 0.5f);
                    valText.setPosition(tok.trayX, tok.trayY);
                    window.draw(valText);
                }
            } else if (lvlNum == 3) {
                for (int i = 0; i < layout.nodeCount; i++) {
                    if (!layout.nodes[i].active) continue;
                    int val = layout.nodes[i].key;
                    for (int k = 0; k < act1.lvl13.currentPostorderIdx; k++) {
                        if (act1.lvl13.postorderValues[k] == val && act1.lvl13.enteredHeights[k] >= 0) {
                            float hx = layout.nodes[i].x + layout.nodes[i].radius * 0.65f;
                            float hy = layout.nodes[i].y - layout.nodes[i].radius * 0.75f;
                            sf::CircleShape badge(11.0f);
                            badge.setOrigin(11.0f, 11.0f);
                            badge.setPosition(hx, hy);
                            badge.setFillColor(sf::Color(20, 70, 80, 240));
                            badge.setOutlineColor(sf::Color(64, 224, 208));
                            badge.setOutlineThickness(1.5f);
                            window.draw(badge);

                            char hbuf[8];
                            snprintf(hbuf, sizeof(hbuf), "%d", act1.lvl13.enteredHeights[k]);
                            sf::Text hText;
                            hText.setFont(renderer.font);
                            hText.setString(hbuf);
                            hText.setCharacterSize(10);
                            hText.setStyle(sf::Text::Bold);
                            hText.setFillColor(sf::Color::White);
                            sf::FloatRect hb = hText.getLocalBounds();
                            hText.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
                            hText.setPosition(hx, hy);
                            window.draw(hText);
                            break;
                        }
                    }
                }
            }
        }

        // Act 2 Special Renderers
        if (isScreen("level_play") && isAct("act_2") && levelManager.getCurrentLevel()) {
            int lvlNum = levelManager.getCurrentLevel()->levelNumber;
            if (lvlNum == 1) {
                // Level 2.1: Cover unrevealed nodes with '?'
                for (int i = 0; i < layout.nodeCount; i++) {
                    if (!layout.nodes[i].active) continue;
                    int k = layout.nodes[i].key;
                    bool revealed = false;
                    for (int p = 0; p < act2.lvl21.pathCount; p++) {
                        if (act2.lvl21.path[p] == k) { revealed = true; break; }
                    }
                    if (!revealed) {
                        sf::CircleShape mask(layout.nodes[i].radius * 0.95f);
                        mask.setOrigin(layout.nodes[i].radius * 0.95f, layout.nodes[i].radius * 0.95f);
                        mask.setPosition(layout.nodes[i].x, layout.nodes[i].y);
                        mask.setFillColor(sf::Color(18, 12, 28, 245));
                        mask.setOutlineColor(sf::Color(70, 45, 95));
                        mask.setOutlineThickness(1.5f);
                        window.draw(mask);

                        sf::Text qText;
                        qText.setFont(renderer.font);
                        qText.setString("?");
                        qText.setCharacterSize(20);
                        qText.setStyle(sf::Text::Bold);
                        qText.setFillColor(sf::Color(160, 140, 190));
                        sf::FloatRect qb = qText.getLocalBounds();
                        qText.setOrigin(qb.left + qb.width * 0.5f, qb.top + qb.height * 0.5f);
                        qText.setPosition(layout.nodes[i].x, layout.nodes[i].y);
                        window.draw(qText);
                    }
                }
            } else if (lvlNum == 2) {
                // Level 2.2: Render empty slots (NULL placeholders)
                computeAct2EmptySlots();
                for (int s = 0; s < act2.lvl22.slotCount; s++) {
                    const BSTEmptySlot& slot = act2.lvl22.slots[s];
                    int pIdx = findVisualNodeByKey(slot.parentKey);
                    if (pIdx != -1) {
                        float px = layout.nodes[pIdx].x;
                        float py = layout.nodes[pIdx].y;
                        sf::Vertex line[] = {
                            sf::Vertex(sf::Vector2f(px, py), sf::Color(64, 224, 208, 120)),
                            sf::Vertex(sf::Vector2f(slot.x, slot.y), sf::Color(64, 224, 208, 220))
                        };
                        window.draw(line, 2, sf::Lines);
                    }

                    sf::CircleShape slotCircle(22.0f);
                    slotCircle.setOrigin(22.0f, 22.0f);
                    slotCircle.setPosition(slot.x, slot.y);
                    slotCircle.setFillColor(sf::Color(15, 35, 48, 230));
                    slotCircle.setOutlineColor(sf::Color(64, 224, 208));
                    slotCircle.setOutlineThickness(1.8f);
                    window.draw(slotCircle);

                    sf::Text label;
                    label.setFont(renderer.font);
                    label.setString(slot.isLeft ? "L:NULL" : "R:NULL");
                    label.setCharacterSize(9);
                    label.setStyle(sf::Text::Bold);
                    label.setFillColor(sf::Color(64, 255, 220));
                    sf::FloatRect lb = label.getLocalBounds();
                    label.setOrigin(lb.left + lb.width * 0.5f, lb.top + lb.height * 0.5f);
                    label.setPosition(slot.x, slot.y);
                    window.draw(label);
                }
            } else if (lvlNum == 4) {
                // Level 2.4: Grey out skipped subtrees with distinct overlay & SKIPPED badges
                if (act2.lvl24.isExecuted) {
                    for (int p = 0; p < act2.lvl24.prunedNodeCount; p++) {
                        int pk = act2.lvl24.prunedNodeKeys[p];
                        int vIdx = findVisualNodeByKey(pk);
                        if (vIdx != -1) {
                            float vx = layout.nodes[vIdx].x;
                            float vy = layout.nodes[vIdx].y;
                            float vr = layout.nodes[vIdx].radius;

                            sf::CircleShape shroud(vr * 1.15f);
                            shroud.setOrigin(vr * 1.15f, vr * 1.15f);
                            shroud.setPosition(vx, vy);
                            shroud.setFillColor(sf::Color(18, 10, 25, 215));
                            shroud.setOutlineColor(sf::Color(110, 50, 110, 220));
                            shroud.setOutlineThickness(2.0f);
                            window.draw(shroud);

                            sf::Text pTag;
                            pTag.setFont(renderer.font);
                            pTag.setString("SKIPPED");
                            pTag.setCharacterSize(8);
                            pTag.setStyle(sf::Text::Bold);
                            pTag.setFillColor(sf::Color(220, 130, 160));
                            sf::FloatRect ptb = pTag.getLocalBounds();
                            pTag.setOrigin(ptb.left + ptb.width * 0.5f, ptb.top + ptb.height * 0.5f);
                            pTag.setPosition(vx, vy + vr * 0.72f);
                            window.draw(pTag);
                        }
                    }
                }
            } else if (lvlNum == 5) {
                // Level 2.5: Stage A sorted array bar
                if (act2.lvl25.stage == 0) {
                    window.setView(defaultView);
                    float startX = 640.0f - (7 * 65.0f) * 0.5f + 30.0f;
                    float trayY = 630.0f;
                    for (int i = 0; i < act2.lvl25.totalCount; i++) {
                        float bx = startX + i * 65.0f;
                        bool isInserted = (i < act2.lvl25.insertedCount);
                        bool isCurrent = (i == act2.lvl25.insertedCount);

                        sf::RectangleShape box(sf::Vector2f(54.0f, 36.0f));
                        box.setOrigin(27.0f, 18.0f);
                        box.setPosition(bx, trayY);

                        if (isInserted) {
                            box.setFillColor(sf::Color(15, 20, 30, 140));
                            box.setOutlineColor(sf::Color(60, 70, 80));
                            box.setOutlineThickness(1.0f);
                        } else if (isCurrent) {
                            box.setFillColor(sf::Color(40, 90, 120, 240));
                            box.setOutlineColor(sf::Color(255, 215, 0));
                            box.setOutlineThickness(2.5f);
                        } else {
                            box.setFillColor(sf::Color(20, 45, 65, 220));
                            box.setOutlineColor(sf::Color(64, 224, 208));
                            box.setOutlineThickness(1.5f);
                        }
                        window.draw(box);

                        char valStr[12];
                        snprintf(valStr, sizeof(valStr), "%d", act2.lvl25.sortedValues[i]);
                        sf::Text valText;
                        valText.setFont(renderer.font);
                        valText.setString(valStr);
                        valText.setCharacterSize(15);
                        valText.setStyle(sf::Text::Bold);
                        valText.setFillColor(isInserted ? sf::Color(100, 110, 120) : sf::Color::White);
                        sf::FloatRect vb = valText.getLocalBounds();
                        valText.setOrigin(vb.left + vb.width * 0.5f, vb.top + vb.height * 0.5f);
                        valText.setPosition(bx, trayY);
                        window.draw(valText);
                    }
                    window.setView(cameraView);
                }
            }
        }

        window.setView(defaultView);

        if (isScreen("level_play") && isAct("act_2")) {
            int lvlNum = levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->levelNumber : 1;
            if (lvlNum == 4 && act2.lvl24.isExecuted) {
                // Section 6: Range Raid Result Panel
                float px = 970.0f;
                float py = 150.0f;
                float pw = 280.0f;
                float ph = 265.0f;

                sf::RectangleShape panelBg(sf::Vector2f(pw, ph));
                panelBg.setPosition(px, py);
                panelBg.setFillColor(sf::Color(15, 18, 28, 240));
                panelBg.setOutlineColor(sf::Color(64, 224, 208, 220));
                panelBg.setOutlineThickness(2.0f);
                window.draw(panelBg);

                sf::Text pTitle;
                pTitle.setFont(renderer.font);
                pTitle.setString("RANGE RAID RESULTS");
                pTitle.setCharacterSize(16);
                pTitle.setStyle(sf::Text::Bold);
                pTitle.setFillColor(sf::Color(255, 215, 0));
                pTitle.setPosition(px + 14.0f, py + 12.0f);
                window.draw(pTitle);

                char rBuf[256];
                snprintf(rBuf, sizeof(rBuf),
                    "Requested Range: [30, 65]\n\n"
                    "Matching Treasures:\n"
                    "  30, 40, 50, 60, 65\n\n"
                    "Total Treasures Found: %d\n"
                    "Pruned: 2 Subtrees (20, 80)\n"
                    "Status: Raid Completed\n"
                    "+150 Pruning Bonus!",
                    act2.lvl24.matchingCount);
                sf::Text pBody;
                pBody.setFont(renderer.font);
                pBody.setString(rBuf);
                pBody.setCharacterSize(13);
                pBody.setFillColor(sf::Color(220, 230, 240));
                pBody.setPosition(px + 14.0f, py + 42.0f);
                window.draw(pBody);
            }
        }

        if (isAct("act_4")) {
            renderer.renderHeapArray(window, heap.getArray(), heap.getSize(), selectedNodeIndex, layout);
        }

        hud.render(window, renderer.font);
        if (inputBox.x > 0.0f) {
            inputBox.render(window, renderer.font);
        }

        for (int i = 0; i < buttonCount; i++) {
            buttons[i].render(window, renderer.font);
        }

        // Victory / Defeat Overlays in Level Play
        if (isScreen("level_play") && strcmp(levelManager.playState, "victory") == 0) {
            sf::RectangleShape modalBg(sf::Vector2f(600.0f, 320.0f));
            modalBg.setPosition(340.0f, 200.0f);
            modalBg.setFillColor(sf::Color(10, 30, 35, 245));
            modalBg.setOutlineColor(sf::Color(64, 224, 208));
            modalBg.setOutlineThickness(3.0f);
            window.draw(modalBg);

            sf::Text winTitle;
            winTitle.setFont(renderer.font);
            winTitle.setString("CHAMBER DECONTAMINATED!");
            winTitle.setCharacterSize(26);
            winTitle.setStyle(sf::Text::Bold);
            winTitle.setFillColor(sf::Color(64, 255, 180));
            sf::FloatRect wtb = winTitle.getLocalBounds();
            winTitle.setOrigin(wtb.left + wtb.width * 0.5f, wtb.top + wtb.height * 0.5f);
            winTitle.setPosition(640.0f, 250.0f);
            window.draw(winTitle);

            char starStr[32] = "";
            int stars = levelManager.scoreSystem.computeStars();
            for (int s = 0; s < stars; s++) strcat(starStr, "* ");
            sf::Text starText;
            starText.setFont(renderer.font);
            starText.setString(starStr);
            starText.setCharacterSize(36);
            starText.setStyle(sf::Text::Bold);
            starText.setFillColor(sf::Color(255, 215, 0));
            sf::FloatRect stb = starText.getLocalBounds();
            starText.setOrigin(stb.left + stb.width * 0.5f, stb.top + stb.height * 0.5f);
            starText.setPosition(640.0f, 305.0f);
            window.draw(starText);

            char scoreSummary[128];
            int bonusVal = (levelManager.scoreSystem.alarmSteps < levelManager.scoreSystem.optimalSteps) ? 50 : 0;
            if (bonusVal > 0) {
                snprintf(scoreSummary, sizeof(scoreSummary), "Score: %d (+50 Sub-optimal Bonus!) | Steps: %d / %d optimal",
                         levelManager.scoreSystem.computeFinalScore(),
                         levelManager.scoreSystem.alarmSteps,
                         levelManager.scoreSystem.optimalSteps);
            } else {
                snprintf(scoreSummary, sizeof(scoreSummary), "Score: %d | Steps: %d / %d optimal",
                         levelManager.scoreSystem.computeFinalScore(),
                         levelManager.scoreSystem.alarmSteps,
                         levelManager.scoreSystem.optimalSteps);
            }
            sf::Text scoreMsg;
            scoreMsg.setFont(renderer.font);
            scoreMsg.setString(scoreSummary);
            scoreMsg.setCharacterSize(14);
            scoreMsg.setFillColor(sf::Color(220, 240, 255));
            sf::FloatRect smb = scoreMsg.getLocalBounds();
            scoreMsg.setOrigin(smb.left + smb.width * 0.5f, smb.top + smb.height * 0.5f);
            scoreMsg.setPosition(640.0f, 360.0f);
            window.draw(scoreMsg);
        } else if (isScreen("level_play") && strcmp(levelManager.playState, "failed") == 0) {
            sf::RectangleShape modalBg(sf::Vector2f(620.0f, 320.0f));
            modalBg.setPosition(330.0f, 195.0f);
            modalBg.setFillColor(sf::Color(35, 10, 15, 248));
            modalBg.setOutlineColor(sf::Color(255, 45, 45));
            modalBg.setOutlineThickness(3.0f);
            window.draw(modalBg);

            sf::Text failTitle;
            failTitle.setFont(renderer.font);
            failTitle.setString("GAME OVER");
            failTitle.setCharacterSize(44);
            failTitle.setStyle(sf::Text::Bold);
            failTitle.setFillColor(sf::Color(255, 45, 45));
            sf::FloatRect ftb = failTitle.getLocalBounds();
            failTitle.setOrigin(ftb.left + ftb.width * 0.5f, ftb.top + ftb.height * 0.5f);
            failTitle.setPosition(640.0f, 255.0f);
            window.draw(failTitle);

            sf::Text subTitle;
            subTitle.setFont(renderer.font);
            subTitle.setString("ALARM CAPACITY BREACHED - VAULT LOCKDOWN");
            subTitle.setCharacterSize(16);
            subTitle.setStyle(sf::Text::Bold);
            subTitle.setFillColor(sf::Color(255, 140, 140));
            sf::FloatRect stb = subTitle.getLocalBounds();
            subTitle.setOrigin(stb.left + stb.width * 0.5f, stb.top + stb.height * 0.5f);
            subTitle.setPosition(640.0f, 315.0f);
            window.draw(subTitle);

            sf::Text failMsg;
            failMsg.setFont(renderer.font);
            failMsg.setString("Alarm threshold was exhausted before satisfying chamber objectives.");
            failMsg.setCharacterSize(14);
            failMsg.setFillColor(sf::Color(220, 200, 200));
            sf::FloatRect fmb = failMsg.getLocalBounds();
            failMsg.setOrigin(fmb.left + fmb.width * 0.5f, fmb.top + fmb.height * 0.5f);
            failMsg.setPosition(640.0f, 355.0f);
            window.draw(failMsg);

            char stepSummary[96];
            snprintf(stepSummary, sizeof(stepSummary), "Alarm Steps: %d / %d (Capacity Reached)",
                     levelManager.scoreSystem.alarmSteps, levelManager.scoreSystem.alarmCapacity);
            sf::Text stepMsg;
            stepMsg.setFont(renderer.font);
            stepMsg.setString(stepSummary);
            stepMsg.setCharacterSize(13);
            stepMsg.setFillColor(sf::Color(255, 180, 180));
            sf::FloatRect ssmb = stepMsg.getLocalBounds();
            stepMsg.setOrigin(ssmb.left + ssmb.width * 0.5f, ssmb.top + ssmb.height * 0.5f);
            stepMsg.setPosition(640.0f, 388.0f);
            window.draw(stepMsg);
        }
    }
};

#endif
