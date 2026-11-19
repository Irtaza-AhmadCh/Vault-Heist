#include "ScoreSystem.h"

ScoreSystem::ScoreSystem() {
    alarmSteps = 0;
    alarmCapacity = 10;
    optimalSteps = 6;
    penalties = 0;
    currentScore = 1000;
    hintsUsed = 0;
    penaltyCount = 0;
    bonuses = 0;
}

void ScoreSystem::reset(int optSteps, int capacity) {
    alarmSteps = 0;
    optimalSteps = (optSteps > 0) ? optSteps : 6;
    alarmCapacity = (capacity > 0) ? capacity : (static_cast<int>(1.5f * optimalSteps + 0.999f) + 2);
    penalties = 0;
    currentScore = 1000;
    hintsUsed = 0;
    penaltyCount = 0;
    bonuses = 0;
}

void ScoreSystem::addAlarmStep(int count) {
    alarmSteps += count;
    currentScore -= count * 10;
    if (currentScore < 0) currentScore = 0;
}

void ScoreSystem::addPenalty(int penaltyScore, int penaltyAlarm) {
    penalties += penaltyScore;
    alarmSteps += penaltyAlarm;
    currentScore -= penaltyScore;
    if (currentScore < 0) currentScore = 0;
    penaltyCount++;
}

void ScoreSystem::useHint() {
    hintsUsed++;
    addAlarmStep(1);
}

int ScoreSystem::computeFinalScore() const {
    int bonusPoints = (alarmSteps < optimalSteps) ? 50 : 0;
    int finalScore = 1000 - (alarmSteps * 10) - penalties + bonusPoints + bonuses;
    if (finalScore < 0) finalScore = 0;
    return finalScore;
}

int ScoreSystem::computeStars() const {
    if (isAlarmBreached()) {
        return 0;
    }

    int stars = 1;
    // Condition 1: Steps <= optimal, no penalties -> 3 stars
    if (alarmSteps <= optimalSteps && penaltyCount == 0 && penalties == 0) {
        stars = 3;
    } 
    // Condition 2: Steps <= 1.25 * optimal, at most 1 penalty -> 2 stars
    else {
        int max2StarSteps = static_cast<int>(1.25f * optimalSteps + 0.999f); // ceil(1.25 * optimalSteps)
        if (alarmSteps <= max2StarSteps && penaltyCount <= 1 && penalties <= 50) {
            stars = 2;
        }
    }

    // Hint penalty: reduces maximum stars by 1 per hint
    if (hintsUsed > 0) {
        stars -= hintsUsed;
        if (stars < 1) stars = 1;
    }

    return stars;
}

bool ScoreSystem::isAlarmBreached() const {
    return alarmSteps >= alarmCapacity;
}

float ScoreSystem::getAlarmRatio() const {
    if (alarmCapacity <= 0) return 0.0f;
    float r = static_cast<float>(alarmSteps) / alarmCapacity;
    if (r > 1.0f) r = 1.0f;
    return r;
}
