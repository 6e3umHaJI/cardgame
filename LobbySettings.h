#ifndef LOBBYSETTINGS_H
#define LOBBYSETTINGS_H
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Menu.h"

class LobbySettings
{
public:
    sf::RectangleShape shape;
    sf::Text message;
    Menu menu;
    Button buttonCancel = Button(L"Отмена", sf::Vector2u(0, 850), sf::Vector2u(150, 40)), buttonCreate = Button(L"Создать", sf::Vector2u(0, 850), sf::Vector2u(150, 40));

    LobbySettings();
    void reset();
    void draw(sf::RenderWindow& window);
};

#endif // LOBBYSETTINGS_H
