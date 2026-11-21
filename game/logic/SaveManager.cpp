#include "SaveManager.h"
#include <cstdio>
#include <cstring>

bool SaveManager::saveProgress(const char* filepath, const LevelDef levels[], int levelCount) {
    if (filepath == nullptr || levels == nullptr || levelCount <= 0) return false;

    FILE* file = fopen(filepath, "w");
    if (file == nullptr) return false;

    int savedCount = 0;
    for (int i = 0; i < levelCount; i++) if (levels[i].id[0] != '\0') savedCount++;
    fprintf(file, "VAULT_HEIST_SAVE_V2\n");
    fprintf(file, "%d\n", savedCount);

    for (int i = 0; i < levelCount; i++) {
        if (levels[i].id[0] == '\0') continue;
        fprintf(file, "%s %d %d %d %d\n",
                levels[i].id,
                levels[i].isUnlocked ? 1 : 0,
                levels[i].isCompleted ? 1 : 0,
                levels[i].starsEarned,
                levels[i].bestScore);
    }

    fclose(file);
    return true;
}

bool SaveManager::loadProgress(const char* filepath, LevelDef levels[], int levelCount) {
    if (filepath == nullptr || levels == nullptr || levelCount <= 0) return false;

    FILE* file = fopen(filepath, "r");
    if (file == nullptr) return false;

    char header[32] = {0};
    if (fscanf(file, "%31s", header) != 1) {
        fclose(file);
        return false;
    }

    if (strcmp(header, "VAULT_HEIST_SAVE_V2") == 0) {
        int savedCount = 0;
        if (fscanf(file, "%d", &savedCount) != 1) {
            fclose(file);
            return false;
        }

        for (int i = 0; i < savedCount; i++) {
            char id[32] = {0};
            int unlocked = 0, completed = 0, stars = 0, score = 0;
            if (fscanf(file, "%31s %d %d %d %d", id, &unlocked, &completed, &stars, &score) != 5) break;
            for (int j = 0; j < levelCount; j++) {
                if (strcmp(levels[j].id, id) == 0) {
                    levels[j].isUnlocked = unlocked != 0;
                    levels[j].isCompleted = completed != 0;
                    levels[j].starsEarned = stars;
                    levels[j].bestScore = score;
                    break;
                }
            }
        }
        fclose(file);
        return true;
    } else if (strcmp(header, "VAULT_HEIST_SAVE_V1") == 0) {
        int savedCount = 0;
        if (fscanf(file, "%d", &savedCount) != 1) {
            fclose(file);
            return false;
        }

        for (int i = 0; i < savedCount; i++) {
            int act = 0, lvl = 0, unlocked = 0, completed = 0, stars = 0, score = 0;
            if (fscanf(file, "%d %d %d %d %d %d", &act, &lvl, &unlocked, &completed, &stars, &score) != 6) break;
            for (int j = 0; j < levelCount; j++) {
                if (levels[j].actNumber == act && levels[j].levelNumber == lvl) {
                    levels[j].isUnlocked = unlocked != 0;
                    levels[j].isCompleted = completed != 0;
                    levels[j].starsEarned = stars;
                    levels[j].bestScore = score;
                    break;
                }
            }
        }
        fclose(file);
        return true;
    }

    fclose(file);
    return false;
}

static void serializeTreeHelper(TreeNode* node, char* buffer, int maxLen, int& offset) {
    if (offset >= maxLen - 8) return;
    if (node == nullptr) {
        int written = snprintf(buffer + offset, maxLen - offset, "# ");
        if (written > 0) offset += written;
        return;
    }
    int written = snprintf(buffer + offset, maxLen - offset, "%d ", node->data);
    if (written > 0) offset += written;
    serializeTreeHelper(node->left, buffer, maxLen, offset);
    serializeTreeHelper(node->right, buffer, maxLen, offset);
}

bool SaveManager::serializeTree(TreeNode* node, char* buffer, int maxLen) {
    if (buffer == nullptr || maxLen <= 0) return false;
    buffer[0] = '\0';
    int offset = 0;
    serializeTreeHelper(node, buffer, maxLen, offset);
    return true;
}

static TreeNode* deserializeTreeHelper(const char*& ptr) {
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r') ptr++;
    if (*ptr == '\0') return nullptr;

    if (*ptr == '#') {
        ptr++;
        return nullptr;
    }

    int val = 0;
    int sign = 1;
    if (*ptr == '-') { sign = -1; ptr++; }
    while (*ptr >= '0' && *ptr <= '9') {
        val = val * 10 + (*ptr - '0');
        ptr++;
    }
    val *= sign;

    TreeNode* node = new TreeNode(val);
    node->left = deserializeTreeHelper(ptr);
    node->right = deserializeTreeHelper(ptr);
    return node;
}

TreeNode* SaveManager::deserializeTree(const char* buffer) {
    if (buffer == nullptr) return nullptr;
    const char* ptr = buffer;
    return deserializeTreeHelper(ptr);
}

bool SaveManager::serializeHeap(const int* arr, int n, char* buffer, int maxLen) {
    if (arr == nullptr || buffer == nullptr || maxLen <= 0) return false;
    int offset = snprintf(buffer, maxLen, "%d", n);
    for (int i = 0; i < n && offset < maxLen - 12; i++) {
        int written = snprintf(buffer + offset, maxLen - offset, " %d", arr[i]);
        if (written > 0) offset += written;
    }
    return true;
}

bool SaveManager::deserializeHeap(const char* buffer, int* arr, int& n, int maxN) {
    if (buffer == nullptr || arr == nullptr) return false;
    n = 0;
    int total = 0;
    int readChars = 0;
    if (sscanf(buffer, "%d%n", &total, &readChars) != 1) return false;

    const char* ptr = buffer + readChars;
    while (*ptr != '\0' && n < total && n < maxN) {
        int val = 0;
        int step = 0;
        if (sscanf(ptr, "%d%n", &val, &step) == 1) {
            arr[n++] = val;
            ptr += step;
        } else {
            break;
        }
    }
    return true;
}

bool SaveManager::saveEndless(const char* filepath, const EndlessSaveData& data) {
    if (filepath == nullptr || data.keyCount < 0 || data.keyCount > 32) return false;
    FILE* f = fopen(filepath, "w");
    if (f == nullptr) return false;
    fprintf(f, "%d %d %d %d\n", data.wave, data.score, data.seed, data.keyCount);
    for (int i = 0; i < data.keyCount; i++) {
        fprintf(f, "%d ", data.keys[i]);
    }
    fprintf(f, "\n");
    fclose(f);
    return true;
}

bool SaveManager::loadEndless(const char* filepath, EndlessSaveData& data) {
    if (filepath == nullptr) return false;
    FILE* f = fopen(filepath, "r");
    if (f == nullptr) return false;
    if (fscanf(f, "%d %d %d %d", &data.wave, &data.score, &data.seed, &data.keyCount) != 4) {
        fclose(f);
        return false;
    }
    if (data.keyCount < 0 || data.keyCount > 32) {
        fclose(f);
        return false;
    }
    for (int i = 0; i < data.keyCount; i++) {
        if (fscanf(f, "%d", &data.keys[i]) != 1) { fclose(f); return false; }
    }
    fclose(f);
    return true;
}
