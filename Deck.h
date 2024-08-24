#ifndef DECK_H
#define DECK_H
#include "Card.h"
#include <vector>

class Deck
{
    std::vector<Card>* stack;
public:
    int size;
    Deck();
    Deck(int faces, int suits);
    void shuffle();
    Card take();
    std::vector<Card> take(int i);
    int add(Card a);
};

#endif // DECK_H