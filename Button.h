#ifndef BUTTON_H
#define BUTTON_H
#include <SFML/Graphics.hpp>
#include <vector>
#include "globals.h"

class Button
{
public:
    sf::RectangleShape shape;
    sf::Sprite sprite;
    sf::Text text;
    sf::Color colorDefault, colorHovered, colorActive, colorBlocked = sf::Color(139, 147, 148);
    bool active = false;
    bool hovered = false;
    bool blocked = false;

    Button(std::wstring wstr = L"", sf::Vector2u position = sf::Vector2u(0, 0), sf::Vector2u size = sf::Vector2u(200, 100), sf::Color colorDefault = sf::Color(255, 224, 56), sf::Color colorHovered = sf::Color(255, 188, 60), sf::Color colorActive = sf::Color(255, 157, 39));
    Button(sf::Texture &texture, sf::Vector2u position = sf::Vector2u(0, 0), sf::Vector2u size = sf::Vector2u(200, 100), sf::Color colorDefault = sf::Color(255, 224, 56), sf::Color colorHovered = sf::Color(255, 188, 60), sf::Color colorActive = sf::Color(255, 157, 39));
    void setPosition(sf::Vector2u position);
    bool isActive(sf::Vector2u cursorPosition);
    bool isHovered(sf::Vector2u cursorPosition);
    void updateText(std::wstring newText);
    void block();
    void activate();
    void deactivate();
    void draw(sf::RenderWindow &window);
};

#endif // BUTTON_H
