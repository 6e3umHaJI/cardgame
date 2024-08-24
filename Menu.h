#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"

class Menu
{
public:
    std::vector<Button> buttons;
    int activeButtonIndex = -1;

    Menu();
    Menu(std::vector<Button> buttons);
    Menu(std::vector<Button> buttons, int distanceY);
    int isButtonActive(sf::Vector2u cursorPosition);
    int isButtonHovered(sf::Vector2u cursorPosition);
    void reset();
    void draw(sf::RenderWindow& window);
};

#endif // MENU_H
