#include "Pair.h"
#include "Card.h"

Pair::Pair(Card *first)
{
    this->first = first;
    this->second = nullptr;
}

std::vector<Card*> Pair::contains()
{
    std::vector<Card*> temp;
    temp.push_back(first);
    if (second != nullptr)
    {
        temp.push_back(second);
    }
    return temp;
}

void Pair::draw(sf::RenderWindow& window)
{
    first->draw(window);
    if (second != nullptr)
    {
        second->draw(window);
    }
}