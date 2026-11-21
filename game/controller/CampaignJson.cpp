#include "CampaignJson.h"
#include <cstdio>
#include <cstring>
#include <cctype>

static void setJsonError(char* error, int errorSize, const char* message) {
    if (error != nullptr && errorSize > 0) {
        strncpy(error, message, errorSize - 1);
        error[errorSize - 1] = '\0';
    }
}

static const char* findJsonField(const char* objectText, const char* field) {
    char token[80];
    snprintf(token, sizeof(token), "\"%s\"", field);
    const char* found = strstr(objectText, token);
    if (found == nullptr) return nullptr;
    found += strlen(token);
    while (*found != '\0' && isspace(static_cast<unsigned char>(*found))) found++;
    if (*found != ':') return nullptr;
    found++;
    while (*found != '\0' && isspace(static_cast<unsigned char>(*found))) found++;
    return found;
}

bool CampaignJson::loadFile(const char* filePath, char* error, int errorSize) {
    if (filePath == nullptr) { setJsonError(error, errorSize, "Campaign path is missing."); return false; }
    FILE* file = fopen(filePath, "r");
    if (file == nullptr) { setJsonError(error, errorSize, "Could not open campaign JSON."); return false; }
    size_t count = fread(fileText, 1, MAX_FILE_SIZE - 1, file);
    fclose(file);
    fileText[count] = '\0';
    if (count == MAX_FILE_SIZE - 1) { setJsonError(error, errorSize, "Campaign JSON exceeds fixed file limit."); return false; }
    return true;
}

bool CampaignJson::splitLevels(char levelText[][MAX_LEVEL_TEXT], int maxLevels, int& levelCount,
                               char* error, int errorSize) const {
    levelCount = 0;
    const char* levels = strstr(fileText, "\"levels\"");
    if (levels == nullptr) { setJsonError(error, errorSize, "Campaign JSON has no levels array."); return false; }
    const char* p = strchr(levels, '[');
    if (p == nullptr) { setJsonError(error, errorSize, "Campaign levels array is malformed."); return false; }
    p++;
    while (*p != '\0') {
        while (*p != '\0' && (isspace(static_cast<unsigned char>(*p)) || *p == ',')) p++;
        if (*p == ']') return levelCount > 0;
        if (*p != '{') { setJsonError(error, errorSize, "Expected a level object in levels array."); return false; }
        if (levelCount >= maxLevels) { setJsonError(error, errorSize, "Campaign exceeds configured level capacity."); return false; }
        const char* begin = p;
        int depth = 0;
        bool inString = false;
        bool escaped = false;
        while (*p != '\0') {
            char c = *p++;
            if (inString) {
                if (escaped) escaped = false;
                else if (c == '\\') escaped = true;
                else if (c == '\"') inString = false;
            } else if (c == '\"') inString = true;
            else if (c == '{') depth++;
            else if (c == '}') {
                depth--;
                if (depth == 0) break;
            }
        }
        if (depth != 0) { setJsonError(error, errorSize, "Unclosed level object in campaign JSON."); return false; }
        int length = static_cast<int>(p - begin);
        if (length >= MAX_LEVEL_TEXT) { setJsonError(error, errorSize, "A level object exceeds fixed size limit."); return false; }
        strncpy(levelText[levelCount], begin, length);
        levelText[levelCount][length] = '\0';
        levelCount++;
    }
    setJsonError(error, errorSize, "Campaign levels array is not closed.");
    return false;
}

bool CampaignJson::readString(const char* objectText, const char* field, char* output, int outputSize) const {
    if (output == nullptr || outputSize <= 0) return false;
    output[0] = '\0';
    const char* p = findJsonField(objectText, field);
    if (p == nullptr || *p != '\"') return false;
    p++;
    int count = 0;
    while (*p != '\0' && *p != '\"') {
        if (*p == '\\' || count >= outputSize - 1) return false;
        output[count++] = *p++;
    }
    if (*p != '\"') return false;
    output[count] = '\0';
    return true;
}

bool CampaignJson::readInt(const char* objectText, const char* field, int& output) const {
    const char* p = findJsonField(objectText, field);
    if (p == nullptr) return false;
    return sscanf(p, "%d", &output) == 1;
}

bool CampaignJson::readIntArray(const char* objectText, const char* field, int* output, int maxCount, int& outputCount) const {
    outputCount = 0;
    const char* p = findJsonField(objectText, field);
    if (p == nullptr || *p != '[') return false;
    p++;
    while (*p != '\0') {
        while (*p != '\0' && isspace(static_cast<unsigned char>(*p))) p++;
        if (*p == ']') return true;
        if (outputCount >= maxCount || sscanf(p, "%d", &output[outputCount]) != 1) return false;
        outputCount++;
        while (*p != '\0' && *p != ',' && *p != ']') p++;
        if (*p == ',') p++;
    }
    return false;
}
