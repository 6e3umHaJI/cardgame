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
    bool connect(std::string localAddress, std::wstring& error); // ����������� ������� � �������
    bool receive(Lobby* lobby); // ��������� ���������� �� ����� �� �������
    bool receive(Game* game); // ��������� ���������� �� ���� �� �������
    bool send(std::wstring data); // �������� data �������
    bool sendBeatCard(unsigned short face, unsigned short suit); // �������� ����� �������, ������� ������ ���������
    bool sendThrowCard(unsigned short face, unsigned short suit); // �������� ����� �������, ������� ������ �������(����������)
    bool sendPick(); // �������� �������, ��� ������ �� ����� �����
    bool sendPass(); // �������� �������, ��� ������ �������� ��������(�����������)
    void closeSocket(); // ���������� �� �������
};

#endif // CLIENT_H