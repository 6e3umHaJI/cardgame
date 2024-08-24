#ifndef SERVER_H
#define SERVER_H
#include <vector>
#include <string>
#include <memory>
#include <SFML/Network.hpp>
#include "Player.h"
#include "Lobby.h"
#include "Game.h"

class Server
{
public:
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> clientSockets;
    Lobby* lobby;
    int currCount = 1;
    int port;
    bool stopConnection = false;
    

    Server(int port, Lobby& lobby);
    void wait(int count);
    bool sendAll(std::wstring data);

    bool sendTrump(unsigned short face, unsigned short suit);
    bool sendOrder(unsigned short apl, unsigned short vpl);
    bool sendWinner(int id);
    bool sendUpdateCards(int id);

    // Методы для отправки действий клиентов
    bool sendBeatCard(int id, unsigned short face, unsigned short suit);
    bool sendThrowCard(int id, unsigned short face, unsigned short suit);
    bool sendTakeCard(int id, std::vector<Card> cards);
    bool sendPick(int id);
    bool sendPass(int id); 

    // Методы для отправки действий сервера
    bool sendBeatCard(unsigned short face, unsigned short suit);
    bool sendThrowCard(unsigned short face, unsigned short suit);
    bool sendTakeCard(int count);
    bool sendPick();
    bool sendPass();

    bool receive(int id, Game *game);

    void closeConnection();
};

#endif // SERVER_H