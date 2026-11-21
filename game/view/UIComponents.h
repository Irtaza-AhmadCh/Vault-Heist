#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include <cstring>
#include <cstdio>

struct BioButton {
    float x;
    float y;
    float width;
    float height;
    char label[32];
    bool isHovered;
    bool isPressed;
    bool isActive;
    int actionId;

    BioButton() {
        x = 0; y = 0; width = 120; height = 36;
        label[0] = '\0';
        isHovered = false;
        isPressed = false;
        isActive = false;
        actionId = 0;
    }

    BioButton(float px, float py, float pw, float ph, const char* text, int id, bool active = false) {
        x = px; y = py; width = pw; height = ph;
        strncpy(label, text, sizeof(label) - 1);
        label[sizeof(label) - 1] = '\0';
        isHovered = false;
        isPressed = false;
        isActive = active;
        actionId = id;
    }

    bool contains(float mx, float my) const {
        return mx >= x && mx <= x + width && my >= y && my <= y + height;
    }

    void update(float mx, float my) {
        isHovered = contains(mx, my);
    }

    void render(sf::RenderWindow& window, sf::Font& font) const {
        sf::RectangleShape box(sf::Vector2f(width, height));
        box.setPosition(x, y);

        if (isPressed) {
            box.setFillColor(sf::Color(26, 5, 11, 230));
            box.setOutlineColor(sf::Color(0, 180, 180, 255));
        } else if (isActive) {
            box.setFillColor(sf::Color(10, 50, 55, 240));
            box.setOutlineColor(sf::Color(64, 224, 208, 255));
        } else if (isHovered) {
            box.setFillColor(sf::Color(62, 12, 23, 240));
            box.setOutlineColor(sf::Color(64, 224, 208, 255));
        } else {
            box.setFillColor(sf::Color(45, 20, 44, 210));
            box.setOutlineColor(sf::Color(0, 85, 85, 200));
        }

        box.setOutlineThickness((isHovered || isActive) ? 2.5f : 1.5f);
        window.draw(box);

        if (isHovered || isActive) {
            sf::RectangleShape glow(sf::Vector2f(width + 4.0f, height + 4.0f));
            glow.setPosition(x - 2.0f, y - 2.0f);
            glow.setFillColor(sf::Color::Transparent);
            glow.setOutlineColor(sf::Color(64, 224, 208, isActive ? 140 : 90));
            glow.setOutlineThickness(2.0f);
            window.draw(glow);
        }

        sf::Text text;
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(14);
        text.setStyle(sf::Text::Bold);

        if (isHovered || isActive) {
            text.setFillColor(sf::Color::White);
        } else {
            text.setFillColor(sf::Color(224, 255, 255));
        }

        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        text.setPosition(x + width * 0.5f, y + height * 0.5f);
        window.draw(text);
    }
};

struct BioInputBox {
    float x;
    float y;
    float width;
    float height;
    char text[16];
    bool isFocused;
    bool isHovered;
    float cursorTimer;

    BioInputBox() {
        x = 0; y = 0; width = 58; height = 28;
        strncpy(text, "50", sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0';
        isFocused = false;
        isHovered = false;
        cursorTimer = 0.0f;
    }

    BioInputBox(float px, float py, float pw, float ph, const char* defaultVal = "50") {
        x = px; y = py; width = pw; height = ph;
        strncpy(text, defaultVal, sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0';
        isFocused = false;
        isHovered = false;
        cursorTimer = 0.0f;
    }

    bool contains(float mx, float my) const {
        return mx >= x && mx <= x + width && my >= y && my <= y + height;
    }

    void update(float mx, float my, float dt) {
        isHovered = contains(mx, my);
        cursorTimer += dt;
        if (cursorTimer > 1.0f) cursorTimer -= 1.0f;
    }

    void handleClick(float mx, float my) {
        isFocused = contains(mx, my);
    }

    int getValue(int fallback = 50) const {
        if (text[0] == '\0') return fallback;
        int val = atoi(text);
        return (val >= 0 && val <= 999) ? val : fallback;
    }

    void setValue(int val) {
        snprintf(text, sizeof(text), "%d", val);
    }

    void adjustValue(int delta) {
        int v = getValue(50) + delta;
        if (v < 1) v = 1;
        if (v > 99) v = 99;
        setValue(v);
    }

    void handleTextEntered(sf::Uint32 unicode) {
        if (!isFocused) return;
        if (unicode == 8) { // Backspace
            int len = strlen(text);
            if (len > 0) text[len - 1] = '\0';
        } else if (unicode >= '0' && unicode <= '9') {
            int len = strlen(text);
            if (len < 3) { // up to 3 digits (1-999)
                text[len] = static_cast<char>(unicode);
                text[len + 1] = '\0';
            }
        }
    }

    void render(sf::RenderWindow& window, sf::Font& font) const {
        // Label "Val:" before the box
        sf::Text labelText;
        labelText.setFont(font);
        labelText.setString("Val:");
        labelText.setCharacterSize(13);
        labelText.setStyle(sf::Text::Bold);
        labelText.setFillColor(sf::Color(64, 224, 208));
        labelText.setPosition(x - 30.0f, y + 5.0f);
        window.draw(labelText);

        sf::RectangleShape box(sf::Vector2f(width, height));
        box.setPosition(x, y);

        if (isFocused) {
            box.setFillColor(sf::Color(10, 40, 45, 240));
            box.setOutlineColor(sf::Color(0, 255, 255, 255));
            box.setOutlineThickness(2.0f);
        } else if (isHovered) {
            box.setFillColor(sf::Color(45, 15, 30, 230));
            box.setOutlineColor(sf::Color(64, 224, 208, 220));
            box.setOutlineThickness(1.5f);
        } else {
            box.setFillColor(sf::Color(30, 10, 25, 210));
            box.setOutlineColor(sf::Color(0, 85, 85, 180));
            box.setOutlineThickness(1.0f);
        }
        window.draw(box);

        char displayStr[20];
        if (isFocused && cursorTimer < 0.5f) {
            snprintf(displayStr, sizeof(displayStr), "%s|", text);
        } else {
            snprintf(displayStr, sizeof(displayStr), "%s", text);
        }

        sf::Text textObj;
        textObj.setFont(font);
        textObj.setString(displayStr);
        textObj.setCharacterSize(14);
        textObj.setStyle(sf::Text::Bold);
        textObj.setFillColor(isFocused ? sf::Color::White : sf::Color(224, 255, 255));
        sf::FloatRect bounds = textObj.getLocalBounds();
        textObj.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        textObj.setPosition(x + width * 0.5f, y + height * 0.5f);
        window.draw(textObj);
    }
};

class HUD {
public:
    int alarmSteps;
    int alarmCapacity;
    int score;
    char statusMessage[128];
    char subMessage[128];
    char animStatus[32];

    HUD() {
        alarmSteps = 0;
        alarmCapacity = 10;
        score = 1000;
        strncpy(statusMessage, "SYSTEM READY: Explore the Living Vault", sizeof(statusMessage) - 1);
        subMessage[0] = '\0';
        strncpy(animStatus, "IDLE", sizeof(animStatus) - 1);
    }

    void setStatus(const char* msg, const char* sub = "") {
        strncpy(statusMessage, msg, sizeof(statusMessage) - 1);
        statusMessage[sizeof(statusMessage) - 1] = '\0';
        strncpy(subMessage, sub, sizeof(subMessage) - 1);
        subMessage[sizeof(subMessage) - 1] = '\0';
    }

    void setAnimStatus(const char* state, float speed) {
        if (strcmp(state, "playing") == 0) {
            snprintf(animStatus, sizeof(animStatus), "PLAY (%.1fx)", speed);
        } else if (strcmp(state, "paused") == 0) {
            strncpy(animStatus, "PAUSED", sizeof(animStatus) - 1);
        } else if (strcmp(state, "completed") == 0) {
            strncpy(animStatus, "DONE", sizeof(animStatus) - 1);
        } else {
            strncpy(animStatus, "IDLE", sizeof(animStatus) - 1);
        }
    }

    void addAlarmStep(int steps = 1) {
        alarmSteps += steps;
        score -= steps * 10;
        if (score < 0) score = 0;
    }

    void resetAlarm(int capacity = 10) {
        alarmSteps = 0;
        alarmCapacity = capacity;
        score = 1000;
    }

    void render(sf::RenderWindow& window, sf::Font& font, bool showScore = true) {
        // Top HUD Panel (Bio-organic bar)
        sf::RectangleShape topBar(sf::Vector2f(1280.0f, 52.0f));
        topBar.setPosition(0.0f, 0.0f);
        topBar.setFillColor(sf::Color(15, 5, 20, 230));
        topBar.setOutlineColor(sf::Color(0, 85, 85, 200));
        topBar.setOutlineThickness(1.5f);
        window.draw(topBar);

        // Title text
        sf::Text title;
        title.setFont(font);
        title.setString("VAULT HEIST: THE LIVING VAULT");
        title.setCharacterSize(16);
        title.setStyle(sf::Text::Bold);
        title.setFillColor(sf::Color(64, 224, 208));
        title.setPosition(20.0f, 15.0f);
        window.draw(title);

        // Alarm Meter
        sf::Text alarmLabel;
        alarmLabel.setFont(font);
        char alarmBuf[64];
        snprintf(alarmBuf, sizeof(alarmBuf), "ALARM: %d / %d", alarmSteps, alarmCapacity);
        alarmLabel.setString(alarmBuf);
        alarmLabel.setCharacterSize(14);
        alarmLabel.setStyle(sf::Text::Bold);

        float alarmRatio = (alarmCapacity > 0) ? (static_cast<float>(alarmSteps) / alarmCapacity) : 0.0f;
        if (alarmRatio > 1.0f) alarmRatio = 1.0f;

        if (alarmRatio >= 0.8f) {
            alarmLabel.setFillColor(sf::Color(255, 60, 60));
        } else if (alarmRatio >= 0.5f) {
            alarmLabel.setFillColor(sf::Color(255, 200, 50));
        } else {
            alarmLabel.setFillColor(sf::Color(64, 224, 208));
        }
        alarmLabel.setPosition(520.0f, 16.0f);
        window.draw(alarmLabel);

        // Alarm Bar Background
        sf::RectangleShape barBg(sf::Vector2f(180.0f, 14.0f));
        barBg.setPosition(660.0f, 18.0f);
        barBg.setFillColor(sf::Color(30, 10, 20, 220));
        barBg.setOutlineColor(sf::Color(80, 20, 40));
        barBg.setOutlineThickness(1.0f);
        window.draw(barBg);

        // Alarm Bar Fill
        if (alarmRatio > 0.0f) {
            sf::RectangleShape barFill(sf::Vector2f(180.0f * alarmRatio, 14.0f));
            barFill.setPosition(660.0f, 18.0f);
            if (alarmRatio >= 0.8f) {
                barFill.setFillColor(sf::Color(240, 40, 50, 230));
            } else if (alarmRatio >= 0.5f) {
                barFill.setFillColor(sf::Color(240, 180, 30, 230));
            } else {
                barFill.setFillColor(sf::Color(0, 200, 180, 230));
            }
            window.draw(barFill);
        }

        if (showScore) {
            sf::Text scoreText;
            scoreText.setFont(font);
            char scoreBuf[32];
            snprintf(scoreBuf, sizeof(scoreBuf), "SCORE: %d", score);
            scoreText.setString(scoreBuf);
            scoreText.setCharacterSize(15);
            scoreText.setStyle(sf::Text::Bold);
            scoreText.setFillColor(sf::Color(224, 255, 255));
            scoreText.setPosition(890.0f, 16.0f);
            window.draw(scoreText);
        }

        // Animation playback status
        sf::Text animText;
        animText.setFont(font);
        char animBuf[48];
        snprintf(animBuf, sizeof(animBuf), "ANIM: [%s]", animStatus);
        animText.setString(animBuf);
        animText.setCharacterSize(14);
        animText.setStyle(sf::Text::Bold);
        if (strncmp(animStatus, "PLAY", 4) == 0) {
            animText.setFillColor(sf::Color(64, 255, 128));
        } else if (strcmp(animStatus, "PAUSED") == 0) {
            animText.setFillColor(sf::Color(255, 200, 50));
        } else {
            animText.setFillColor(sf::Color(180, 220, 240));
        }
        animText.setPosition(1050.0f, 16.0f);
        window.draw(animText);

        // Bottom Status Banner
        sf::RectangleShape botBar(sf::Vector2f(1280.0f, 38.0f));
        botBar.setPosition(0.0f, 682.0f);
        botBar.setFillColor(sf::Color(15, 5, 20, 230));
        botBar.setOutlineColor(sf::Color(0, 85, 85, 180));
        botBar.setOutlineThickness(1.0f);
        window.draw(botBar);

        sf::Text statusText;
        statusText.setFont(font);
        statusText.setString(statusMessage);
        statusText.setCharacterSize(14);
        statusText.setFillColor(sf::Color(224, 255, 255));
        statusText.setPosition(20.0f, 691.0f);
        window.draw(statusText);

        if (subMessage[0] != '\0') {
            sf::Text subText;
            subText.setFont(font);
            subText.setString(subMessage);
            subText.setCharacterSize(13);
            subText.setFillColor(sf::Color(64, 224, 208));
            subText.setPosition(750.0f, 691.0f);
            window.draw(subText);
        }
    }
};

#endif
