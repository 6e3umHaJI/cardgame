#include <SFML/Graphics.hpp>
#include "globals.h"
#include "StateManager.h"

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 10;

    sf::RenderWindow window(sf::VideoMode(globals::screenWidth, globals::screenHeight), "Durak", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(globals::frameLimit);

    globals::loadTextures();
    globals::loadFont();

    StateManager stateManager;
    stateManager.addState(State::MENU);
    int i = 0;
    while (window.isOpen())
    {
        i++;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            stateManager.handleEvent(event, window);
        }

        stateManager.draw(window);
        window.display();
    }

    return 0;
}