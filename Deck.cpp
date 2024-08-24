#include "Deck.h"
#include <time.h>

Deck::Deck() {}

Deck::Deck(int faces, int suits)
{
    int count = faces * suits;
    stack = new std::vector<Card>(count);
    for (int i = 0; i < count; i++)
    {
        stack->at(i) = Card(i % faces, i / faces);
    }
    size = count;
}

void Deck::shuffle()
{
    srand(time(0));
    int size = stack->size();
    for (int i = 0; i < size - 1; i++)
    {
        std::swap(stack->at(i), stack->at(((rand() % (size - i)) + i)));

    }
}

Card Deck::take()
{
    if (size != 0)
    {
        Card temp = stack->back();
        stack->pop_back();
        --size;
        return temp;
    }
    else
    {
        return Card();
    }

}

std::vector<Card> Deck::take(int i)
{
    std::vector<Card> temp(i);
    for (int j = 0; j < i; j++)
    {
        temp[j] = this->take();
    }
    return temp;
}

int Deck::add(Card a)
{
    stack->insert(stack->begin(), a);
    ++this->size;
    return 0;
}