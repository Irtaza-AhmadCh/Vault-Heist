#include <iostream>
#include <cassert>
#include <cstring>

#include "../../game/scoring/ScoreSystem.h"
#include "../../game/controller/LevelDef.h"
#include "../../game/controller/LevelManager.h"
#include "../../game/logic/binary_tree/BinaryTree.h"
#include "../../game/controller/act1-2/Act1State.h"

using namespace std;

// Test 1: General Star Calculation Rules
void testStarThresholdRules() {
    cout << "Running testStarThresholdRules..." << endl;
    ScoreSystem ss;

    // Base: optimalSteps = 6, capacity = 11
    ss.reset(6, 11);

    // 1. Exact optimal, 0 penalties -> 3 Stars (no bonus)
    ss.addAlarmStep(6);
    assert(ss.alarmSteps == 6);
    assert(ss.penaltyCount == 0);
    assert(ss.computeStars() == 3);
    assert(ss.computeFinalScore() == 940); // 1000 - 60

    // 2. Below optimal, 0 penalties -> 3 Stars (+50 bonus)
    ss.reset(6, 11);
    ss.addAlarmStep(4);
    assert(ss.computeStars() == 3);
    assert(ss.computeFinalScore() == 1010); // 1000 - 40 + 50

    // 3. Exactly 1.25 * optimal (ceil(1.25 * 6) = 8 steps), 0 penalties -> 2 Stars
    ss.reset(6, 11);
    ss.addAlarmStep(7);
    assert(ss.computeStars() == 2);

    ss.reset(6, 11);
    ss.addAlarmStep(8);
    assert(ss.computeStars() == 2);

    // 4. Optimal steps + 1 penalty (penalty adds 1 alarm step, total = 7), 1 penalty -> 2 Stars
    ss.reset(6, 11);
    ss.addAlarmStep(6);
    ss.addPenalty(0, 1);
    assert(ss.alarmSteps == 7);
    assert(ss.penaltyCount == 1);
    assert(ss.computeStars() == 2);

    // 5. Exceeding 1.25 * optimal (9 steps <= 11 capacity), 0 penalties -> 1 Star
    ss.reset(6, 11);
    ss.addAlarmStep(9);
    assert(ss.computeStars() == 1);

    // 6. Within 1.25 * optimal but 2 penalties -> 1 Star
    ss.reset(6, 11);
    ss.addAlarmStep(5);
    ss.addPenalty(0, 1);
    ss.addPenalty(0, 1);
    assert(ss.alarmSteps == 7);
    assert(ss.penaltyCount == 2);
    assert(ss.computeStars() == 1);

    // 7. Full alarm capacity (11 steps) -> 0 Stars (Alarm Breached)
    ss.reset(6, 11);
    ss.addAlarmStep(11);
    assert(ss.isAlarmBreached());
    assert(ss.computeStars() == 0);

    // 8. Beyond alarm capacity (12 steps) -> 0 Stars
    ss.reset(6, 11);
    ss.addAlarmStep(12);
    assert(ss.isAlarmBreached());
    assert(ss.computeStars() == 0);

    cout << "testStarThresholdRules passed!" << endl;
}

// Test 2: Hint Penalty Effect on Stars
void testHintPenalties() {
    cout << "Running testHintPenalties..." << endl;
    ScoreSystem ss;

    // Case A: 3-star performance with 1 hint -> 2 Stars
    ss.reset(10, 18);
    ss.addAlarmStep(5);
    ss.useHint(); // adds 1 alarm step (total = 6 <= 10)
    assert(ss.alarmSteps == 6);
    assert(ss.hintsUsed == 1);
    assert(ss.computeStars() == 2);

    // Case B: 3-star performance with 2 hints -> 1 Star
    ss.reset(10, 18);
    ss.addAlarmStep(5);
    ss.useHint();
    ss.useHint();
    assert(ss.hintsUsed == 2);
    assert(ss.computeStars() == 1);

    // Case C: 2-star performance with 1 hint -> 1 Star
    ss.reset(10, 18);
    ss.addAlarmStep(11); // > 10, <= 13 (ceil(1.25 * 10))
    assert(ss.computeStars() == 2);
    ss.useHint(); // adds 1 alarm step (12 <= 13)
    assert(ss.computeStars() == 1);

    // Case D: 1-star performance with 1 hint -> minimum 1 Star upon completion
    ss.reset(10, 18);
    ss.addAlarmStep(14); // > 13
    assert(ss.computeStars() == 1);
    ss.useHint();
    assert(ss.computeStars() == 1);

    cout << "testHintPenalties passed!" << endl;
}

// Test 3: Act 1 All 5 Levels Star Scoring Progression
void testAct1LevelsStarScoring() {
    cout << "Running testAct1LevelsStarScoring..." << endl;
    LevelManager lm;

    // Verify Act 1 LevelDef optimalSteps and alarmCapacity
    assert(lm.levels[0].optimalSteps == 28);
    assert(lm.levels[0].alarmCapacity == 44);

    assert(lm.levels[1].optimalSteps == 14);
    assert(lm.levels[1].alarmCapacity == 23);

    assert(lm.levels[2].optimalSteps == 7);
    assert(lm.levels[2].alarmCapacity == 13);

    assert(lm.levels[3].optimalSteps == 5);
    assert(lm.levels[3].alarmCapacity == 10);

    assert(lm.levels[4].optimalSteps == 7);
    assert(lm.levels[4].alarmCapacity == 13);

    // Level 1.1 Tests
    // 3 Stars: 28 steps, 0 penalties
    ScoreSystem s11;
    s11.reset(lm.levels[0].optimalSteps, lm.levels[0].alarmCapacity);
    s11.addAlarmStep(28);
    assert(s11.computeStars() == 3);

    // 2 Stars: 29 steps (1 wrong click), 1 penalty
    s11.reset(lm.levels[0].optimalSteps, lm.levels[0].alarmCapacity);
    s11.addAlarmStep(28);
    s11.addPenalty(0, 1);
    assert(s11.computeStars() == 2);

    // 1 Star: 36 steps
    s11.reset(lm.levels[0].optimalSteps, lm.levels[0].alarmCapacity);
    s11.addAlarmStep(36);
    assert(s11.computeStars() == 1);

    // 0 Stars: 44 steps (capacity exhausted)
    s11.reset(lm.levels[0].optimalSteps, lm.levels[0].alarmCapacity);
    s11.addAlarmStep(44);
    assert(s11.isAlarmBreached());
    assert(s11.computeStars() == 0);

    // Level 1.2 Tests
    // 3 Stars: 14 steps, 0 penalties
    ScoreSystem s12;
    s12.reset(lm.levels[1].optimalSteps, lm.levels[1].alarmCapacity);
    s12.addAlarmStep(14);
    assert(s12.computeStars() == 3);

    // 2 Stars: 15 steps (1 wrong placement), 1 penalty
    s12.reset(lm.levels[1].optimalSteps, lm.levels[1].alarmCapacity);
    s12.addAlarmStep(14);
    s12.addPenalty(0, 1);
    assert(s12.computeStars() == 2);

    // 1 Star: 19 steps
    s12.reset(lm.levels[1].optimalSteps, lm.levels[1].alarmCapacity);
    s12.addAlarmStep(19);
    assert(s12.computeStars() == 1);

    // 0 Stars: 23 steps
    s12.reset(lm.levels[1].optimalSteps, lm.levels[1].alarmCapacity);
    s12.addAlarmStep(23);
    assert(s12.isAlarmBreached());
    assert(s12.computeStars() == 0);

    // Level 1.3 Tests
    // 3 Stars: 7 steps, 0 penalties
    ScoreSystem s13;
    s13.reset(lm.levels[2].optimalSteps, lm.levels[2].alarmCapacity);
    s13.addAlarmStep(7);
    assert(s13.computeStars() == 3);

    // 2 Stars: 8 steps (1 wrong height), 1 penalty
    s13.reset(lm.levels[2].optimalSteps, lm.levels[2].alarmCapacity);
    s13.addAlarmStep(7);
    s13.addPenalty(0, 1);
    assert(s13.computeStars() == 2);

    // 1 Star: 10 steps
    s13.reset(lm.levels[2].optimalSteps, lm.levels[2].alarmCapacity);
    s13.addAlarmStep(10);
    assert(s13.computeStars() == 1);

    // 0 Stars: 13 steps
    s13.reset(lm.levels[2].optimalSteps, lm.levels[2].alarmCapacity);
    s13.addAlarmStep(13);
    assert(s13.isAlarmBreached());
    assert(s13.computeStars() == 0);

    // Level 1.4 Tests
    // 3 Stars: 5 steps, 0 penalties
    ScoreSystem s14;
    s14.reset(lm.levels[3].optimalSteps, lm.levels[3].alarmCapacity);
    s14.addAlarmStep(5);
    assert(s14.computeStars() == 3);

    // 2 Stars: 6 steps (1 wrong LCA/path room), 1 penalty
    s14.reset(lm.levels[3].optimalSteps, lm.levels[3].alarmCapacity);
    s14.addAlarmStep(5);
    s14.addPenalty(0, 1);
    assert(s14.computeStars() == 2);

    // 1 Star: 8 steps
    s14.reset(lm.levels[3].optimalSteps, lm.levels[3].alarmCapacity);
    s14.addAlarmStep(8);
    assert(s14.computeStars() == 1);

    // 0 Stars: 10 steps
    s14.reset(lm.levels[3].optimalSteps, lm.levels[3].alarmCapacity);
    s14.addAlarmStep(10);
    assert(s14.isAlarmBreached());
    assert(s14.computeStars() == 0);

    // Level 1.5 Tests
    // 3 Stars: 7 steps, 0 penalties
    ScoreSystem s15;
    s15.reset(lm.levels[4].optimalSteps, lm.levels[4].alarmCapacity);
    s15.addAlarmStep(7);
    assert(s15.computeStars() == 3);

    // 2 Stars: 8 steps (1 invalid swap order), 1 penalty
    s15.reset(lm.levels[4].optimalSteps, lm.levels[4].alarmCapacity);
    s15.addAlarmStep(7);
    s15.addPenalty(0, 1);
    assert(s15.computeStars() == 2);

    // 1 Star: 10 steps
    s15.reset(lm.levels[4].optimalSteps, lm.levels[4].alarmCapacity);
    s15.addAlarmStep(10);
    assert(s15.computeStars() == 1);

    // 0 Stars: 13 steps
    s15.reset(lm.levels[4].optimalSteps, lm.levels[4].alarmCapacity);
    s15.addAlarmStep(13);
    assert(s15.isAlarmBreached());
    assert(s15.computeStars() == 0);

    cout << "testAct1LevelsStarScoring passed!" << endl;
}

// Test 4: Victory Progression Integration
void testLevelManagerVictoryStars() {
    cout << "Running testLevelManagerVictoryStars..." << endl;
    LevelManager lm;

    // Load Level 1.1
    assert(lm.loadLevel(0));
    assert(strcmp(lm.playState, "in_progress") == 0);

    // Add 28 steps, 0 penalties
    lm.scoreSystem.addAlarmStep(28);
    lm.notifyAction("inorder", 0);

    assert(strcmp(lm.playState, "victory") == 0);
    assert(lm.levels[0].isCompleted == true);
    assert(lm.levels[0].starsEarned == 3);
    assert(lm.levels[0].bestScore == 720); // 1000 - 28*10
    assert(lm.levels[1].isUnlocked == true); // Next level unlocked

    // Test Failure condition on Level 1.2
    assert(lm.loadLevel(1));
    assert(strcmp(lm.playState, "in_progress") == 0);
    lm.scoreSystem.addAlarmStep(23); // Reaches alarm capacity (23)
    assert(lm.scoreSystem.isAlarmBreached());
    lm.notifyAction("reconstruct", 0);
    // Should NOT trigger victory because alarm is breached
    assert(strcmp(lm.playState, "failed") == 0);
    assert(lm.levels[1].starsEarned == 0);

    cout << "testLevelManagerVictoryStars passed!" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  STAR RANKING SYSTEM UNIT TEST SUITE   " << endl;
    cout << "========================================" << endl;

    testStarThresholdRules();
    testHintPenalties();
    testAct1LevelsStarScoring();
    testLevelManagerVictoryStars();

    cout << "========================================" << endl;
    cout << "  ALL STAR RANKING TESTS PASSED (4/4)!  " << endl;
    cout << "========================================" << endl;
    return 0;
}
