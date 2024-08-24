#ifndef CLIENT_H
#define CLIENT_H
#include "Lobby.h"
#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class Client
{
public:
    sf::TcpSocket socket;
    int id = -1;
    std::wstring nickname;

    Client(std::wstring nickname);
    bool connect(std::string localAddress, std::wstring& error); // Подключение клиента к серверу
    bool receive(Lobby* lobby); // Получение информации по лобби от сервера
    bool receive(Game* game); // Получение информации по игре от сервера
    bool send(std::wstring data); // Отправка data серверу
    bool sendBeatCard(unsigned short face, unsigned short suit); // Отправка карты серверу, которой клиент отбивался
    bool sendThrowCard(unsigned short face, unsigned short suit); // Отправка карты серверу, которой клиент нападал(подкидывал)
    bool sendPick(); // Отправка серверу, что клиент не отбил карты
    bool sendPass(); // Отправка серверу, что клиент закончил нападать(подкидывать)
    void closeSocket(); // Отключение от сервера
};

#endif // CLIENT_H