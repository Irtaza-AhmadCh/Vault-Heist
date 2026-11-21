#ifndef CAMPAIGN_JSON_H
#define CAMPAIGN_JSON_H

class CampaignJson {
public:
    static const int MAX_FILE_SIZE = 65536;
    static const int MAX_LEVEL_TEXT = 4096;

    bool loadFile(const char* filePath, char* error, int errorSize);
    bool splitLevels(char levelText[][MAX_LEVEL_TEXT], int maxLevels, int& levelCount,
                     char* error, int errorSize) const;
    bool readString(const char* objectText, const char* field, char* output, int outputSize) const;
    bool readInt(const char* objectText, const char* field, int& output) const;
    bool readIntArray(const char* objectText, const char* field, int* output, int maxCount, int& outputCount) const;

private:
    char fileText[MAX_FILE_SIZE];
};

#endif
