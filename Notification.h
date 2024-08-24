#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <SFML/Graphics.hpp>
#include "Button.h"

class Notification
{
public:
    sf::RectangleShape shape;
    sf::Text message;
    Button buttonOK = Button(L"Œ ", sf::Vector2u(0, 850), sf::Vector2u(170, 50));

    Notification(std::wstring messageText);
    void updateMessage(std::wstring newText);
    void reset();
    void draw(sf::RenderWindow& window);
};

#endif // NOTIFICATION_H
