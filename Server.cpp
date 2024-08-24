#include "Server.h"
#include <chrono>
#include <thread>

Server::Server(int port, Lobby &lobby)
{
    this->port = port;
	listener.listen(port);
    listener.setBlocking(false);
    this->lobby = &lobby;
};

void Server::wait(int count)
{
    int prevCount = 1;
    while (!stopConnection) {
        prevCount = currCount;
        int index = 0;
        // Check if any players have disconnected
        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            sf::Packet packet;
            if ((*it)->receive(packet) == sf::Socket::Disconnected) {
                // Player disconnected
                delete* it; // Free memory
                it = clientSockets.erase(it);
                lobby->deletePlayer(index + 1);
                sendAll(L"delete " + std::to_wstring(index + 1));
                currCount--;
            }
            else {
                // Player still connected, move to next player
                index++;
                ++it;
            }
        }

        sf::TcpSocket* player = new sf::TcpSocket;
        if (listener.accept(*player) == sf::Socket::Done) {
            if (currCount < count)
            {
                clientSockets.push_back(player);
                clientSockets.back()->setBlocking(false);

                sf::Packet packet;
                std::wstring nickname;
                while (nickname.empty())
                {
                    clientSockets.back()->receive(packet);
                    packet >> nickname;
                }

                std::wstring data = L"add " + std::to_wstring(lobby->connectedPlayers.size());
                for (int i = 0; i < lobby->connectedPlayers.size(); i++)
                    data += L" " + lobby->connectedPlayers[i].text.getString();
                packet.clear();
                packet << data;
                player->send(packet);

                sendAll(L"add 1 " + nickname);
                lobby->addPlayer(nickname);
                currCount++;

                if (currCount == count)
                {
                    lobby->buttonStart.deactivate();
                }
                else if (prevCount == count)
                {
                    lobby->buttonStart.block();
                }
            }
            else
            {
                sf::Packet packet;
                packet << L"disconnect";
                player->send(packet);
                player->disconnect();
                delete player;
            }
        }
        else
            delete player;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

bool Server::sendAll(std::wstring data)
{
    sf::Packet packet;
    packet << data;
    for (sf::TcpSocket* socket : clientSockets)
    {
        if (socket->send(packet) == sf::Socket::Disconnected)
            return false;
    }
    return true;
};

bool Server::sendTrump(unsigned short face, unsigned short suit)
{
    std::wstring data = L"trump " + std::to_wstring(face) + L" " + std::to_wstring(suit);
    return sendAll(data);
}

bool Server::sendOrder(unsigned short apl, unsigned short vpl)
{
    std::wstring data = L"order " + std::to_wstring(apl) + L" " + std::to_wstring(vpl);
    return sendAll(data);
}

bool Server::sendWinner(int id)
{
    std::wstring data = L"winner " + std::to_wstring(id);
    return sendAll(data);
}

bool Server::sendUpdateCards(int id)
{
    std::wstring data = L"update " + std::to_wstring(id);
    return sendAll(data);
}

bool Server::sendBeatCard(int id, unsigned short face, unsigned short suit)
{
    std::wstring data = L"beat " + std::to_wstring(id) + L" " + std::to_wstring(face) + L" " + std::to_wstring(suit);
    sf::Packet packet;
    packet << data;
    for (int i = 0; i < clientSockets.size(); i++)
    {
        if (i == id - 1)
            continue;
        if (clientSockets[i]->send(packet) == sf::Socket::Disconnected)
        {
            return false;
        }
    }
    return true;
}

bool Server::sendThrowCard(int id, unsigned short face, unsigned short suit)
{
    std::wstring data = L"throw " + std::to_wstring(id) + L" " + std::to_wstring(face) + L" " + std::to_wstring(suit);
    sf::Packet packet;
    packet << data;
    for (int i = 0; i < clientSockets.size(); i++)  
    {
        if (i == id - 1)
            continue;
        if (clientSockets[i]->send(packet) == sf::Socket::Disconnected)
        {
            return false;
        }
    }
    return true;
}

bool Server::sendTakeCard(int id, std::vector<Card> cards)
{
    std::wstring data = L"take " + std::to_wstring(id) + L" " + std::to_wstring(cards.size());
    std::wstring dataID = L"take " + std::to_wstring(id) + L" " + std::to_wstring(cards.size());
    for (Card card : cards)
    {
        dataID += L" " + std::to_wstring(card.face) + L" " + std::to_wstring(card.suit);
    }
    sf::Packet packet;
    packet << dataID;
    if (clientSockets[id - 1]->send(packet) == sf::Socket::Disconnected)
    {
        return false;
    }
    packet.clear();
    packet << data;
    for (int i = 0; i < clientSockets.size(); i++)
    {
        if (i == id - 1)
            continue;
        if (clientSockets[i]->send(packet) == sf::Socket::Disconnected)
        {
            return false;
        }
    }
    return true;
}

bool Server::sendPick(int id)
{
    std::wstring data = L"pick " + std::to_wstring(id);
    sf::Packet packet;
    packet << data;
    for (int i = 0; i < clientSockets.size(); i++)
    {
        if (i == id - 1)
            continue;
        if (clientSockets[i]->send(packet) == sf::Socket::Disconnected)
        {
            return false;
        }
    }
    return true;
}

bool Server::sendPass(int id)
{
    std::wstring data = L"pass " + std::to_wstring(id);
    sf::Packet packet;
    packet << data;
    for (int i = 0; i < clientSockets.size(); i++)
    {
        if (i == id - 1)
            continue;
        if (clientSockets[i]->send(packet) == sf::Socket::Disconnected)
        {
            return false;
        }
    }
    return true;
}

bool Server::sendBeatCard(unsigned short face, unsigned short suit)
{
    std::wstring data = L"beat " + std::to_wstring(0) + L" " + std::to_wstring(face) + L" " + std::to_wstring(suit);
    return sendAll(data);
}

bool Server::sendThrowCard(unsigned short face, unsigned short suit)
{
    std::wstring data = L"throw " + std::to_wstring(0) + L" " + std::to_wstring(face) + L" " + std::to_wstring(suit);
    return sendAll(data);
}

bool Server::sendTakeCard(int count)
{
    std::wstring data = L"take " + std::to_wstring(0) + L" " + std::to_wstring(count);
    return sendAll(data);
}

bool Server::sendPick()
{
    std::wstring data = L"pick " + std::to_wstring(0);
    return sendAll(data);
}

bool Server::sendPass()
{
    std::wstring data = L"pass " + std::to_wstring(0);
    return sendAll(data);
}

bool Server::receive(int id, Game *game)
{
    sf::Packet packet;
    if (clientSockets[id]->receive(packet) == sf::Socket::Disconnected)
        return false;
    std::wstring data;
    packet >> data;
    if (data.find(L"throw") != std::wstring::npos)
    {
        std::wstring idStr, faceStr, suitStr;
        size_t start = data.find(L"throw ") + 6; // Находим позицию начала чисел после "throw "

        size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
        idStr = data.substr(start, space1 - start);

        size_t space2 = data.find(L" ", space1 + 1); // Находим позицию пробела после face
        faceStr = data.substr(space1 + 1, space2 - space1 - 1);

        size_t space3 = data.find(L" ", space2 + 1); // Находим позицию пробела после suit
        suitStr = data.substr(space2 + 1, space3 - space2 - 1);

        int id = std::stoi(idStr);
        unsigned short face = std::stoi(faceStr);
        unsigned short suit = std::stoi(suitStr);
        sendThrowCard(id, face, suit);
        game->throwCard(id, face, suit);
        if (game->my.size == 0 && game->players.size() == 2 && game->players[game->currPlayer]->hand.size() == 0)
        {
            game->addWinner(game->players[game->currPlayer]->id);
            game->showGameResult();
        }
        else if (game->currPlayer == game->apl && !game->isPicked) // Если карту не подкидывают, то отбивается vpl
        {
            game->currPlayer = game->vpl;
            //game->currPlayer = (game->currPlayer + 1) % game->players.size();
            if (game->id == game->players[game->vpl]->id)
            {
                game->setState(GameState::DEFENSE);
            }
        }
        else
            game->leftCountToToss--;
    }
    else if (data.find(L"beat") != std::wstring::npos)
    {
        std::wstring idStr, faceStr, suitStr;
        size_t start = data.find(L"beat ") + 5; // Находим позицию начала чисел после "beat "

        size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
        idStr = data.substr(start, space1 - start);

        size_t space2 = data.find(L" ", space1 + 1); // Находим позицию пробела после face
        faceStr = data.substr(space1 + 1, space2 - space1 - 1);

        size_t space3 = data.find(L" ", space2 + 1); // Находим позицию пробела после suit
        suitStr = data.substr(space2 + 1, space3 - space2 - 1);

        int id = std::stoi(idStr);
        unsigned short face = std::stoi(faceStr);
        unsigned short suit = std::stoi(suitStr);
        sendBeatCard(id, face, suit);
        game->beatCard(id, face, suit);

        if (game->my.size == 0 && game->players.size() == 2 && game->players[game->currPlayer]->hand.size() == 0)
        {
            game->addWinner(game->players[game->currPlayer]->id);
            game->showGameResult();
        }
        else
        {
            game->currPlayer = game->apl;
            //game->currPlayer = (game->currPlayer - 1 + game->players.size()) % game->players.size();
            if (game->id == game->players[game->apl]->id)
            {
                game->setState(GameState::BEAT);
            }
        }   
    }
    else if (data.find(L"pick") != std::wstring::npos)
    {
        std::wstring idStr;
        size_t start = data.find(L"pick ") + 5; // Находим позицию начала чисел после "pick "

        size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
        idStr = data.substr(start, space1 - start);
        int id = std::stoi(idStr);
        sendPick(id);

        game->isPicked = true;

        game->leftCountToToss = game->players[game->currPlayer]->hand.size(); // Определяем количество карт, которое можно подкинуть берущему игроку
        if (game->leftCountToToss + game->pairs.size() > 12) // Проверяем, чтобы на поле не могло лежать более 12 карт
            game->leftCountToToss = 12 - game->pairs.size();

        if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если ходил изначальный игрок
        {
            game->currPlayer = (game->currPlayer - 1 + game->players.size()) % game->players.size();
            game->isAplToss = true;
        }
        else // Если изначальный игрок пассовал
        {
            game->currPlayer = (game->currPlayer + 1 + game->players.size()) % game->players.size();
        }

        if (game->id == game->players[game->currPlayer]->id)
        {
            game->setState(GameState::TOSS);
        }
        game->updateClue(); // Обновляем информацию о том, кто ходит/подкидывает и отбивается/берет
    }
    else if (data.find(L"pass") != std::wstring::npos)
    {
        std::wstring idStr;
        size_t start = data.find(L"pass ") + 5; // Находим позицию начала чисел после "pass "

        size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
        idStr = data.substr(start, space1 - start);
        int id = std::stoi(idStr);
        sendPass(id); 

        if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size() && (game->vpl + 1) % game->players.size() == game->apl ||
            (game->currPlayer + 1) % game->players.size() == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если все игроки походили
        {
            if (game->isPicked)
            {
                game->pickCardsFromTable(game->players[game->vpl]->id);
            }
            else
                game->deleteCardsFromTable();

            game->isDealCardsToPlayers = true;
        }
        else if (game->apl == game->currPlayer && !game->isAplToss) // Если нападающий игрок пассовал
        {
            if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если изначальный нападающий игрок пассовал
            {
                game->setOrder((game->vpl + 1) % game->players.size(), game->vpl); // Нападает игрок следующий после vpl
            }
            else
            {
                game->setOrder((game->apl + 1) % game->players.size(), game->vpl); // Нападает игрок следующий после apl
            }
        }
        else // Если подкидывающий игрок пассовал
        {
            if (game->isAplToss) // Если изначальный подкидывающий игрок пассовал
            {
                game->currPlayer = (game->vpl + 1) % game->players.size(); // Подкидывает игрок следующий после vpl
                game->isAplToss = false;
            }
            else
            {
                game->currPlayer = (game->currPlayer + 1) % game->players.size(); // Подкидывает игрок следующий после currPlayer
            }

            if (game->id == game->players[game->currPlayer]->id)
            {
                game->setState(GameState::TOSS);
            }

            game->updateClue();
        }
    }
    else if (data.find(L"disconnect") != std::wstring::npos)
    {
        return false;
    }
    return true;
}

void Server::closeConnection()
{
    sendAll(L"disconnect");
    for (sf::TcpSocket *socket : clientSockets)
    {
        socket->disconnect();
    }
    clientSockets.clear();
    listener.close();
};