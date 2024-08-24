#ifndef PAIR_H
#define PAIR_H
#include "Card.h"
#include <vector>
#include <memory>

class Pair
{
public:
    Card *first;
    Card *second;
    Pair(Card *first);
    std::vector<Card*> contains();
    void draw(sf::RenderWindow& window);
};

#endif // PAIR_H