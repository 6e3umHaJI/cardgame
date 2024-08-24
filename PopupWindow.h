#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H
#include <SFML/Graphics.hpp>
#include "Button.h"

class PopupWindow
{
public:
    sf::RectangleShape shape;
    sf::Text message;
    sf::RectangleShape textField;
    sf::Text inputText;
    Button buttonCancel = Button(L"Отмена", sf::Vector2u(0, 850), sf::Vector2u(150, 40)), buttonOK = Button(L"ОК", sf::Vector2u(0, 850), sf::Vector2u(150, 40));
    bool activeTextField = true, hoveredTextField = false, showCursor = false;
    sf::Clock clock;

    PopupWindow(std::wstring messageText);
    bool isActiveTextField(sf::Vector2u cursorPosition);
    bool isHoveredTextField(sf::Vector2u cursorPosition);
    void updateMessage(std::wstring newText);
    void reset();
    void updateBlinkingCursor();
    void draw(sf::RenderWindow& window);
};

#endif // POPUPWINDOW_H
