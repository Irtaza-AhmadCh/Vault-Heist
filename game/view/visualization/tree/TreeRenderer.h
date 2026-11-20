#ifndef TREE_RENDERER_H
#define TREE_RENDERER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <cstdio>
#include "TreeLayout.h"

struct SporeParticle {
    float x;
    float y;
    float vx;
    float vy;
    float radius;
    float alpha;
    float maxAlpha;

    SporeParticle() {
        x = 0.0f;
        y = 0.0f;
        vx = 0.0f;
        vy = -15.0f;
        radius = 2.0f;
        alpha = 0.0f;
        maxAlpha = 150.0f;
    }
};

class TreeRenderer {
public:
    sf::Texture nodeDefaultTex;
    sf::Texture nodeHighlightTex;
    sf::Texture nodeInactiveTex;
    sf::Texture edgeTex;
    sf::Texture bgTex;
    sf::Sprite bgSprite;
    sf::Font font;

    bool hasNodeDefaultTex;
    bool hasNodeHighlightTex;
    bool hasNodeInactiveTex;
    bool hasEdgeTex;
    bool hasBgTex;
    bool hasFont;

    static const int MAX_SPORES = 40;
    SporeParticle spores[MAX_SPORES];

    TreeRenderer() {
        hasNodeDefaultTex = false;
        hasNodeHighlightTex = false;
        hasNodeInactiveTex = false;
        hasEdgeTex = false;
        hasBgTex = false;
        hasFont = false;

        initSpores();
    }

    void initSpores() {
        for (int i = 0; i < MAX_SPORES; i++) {
            spores[i].x = static_cast<float>(rand() % 1280);
            spores[i].y = static_cast<float>(rand() % 720);
            spores[i].vx = (static_cast<float>(rand() % 20) - 10.0f) * 0.5f;
            spores[i].vy = -(static_cast<float>(rand() % 20) + 10.0f);
            spores[i].radius = static_cast<float>(rand() % 3 + 2);
            spores[i].maxAlpha = static_cast<float>(rand() % 100 + 50);
            spores[i].alpha = static_cast<float>(rand() % 100);
        }
    }

    bool loadAssets(const char* assetsDir) {
        char path[256];

        snprintf(path, sizeof(path), "%s/textures/node1.png", assetsDir);
        hasNodeDefaultTex = nodeDefaultTex.loadFromFile(path);
        nodeDefaultTex.setSmooth(true);

        snprintf(path, sizeof(path), "%s/textures/node2.png", assetsDir);
        hasNodeHighlightTex = nodeHighlightTex.loadFromFile(path);
        nodeHighlightTex.setSmooth(true);

        snprintf(path, sizeof(path), "%s/textures/node3.png", assetsDir);
        hasNodeInactiveTex = nodeInactiveTex.loadFromFile(path);
        nodeInactiveTex.setSmooth(true);

        snprintf(path, sizeof(path), "%s/textures/edge2.png", assetsDir);
        hasEdgeTex = edgeTex.loadFromFile(path);
        edgeTex.setSmooth(true);
        edgeTex.setRepeated(true);

        snprintf(path, sizeof(path), "%s/textures/Gemini_Generated_Image_byipdtbyipdtbyip.jfif", assetsDir);
        hasBgTex = bgTex.loadFromFile(path);
        if (hasBgTex) {
            bgTex.setSmooth(true);
            bgSprite.setTexture(bgTex);
            sf::Vector2u sz = bgTex.getSize();
            if (sz.x > 0 && sz.y > 0) {
                bgSprite.setScale(1280.0f / sz.x, 720.0f / sz.y);
            }
        }

        snprintf(path, sizeof(path), "%s/fonts/arial.ttf", assetsDir);
        hasFont = font.loadFromFile(path);

        return true;
    }

    void updateSpores(float dt) {
        for (int i = 0; i < MAX_SPORES; i++) {
            spores[i].x += spores[i].vx * dt;
            spores[i].y += spores[i].vy * dt;

            if (spores[i].y < -10.0f) {
                spores[i].y = 730.0f;
                spores[i].x = static_cast<float>(rand() % 1280);
            }
            if (spores[i].x < 0.0f) spores[i].x = 1280.0f;
            if (spores[i].x > 1280.0f) spores[i].x = 0.0f;
        }
    }

    void renderBackground(sf::RenderWindow& window) {
        if (hasBgTex) {
            window.draw(bgSprite);
            sf::RectangleShape darkOverlay(sf::Vector2f(1280.0f, 720.0f));
            darkOverlay.setFillColor(sf::Color(15, 5, 20, 140));
            window.draw(darkOverlay);
        } else {
            window.clear(sf::Color(15, 5, 20));
        }

        for (int i = 0; i < MAX_SPORES; i++) {
            sf::CircleShape sporeCircle(spores[i].radius);
            sporeCircle.setOrigin(spores[i].radius, spores[i].radius);
            sporeCircle.setPosition(spores[i].x, spores[i].y);
            sporeCircle.setFillColor(sf::Color(64, 224, 208, static_cast<sf::Uint8>(spores[i].maxAlpha)));
            window.draw(sporeCircle);
        }
    }

    void renderEdge(sf::RenderWindow& window, const VisualNode& from, const VisualNode& to, bool isHighlighted) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        float length = std::sqrt(dx * dx + dy * dy);
        if (length < 1.0f) return;

        float angle = std::atan2(dy, dx) * 180.0f / 3.14159265f;

        float thickness = isHighlighted ? 12.0f : 8.0f;
        sf::RectangleShape vein(sf::Vector2f(length, thickness));
        vein.setOrigin(0.0f, thickness * 0.5f);
        vein.setPosition(from.x, from.y);
        vein.setRotation(angle);

        if (hasEdgeTex) {
            vein.setTexture(&edgeTex);
            vein.setTextureRect(sf::IntRect(0, 0, static_cast<int>(length), static_cast<int>(thickness * 4.0f)));
            if (isHighlighted) {
                vein.setFillColor(sf::Color(0, 255, 255, 230));
            } else {
                vein.setFillColor(sf::Color(160, 40, 70, 210));
            }
        } else {
            if (isHighlighted) {
                vein.setFillColor(sf::Color(64, 224, 208, 220));
            } else {
                vein.setFillColor(sf::Color(139, 0, 80, 180));
            }
        }

        window.draw(vein);

        sf::RectangleShape coreLine(sf::Vector2f(length, isHighlighted ? 3.0f : 1.5f));
        coreLine.setOrigin(0.0f, coreLine.getSize().y * 0.5f);
        coreLine.setPosition(from.x, from.y);
        coreLine.setRotation(angle);
        if (isHighlighted) {
            coreLine.setFillColor(sf::Color(240, 255, 255, 255));
        } else {
            coreLine.setFillColor(sf::Color(64, 224, 208, 120));
        }
        window.draw(coreLine);
    }

    void renderAVLBadges(sf::RenderWindow& window, const VisualNode& node) {
        if (!hasFont) return;

        // Balance Factor Badge (Top-Right)
        float bfX = node.x + node.radius * 0.72f;
        float bfY = node.y - node.radius * 0.85f;
        float badgeR = 11.5f;

        sf::CircleShape bfBubble(badgeR);
        bfBubble.setOrigin(badgeR, badgeR);
        bfBubble.setPosition(bfX, bfY);

        int bf = node.balanceFactor;
        bool isImbalanced = (bf > 1 || bf < -1);

        if (isImbalanced) {
            bfBubble.setFillColor(sf::Color(230, 35, 45, 240));
            bfBubble.setOutlineColor(sf::Color(255, 220, 220));
            bfBubble.setOutlineThickness(2.0f);
        } else {
            bfBubble.setFillColor(sf::Color(15, 45, 50, 230));
            bfBubble.setOutlineColor(sf::Color(64, 224, 208, 200));
            bfBubble.setOutlineThickness(1.5f);
        }
        window.draw(bfBubble);

        char bfText[8];
        if (bf > 0) snprintf(bfText, sizeof(bfText), "+%d", bf);
        else snprintf(bfText, sizeof(bfText), "%d", bf);

        sf::Text textBF;
        textBF.setFont(font);
        textBF.setString(bfText);
        textBF.setCharacterSize(10);
        textBF.setStyle(sf::Text::Bold);
        textBF.setFillColor(isImbalanced ? sf::Color::White : sf::Color(210, 255, 255));
        sf::FloatRect bfb = textBF.getLocalBounds();
        textBF.setOrigin(bfb.left + bfb.width * 0.5f, bfb.top + bfb.height * 0.5f);
        textBF.setPosition(bfX, bfY);
        window.draw(textBF);

        // Subtree Size Badge (Bottom-Right)
        float szX = node.x + node.radius * 0.65f;
        float szY = node.y + node.radius * 0.85f;

        sf::RectangleShape szBox(sf::Vector2f(28.0f, 15.0f));
        szBox.setOrigin(14.0f, 7.5f);
        szBox.setPosition(szX, szY);
        szBox.setFillColor(sf::Color(25, 10, 30, 220));
        szBox.setOutlineColor(sf::Color(110, 45, 95, 220));
        szBox.setOutlineThickness(1.2f);
        window.draw(szBox);

        char szText[12];
        snprintf(szText, sizeof(szText), "s:%d", node.subtreeSize);
        sf::Text textSZ;
        textSZ.setFont(font);
        textSZ.setString(szText);
        textSZ.setCharacterSize(9);
        textSZ.setStyle(sf::Text::Bold);
        textSZ.setFillColor(sf::Color(220, 240, 255));
        sf::FloatRect szb = textSZ.getLocalBounds();
        textSZ.setOrigin(szb.left + szb.width * 0.5f, szb.top + szb.height * 0.5f);
        textSZ.setPosition(szX, szY);
        window.draw(textSZ);
    }

    void renderNode(sf::RenderWindow& window, const VisualNode& node, bool isSelected, bool showAVLBadges = false) {
        float scale = 1.0f;
        if (node.isState("highlighted") || node.isState("searching") || node.isState("target") || isSelected) {
            scale = 1.0f + 0.08f * std::sin(node.pulseTimer * 6.0f);
        }

        float renderRadius = node.radius * scale;

        sf::Texture* tex = &nodeDefaultTex;
        if (node.isState("highlighted") || node.isState("target") || isSelected) {
            tex = hasNodeHighlightTex ? &nodeHighlightTex : &nodeDefaultTex;
        } else if (node.isState("inactive")) {
            tex = hasNodeInactiveTex ? &nodeInactiveTex : &nodeDefaultTex;
        }

        if (tex != nullptr && tex->getSize().x > 0) {
            sf::Sprite sprite(*tex);
            sf::Vector2u tsz = tex->getSize();
            sprite.setOrigin(tsz.x * 0.5f, tsz.y * 0.5f);
            float baseScale = (renderRadius * 2.0f) / tsz.x;
            sprite.setScale(baseScale, baseScale);
            sprite.setPosition(node.x, node.y);
            window.draw(sprite);
        } else {
            sf::CircleShape pod(renderRadius);
            pod.setOrigin(renderRadius, renderRadius);
            pod.setPosition(node.x, node.y);

            if (node.isState("highlighted") || node.isState("target") || isSelected) {
                pod.setFillColor(sf::Color(0, 255, 255, 240));
                pod.setOutlineColor(sf::Color(255, 255, 255, 255));
            } else if (node.isState("searching")) {
                pod.setFillColor(sf::Color(255, 215, 0, 240));
                pod.setOutlineColor(sf::Color(64, 224, 208, 255));
            } else if (node.isState("inactive")) {
                pod.setFillColor(sf::Color(45, 20, 35, 200));
                pod.setOutlineColor(sf::Color(74, 14, 78, 255));
            } else {
                pod.setFillColor(sf::Color(0, 128, 128, 230));
                pod.setOutlineColor(sf::Color(64, 224, 208, 255));
            }
            pod.setOutlineThickness(3.0f);
            window.draw(pod);
        }

        if (hasFont) {
            char valStr[16];
            snprintf(valStr, sizeof(valStr), "%d", node.key);

            sf::Text shadowText;
            shadowText.setFont(font);
            shadowText.setString(valStr);
            shadowText.setCharacterSize(18);
            shadowText.setStyle(sf::Text::Bold);
            shadowText.setFillColor(sf::Color(5, 5, 10, 230));
            sf::FloatRect bounds = shadowText.getLocalBounds();
            shadowText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
            shadowText.setPosition(node.x + 1.5f, node.y + 1.5f);
            window.draw(shadowText);

            sf::Text mainText;
            mainText.setFont(font);
            mainText.setString(valStr);
            mainText.setCharacterSize(18);
            mainText.setStyle(sf::Text::Bold);
            if (node.isState("highlighted") || node.isState("target") || isSelected) {
                mainText.setFillColor(sf::Color::White);
            } else if (node.isState("searching")) {
                mainText.setFillColor(sf::Color(255, 255, 200));
            } else if (node.isState("inactive")) {
                mainText.setFillColor(sf::Color(140, 120, 140));
            } else {
                mainText.setFillColor(sf::Color(224, 255, 255));
            }
            mainText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
            mainText.setPosition(node.x, node.y);
            window.draw(mainText);
        }

        if (showAVLBadges) {
            renderAVLBadges(window, node);
        }
    }

    void renderTree(sf::RenderWindow& window, const TreeLayoutEngine& layout, int selectedNodeIndex, bool showAVLBadges = false) {
        for (int i = 0; i < layout.nodeCount; i++) {
            if (!layout.nodes[i].active) continue;

            int leftChild = layout.nodes[i].leftChildIndex;
            if (leftChild != -1 && layout.nodes[leftChild].active) {
                bool high = ((layout.nodes[i].isState("highlighted") || layout.nodes[i].isState("searching")) &&
                             (layout.nodes[leftChild].isState("highlighted") || layout.nodes[leftChild].isState("searching")));
                renderEdge(window, layout.nodes[i], layout.nodes[leftChild], high);
            }

            int rightChild = layout.nodes[i].rightChildIndex;
            if (rightChild != -1 && layout.nodes[rightChild].active) {
                bool high = ((layout.nodes[i].isState("highlighted") || layout.nodes[i].isState("searching")) &&
                             (layout.nodes[rightChild].isState("highlighted") || layout.nodes[rightChild].isState("searching")));
                renderEdge(window, layout.nodes[i], layout.nodes[rightChild], high);
            }
        }

        for (int i = 0; i < layout.nodeCount; i++) {
            if (!layout.nodes[i].active) continue;
            renderNode(window, layout.nodes[i], i == selectedNodeIndex, showAVLBadges);
        }
    }

    void renderHeapArray(sf::RenderWindow& window, const int* heapArr, int heapSize, int selectedIndex, const TreeLayoutEngine& layout) {
        if (heapSize <= 0 || heapArr == nullptr) return;

        float cellW = 54.0f;
        float cellH = 44.0f;
        float gap = 8.0f;
        float totalW = heapSize * cellW + (heapSize - 1) * gap;
        float startX = 640.0f - totalW * 0.5f;
        if (startX < 30.0f) startX = 30.0f;
        float startY = 595.0f;

        // Container panel
        sf::RectangleShape panelBg(sf::Vector2f(totalW + 24.0f, cellH + 34.0f));
        panelBg.setPosition(startX - 12.0f, startY - 18.0f);
        panelBg.setFillColor(sf::Color(15, 5, 22, 235));
        panelBg.setOutlineColor(sf::Color(0, 85, 85, 200));
        panelBg.setOutlineThickness(1.5f);
        window.draw(panelBg);

        if (hasFont) {
            sf::Text headerText;
            headerText.setFont(font);
            headerText.setString("HEAP ARRAY (1D SEQUENTIAL MEMORY BUFFER)");
            headerText.setCharacterSize(11);
            headerText.setStyle(sf::Text::Bold);
            headerText.setFillColor(sf::Color(64, 224, 208));
            headerText.setPosition(startX, startY - 15.0f);
            window.draw(headerText);
        }

        for (int i = 0; i < heapSize; i++) {
            float cx = startX + i * (cellW + gap);
            float cy = startY + 6.0f;

            bool isSelected = (i == selectedIndex);
            bool isSorted = (i < layout.nodeCount && layout.nodes[i].active &&
                            (layout.nodes[i].isState("sorted") || layout.nodes[i].isState("target")));

            sf::RectangleShape cell(sf::Vector2f(cellW, cellH));
            cell.setPosition(cx, cy);

            if (isSelected) {
                cell.setFillColor(sf::Color(10, 60, 65, 240));
                cell.setOutlineColor(sf::Color(0, 255, 255, 255));
                cell.setOutlineThickness(2.5f);
            } else if (isSorted) {
                cell.setFillColor(sf::Color(40, 30, 10, 220));
                cell.setOutlineColor(sf::Color(255, 215, 0, 230));
                cell.setOutlineThickness(2.0f);
            } else {
                cell.setFillColor(sf::Color(45, 15, 35, 210));
                cell.setOutlineColor(sf::Color(139, 0, 80, 200));
                cell.setOutlineThickness(1.5f);
            }
            window.draw(cell);

            if (hasFont) {
                char valStr[16];
                snprintf(valStr, sizeof(valStr), "%d", heapArr[i]);

                sf::Text valText;
                valText.setFont(font);
                valText.setString(valStr);
                valText.setCharacterSize(15);
                valText.setStyle(sf::Text::Bold);
                valText.setFillColor(isSelected ? sf::Color::White : sf::Color(224, 255, 255));
                sf::FloatRect vb = valText.getLocalBounds();
                valText.setOrigin(vb.left + vb.width * 0.5f, vb.top + vb.height * 0.5f);
                valText.setPosition(cx + cellW * 0.5f, cy + cellH * 0.5f);
                window.draw(valText);

                char idxStr[12];
                snprintf(idxStr, sizeof(idxStr), "[%d]", i);
                sf::Text idxText;
                idxText.setFont(font);
                idxText.setString(idxStr);
                idxText.setCharacterSize(10);
                idxText.setFillColor(isSelected ? sf::Color(64, 224, 208) : sf::Color(150, 150, 170));
                sf::FloatRect ib = idxText.getLocalBounds();
                idxText.setOrigin(ib.left + ib.width * 0.5f, ib.top + ib.height * 0.5f);
                idxText.setPosition(cx + cellW * 0.5f, cy + cellH + 9.0f);
                window.draw(idxText);
            }

            // Draw connecting line between tree node and array cell if selected
            if (isSelected && i < layout.nodeCount && layout.nodes[i].active) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(layout.nodes[i].x, layout.nodes[i].y), sf::Color(0, 255, 255, 180)),
                    sf::Vertex(sf::Vector2f(cx + cellW * 0.5f, cy), sf::Color(64, 224, 208, 120))
                };
                window.draw(line, 2, sf::Lines);
            }
        }
    }

    int findHeapArrayCellAt(float mouseX, float mouseY, int heapSize) const {
        if (heapSize <= 0) return -1;
        float cellW = 54.0f;
        float cellH = 44.0f;
        float gap = 8.0f;
        float totalW = heapSize * cellW + (heapSize - 1) * gap;
        float startX = 640.0f - totalW * 0.5f;
        if (startX < 30.0f) startX = 30.0f;
        float startY = 595.0f + 6.0f;

        if (mouseY >= startY && mouseY <= startY + cellH) {
            for (int i = 0; i < heapSize; i++) {
                float cx = startX + i * (cellW + gap);
                if (mouseX >= cx && mouseX <= cx + cellW) {
                    return i;
                }
            }
        }
        return -1;
    }
};

#endif
