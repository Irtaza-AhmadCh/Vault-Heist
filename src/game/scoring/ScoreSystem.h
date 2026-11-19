#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

class ScoreSystem {
public:
    int alarmSteps;
    int alarmCapacity;
    int optimalSteps;
    int penalties;
    int currentScore;
    int hintsUsed;
    int penaltyCount;
    int bonuses;

    ScoreSystem();

    void reset(int optSteps, int capacity);
    void addAlarmStep(int count = 1);
    void addPenalty(int penaltyScore, int penaltyAlarm = 1);
    void useHint();
    
    int computeFinalScore() const;
    int computeStars() const;
    bool isAlarmBreached() const;
    float getAlarmRatio() const;
};

#endif
