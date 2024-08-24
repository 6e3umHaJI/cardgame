#ifndef GAME_H
#define GAME_H
#include <SFML/Graphics.hpp>
#include "Deck.h"
#include "Player.h"
#include "Pair.h"
#include "Button.h"
#include "Lobby.h"

using namespace std;

enum GameState {
    WAIT,
    BEAT,
    DEFENSE,
    TOSS
};

class Game
{
public:
    GameState state;
    std::vector<Player*> players;
    std::vector<Player*> winners;
    int id;
    Deck my;
    int trump;
    Card tcard, bcard;

    int apl, vpl;
    int currPlayer;
    bool isAplToss = false;
    bool isPicked = false;
    bool isDealCardsToPlayers = false;
    int leftCountToToss;

    int selectedCardIndex = -1;
    std::vector<Pair*> pairs;
    int handFieldWidth = 400;
    Button buttonExit = Button(globals::textExit, sf::Vector2u(20, 20), sf::Vector2u(50, 50)), buttonAction = Button(L"", sf::Vector2u(0, 0), sf::Vector2u(200, 50));
    bool isShowButtonAction = false;
    sf::RectangleShape table;
    sf::Text cardCount, clue;

    sf::RectangleShape shape;
    sf::Text result;

    Game(std::vector<ConnectedPlayer> pl, int id);
    int findIndex(int id);
    void addWinner(int id);
    void showGameResult();
    void setOrder(int apl, int vpl);
    void setTrump(unsigned short face, unsigned short suit);
    void setState(GameState state);
    void addCard(int id, int count);
    void addCard(std::vector<Card> cards);
    void deleteCard(int id);
    void deleteCard(unsigned short face, unsigned short suit);
    void updateHandPosition(int id);
    void updateHandPosition();
    void selectCard(sf::Vector2u cursorPosition);
    void beatCard(int id, unsigned short face, unsigned short suit);
    void throwCard(int id, unsigned short face, unsigned short suit);
    void pickCardsFromTable(int id);
    void deleteCardsFromTable();
    void updateClue();
    void updateCardCount();
    void draw(sf::RenderWindow& window);
    ~Game();
};

#endif // GAME_H