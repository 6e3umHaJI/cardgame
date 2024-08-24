#ifndef LOBBY_H
#define LOBBY_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Button.h"

class ConnectedPlayer {
public:
    sf::Sprite sprite;
    sf::Text text;

    ConnectedPlayer(std::wstring nickname, sf::Vector2f size);
    void setPosition(sf::Vector2u position);
    void draw(sf::RenderWindow& window);
};

class Lobby
{
public:
    std::vector<ConnectedPlayer> connectedPlayers;
    Button buttonExit = Button(globals::textExit, sf::Vector2u(20, 20), sf::Vector2u(50, 50)), buttonStart = Button(L"Начать игру", sf::Vector2u(0, 0), sf::Vector2u(200, 100));

    sf::Text localAddress;
    Button buttonCopy = Button(globals::textCopy, sf::Vector2u(0, 0), sf::Vector2u(50, 50));
    bool isHost;
    bool startGame = false;
    
    Lobby(bool isHost = false);
    void addPlayer(std::wstring nickname);
    void deletePlayer(int number);
    void updateLocalAddress(std::string newLocalAddress);
    void reset();
    void draw(sf::RenderWindow& window);
};

#endif // LOBBY_H
