#include <SFML/Graphics.hpp>
#include <iostream>
#include "view/GameScreen.h"

using namespace std;

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Vault Heist: The Living Vault - Phase 3 GUI", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(60);

    GameScreen game;
    if (!game.initialize("assets")) {
        cout << "Notice: Could not load some texture or font assets from assets/ folder." << endl;
        cout << "Falling back to procedural bio-organic graphics." << endl;
    } else {
        cout << "Successfully loaded bio-organic hive assets and SFML 2.6.2 GUI." << endl;
    }

    cout << "=== VAULT HEIST: THE LIVING VAULT ===" << endl;
    cout << "Act 1: The Blueprint Wing (Binary Tree Operations - Member A)" << endl;
    cout << "Act 2: The Sorted Wing (BST Operations - Member A)" << endl;
    cout << "Act 3: The Self-Balancing Vault (AVL Tree Operations - Member B)" << endl;
    cout << "Act 4: The Priority Chamber (Max Heap & Array Dual View - Member B)" << endl;
    cout << "Controls:" << endl;
    cout << "  - Left Click        : Select pod / Click UI buttons / Click array cells / Focus input" << endl;
    cout << "  - Numeric Input     : Click 'Val:' input box to type numbers, or use +/- buttons" << endl;
    cout << "  - Right Click + Drag: Pan camera" << endl;
    cout << "  - Mouse Wheel       : Zoom in/out" << endl;
    cout << "  - Spacebar          : Pause / Resume animation" << endl;
    cout << "  - Right Arrow       : Step animation forward" << endl;
    cout << "  - Left Arrow        : Step animation backward" << endl;
    cout << "  - R Key             : Reset camera view" << endl;
    cout << "  - Escape            : Exit application" << endl;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            game.handleEvent(event, window);
        }

        game.update(dt);

        window.clear();
        game.render(window);
        window.display();
    }

    cout << "Vault Heist closed successfully." << endl;
    return 0;
}
