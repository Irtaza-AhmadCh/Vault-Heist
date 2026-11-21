#include "LevelManager.h"
#include "CampaignJson.h"
#include <cstring>
#include <cstdio>

static void setLevelError(char* error, int errorSize, const char* message) {
    if (error != nullptr && errorSize > 0) {
        strncpy(error, message, errorSize - 1);
        error[errorSize - 1] = '\0';
    }
}

LevelManager::LevelManager() {
    currentLevelIndex = -1;
    levelCount = 0;
    strncpy(playState, "in_progress", sizeof(playState) - 1);
    playState[sizeof(playState) - 1] = '\0';
}

bool LevelManager::loadLevelsFromJson(const char* filepath) {
    char error[192];
    return loadCampaign(filepath, error, sizeof(error));
}

bool LevelManager::loadCampaign(const char* filepath, char* error, int errorSize) {
    CampaignJson json;
    if (!json.loadFile(filepath, error, errorSize)) return false;
    char objectText[MAX_CAMPAIGN_LEVELS][CampaignJson::MAX_LEVEL_TEXT];
    int count = 0;
    if (!json.splitLevels(objectText, MAX_CAMPAIGN_LEVELS, count, error, errorSize)) return false;

    LevelDef loaded[MAX_CAMPAIGN_LEVELS];
    for (int i = 0; i < count; i++) {
        LevelDef level;
        if (!json.readString(objectText[i], "id", level.id, sizeof(level.id)) ||
            !json.readInt(objectText[i], "act", level.actNumber) ||
            !json.readInt(objectText[i], "level", level.levelNumber) ||
            !json.readInt(objectText[i], "order", level.order) ||
            !json.readString(objectText[i], "title", level.title, sizeof(level.title)) ||
            !json.readString(objectText[i], "actName", level.actName, sizeof(level.actName)) ||
            !json.readString(objectText[i], "structureType", level.structureType, sizeof(level.structureType)) ||
            !json.readString(objectText[i], "gameplayType", level.gameplayType, sizeof(level.gameplayType)) ||
            !json.readString(objectText[i], "objective", level.objective, sizeof(level.objective)) ||
            !json.readString(objectText[i], "hint", level.hint, sizeof(level.hint)) ||
            !json.readString(objectText[i], "requiredAction", level.requiredAction, sizeof(level.requiredAction))) {
            snprintf(error, errorSize, "Level object %d is missing a required field.", i + 1);
            return false;
        }
        json.readInt(objectText[i], "targetValue", level.targetValue);
        json.readInt(objectText[i], "secondaryTargetValue", level.secondaryTargetValue);
        json.readIntArray(objectText[i], "initialValues", level.initialValues, LevelDef::MAX_MAP_VALUES, level.initialValueCount);
        json.readIntArray(objectText[i], "requiredValues", level.requiredValues, LevelDef::MAX_MAP_VALUES, level.requiredValueCount);
        json.readIntArray(objectText[i], "stageTargets", level.stageTargets, LevelDef::MAX_STAGES, level.stageTargetCount);
        calculateOptimalSteps(level);
        level.alarmCapacity = (3 * level.optimalSteps + 1) / 2 + 2;
        level.isUnlocked = level.order == 1;
        if (!validateLevel(level, error, errorSize)) return false;
        for (int j = 0; j < i; j++) {
            if (strcmp(loaded[j].id, level.id) == 0 || loaded[j].order == level.order) {
                setLevelError(error, errorSize, "Campaign has duplicate level id or order."); return false;
            }
        }
        loaded[i] = level;
    }
    for (int order = 1; order <= count; order++) {
        bool found = false;
        for (int i = 0; i < count; i++) if (loaded[i].order == order) found = true;
        if (!found) { setLevelError(error, errorSize, "Campaign order must start at 1 and be contiguous."); return false; }
    }
    for (int i = 0; i < count; i++) for (int j = i + 1; j < count; j++) if (loaded[j].order < loaded[i].order) { LevelDef swap = loaded[i]; loaded[i] = loaded[j]; loaded[j] = swap; }
    for (int i = 0; i < count; i++) levels[i] = loaded[i];
    levelCount = count;
    currentLevelIndex = -1;
    return true;
}

bool LevelManager::validateLevel(const LevelDef& level, char* error, int errorSize) const {
    bool structureOk = strcmp(level.structureType, "tree") == 0 || strcmp(level.structureType, "bst") == 0 || strcmp(level.structureType, "avl") == 0 || strcmp(level.structureType, "heap") == 0;
    if (level.id[0] == '\0' || level.actNumber < 1 || level.actNumber > 4 || level.levelNumber < 1 || level.order < 1 || !structureOk || level.gameplayType[0] == '\0' || level.optimalSteps <= 0) {
        setLevelError(error, errorSize, "Campaign level has invalid identity, type, or calculated steps."); return false;
    }
    return true;
}

int LevelManager::calculateBstSearchSteps(const LevelDef& level, bool includeInsert) const {
    if (level.requiredValueCount > 0) return level.requiredValueCount + (includeInsert ? 1 : 0);
    if (level.initialValueCount > 0) return includeInsert ? 2 : 1;
    return 1;
}

int LevelManager::calculateHeapBuildSteps(const LevelDef& level) const {
    int values[LevelDef::MAX_MAP_VALUES];
    int count = level.initialValueCount;
    if (count <= 0) return 1;
    for (int i = 0; i < count; i++) values[i] = level.initialValues[i];
    int steps = 0;
    for (int parent = count / 2 - 1; parent >= 0; parent--) {
        int index = parent;
        while (true) {
            int left = index * 2 + 1;
            int right = left + 1;
            if (left >= count) break;
            steps++;
            int largest = index;
            if (left < count && values[left] > values[largest]) largest = left;
            if (right < count && values[right] > values[largest]) largest = right;
            if (largest == index) break;
            int temp = values[index]; values[index] = values[largest]; values[largest] = temp;
            index = largest;
        }
    }
    return steps;
}

void LevelManager::calculateOptimalSteps(LevelDef& level) const {
    int steps = 0;
    if (strcmp(level.gameplayType, "guided_search") == 0) steps = calculateBstSearchSteps(level, false);
    else if (strcmp(level.gameplayType, "guided_insert") == 0) steps = calculateBstSearchSteps(level, true);
    else if (strcmp(level.gameplayType, "build_heap") == 0) steps = calculateHeapBuildSteps(level);
    else if (strcmp(level.gameplayType, "staged_delete") == 0) steps = level.stageTargetCount + 1;
    else if (strcmp(level.gameplayType, "degeneration_boss") == 0) for (int i = 0; i < level.requiredValueCount; i++) steps += i + 1;
    else if (strcmp(level.gameplayType, "order_statistic_boss") == 0) steps = 1 + level.requiredValueCount;
    else if (level.requiredValueCount > 0) steps = level.requiredValueCount;
    else if (level.stageTargetCount > 0) steps = level.stageTargetCount;
    else steps = 1;
    level.optimalSteps = steps;
}

bool LevelManager::loadLevel(int index) {
    if (index < 0 || index >= levelCount) { currentLevelIndex = -1; return false; }
    currentLevelIndex = index;
    strncpy(playState, "in_progress", sizeof(playState) - 1);
    playState[sizeof(playState) - 1] = '\0';
    scoreSystem.reset(levels[index].optimalSteps, levels[index].alarmCapacity);
    return true;
}

int LevelManager::findLevelIndexById(const char* id) const { if (id == nullptr) return -1; for (int i = 0; i < levelCount; i++) if (strcmp(levels[i].id, id) == 0) return i; return -1; }

bool LevelManager::checkVictory(const char* actionName, int value) {
    if (currentLevelIndex < 0 || currentLevelIndex >= levelCount) return false;
    const LevelDef& level = levels[currentLevelIndex];
    if (strcmp(level.requiredAction, actionName) == 0 && (level.targetValue == 0 || level.targetValue == value)) return true;
    if (strcmp(level.requiredAction, "boss") == 0 && (strcmp(actionName, "boss") == 0 || strcmp(actionName, "heap_sort") == 0)) return true;
    if (strcmp(level.requiredAction, "repair") == 0 && (strcmp(actionName, "repair") == 0 || strcmp(actionName, "sift_down") == 0 || strcmp(actionName, "sift_up") == 0)) return true;
    if (strcmp(level.requiredAction, "race") == 0 && (strcmp(actionName, "race") == 0 || strcmp(actionName, "extract_max") == 0)) return true;
    return strcmp(level.requiredAction, "rotate") == 0 && strcmp(actionName, "rotate") == 0;
}

void LevelManager::notifyAction(const char* actionName, int value) {
    if (currentLevelIndex < 0 || currentLevelIndex >= levelCount || strcmp(playState, "in_progress") != 0) return;
    if (scoreSystem.isAlarmBreached()) { strncpy(playState, "failed", sizeof(playState) - 1); return; }
    if (checkVictory(actionName, value)) {
        strncpy(playState, "victory", sizeof(playState) - 1);
        LevelDef& level = levels[currentLevelIndex]; level.isCompleted = true;
        int stars = scoreSystem.computeStars(); int score = scoreSystem.computeFinalScore();
        if (stars > level.starsEarned) level.starsEarned = stars;
        if (score > level.bestScore) level.bestScore = score;
        unlockNextLevel();
    }
}

LevelDef* LevelManager::getCurrentLevel() { return (currentLevelIndex >= 0 && currentLevelIndex < levelCount) ? &levels[currentLevelIndex] : nullptr; }
const LevelDef* LevelManager::getCurrentLevel() const { return (currentLevelIndex >= 0 && currentLevelIndex < levelCount) ? &levels[currentLevelIndex] : nullptr; }
int LevelManager::getCurrentAct() const { const LevelDef* level = getCurrentLevel(); return level ? level->actNumber : 1; }
bool LevelManager::unlockNextLevel() { int next = currentLevelIndex + 1; if (next >= 0 && next < levelCount) { levels[next].isUnlocked = true; return true; } return false; }
void LevelManager::resetCurrentLevel() { if (currentLevelIndex >= 0 && currentLevelIndex < levelCount) loadLevel(currentLevelIndex); }
