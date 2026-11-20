#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // 1. Create a 800x600 window with anti-aliasing
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML 2.6.2 - DS Pro", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(60);

    // 2. Create an interactive shape (circle)
    float radius = 50.f;
    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color(76, 175, 80));       // Material Green
    circle.setOutlineThickness(4.f);
    circle.setOutlineColor(sf::Color::White);
    circle.setOrigin(radius, radius);                  // Center origin
    circle.setPosition(400.f, 300.f);

    // Movement velocity for auto-bounce
    sf::Vector2f velocity(200.f, 150.f);
    sf::Clock clock;

    // Palette for color switching
    const sf::Color colors[] = {
        sf::Color(76, 175, 80),   // Green
        sf::Color(33, 150, 243),  // Blue
        sf::Color(255, 152, 0),   // Orange
        sf::Color(156, 39, 176),  // Purple
        sf::Color(244, 67, 54)    // Red
    };
    int colorIndex = 0;

    std::cout << "SFML Application Started Successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - Spacebar   : Change shape color" << std::endl;
    std::cout << "  - Left Click : Move circle to cursor" << std::endl;
    std::cout << "  - Escape     : Exit application" << std::endl;

    // 3. Main game loop
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::Space) {
                    colorIndex = (colorIndex + 1) % 5;
                    circle.setFillColor(colors[colorIndex]);
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    circle.setPosition(static_cast<float>(event.mouseButton.x),
                                       static_cast<float>(event.mouseButton.y));
                }
            }
        }

        // Update circle position (bouncing animation)
        sf::Vector2f pos = circle.getPosition();
        pos += velocity * dt;

        // Bounce on horizontal borders
        if (pos.x - radius < 0.f) {
            pos.x = radius;
            velocity.x = -velocity.x;
        } else if (pos.x + radius > 800.f) {
            pos.x = 800.f - radius;
            velocity.x = -velocity.x;
        }

        // Bounce on vertical borders
        if (pos.y - radius < 0.f) {
            pos.y = radius;
            velocity.y = -velocity.y;
        } else if (pos.y + radius > 600.f) {
            pos.y = 600.f - radius;
            velocity.y = -velocity.y;
        }

        circle.setPosition(pos);

        // Render frame
        window.clear(sf::Color(24, 24, 28)); // Dark modern background
        window.draw(circle);
        window.display();
    }

    std::cout << "SFML Application Closed." << std::endl;
    return 0;
}

