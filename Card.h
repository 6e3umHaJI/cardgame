#ifndef CARD_H
#define CARD_H
#include <string>
#include "globals.h"
#include <SFML/Graphics.hpp>

class Card
{
public:
    unsigned short face;
    unsigned short suit;
    sf::Sprite sprite;
    sf::Vector2f size = sf::Vector2f(99, 143);

    Card();
    Card(unsigned short face, unsigned short suit);
    bool fight(Card& a, unsigned short trump);
    void setFace();
    void setBack();
    void setPosition(sf::Vector2u pos);
    void draw(sf::RenderWindow& window);
};

#endif // CARD_H