#include "Card.h"
#include "globals.h"

Card::Card() {}
Card::Card(unsigned short face, unsigned short suit)
{
    this->face = face;
    this->suit = suit;
}

bool Card::fight(Card& a, unsigned short trump)
{
    if (this->suit == a.suit)
        return ((this->face > a.face) ? true : false);
    else if ((this->suit == trump) && (a.suit != trump))
        return true;
    else
        return false;
}

void Card::setFace()
{
    // ”казываем координаты и размеры части изображени€
    sf::IntRect rect(face*79, suit*123, 79, 123);
    sprite.setTexture(globals::textCards);
    sprite.setTextureRect(rect);
    sprite.setScale(size.x / sprite.getGlobalBounds().width, size.y / sprite.getGlobalBounds().height);
}
void Card::setBack()
{
    sprite.setTexture(globals::textBack);
    sprite.setScale(size.x / sprite.getGlobalBounds().width, size.y / sprite.getGlobalBounds().height);
}

void Card::setPosition(sf::Vector2u pos)
{
    sprite.setPosition(pos.x, pos.y);
}

void Card::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}