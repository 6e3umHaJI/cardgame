#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <memory>
#include "Deck.h"
#include "Pair.h"
#include <SFML/Graphics.hpp>

class Player
{
public:
    int id;
    std::wstring nickname;
    sf::Text text;
    std::vector<Card*> hand;

    Player(std::wstring nickname, int id);
    void sort();
    void draw(sf::RenderWindow& window);
};

#endif // PLAYER_H