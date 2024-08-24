#include "Client.h"
#include "globals.h"

Client::Client(std::wstring nickname)
{
	this->nickname = nickname;
};

bool Client::connect(std::string localAddress, std::wstring& error)
{
	sf::Socket::Status status;
	sf::IpAddress ipAddress(localAddress);
	if (ipAddress == sf::IpAddress::getLocalAddress())
		status = socket.connect("127.0.0.1", globals::port, sf::seconds(5)); // local host
	else
		status = socket.connect(ipAddress, globals::port, sf::seconds(5)); // local address

	if (status != sf::Socket::Done) {
		error = L"Ошибка: не удалось\nподключиться к серверу";
		return false;
	}
	socket.setBlocking(false);
	sf::Packet packet;
	packet << nickname;

	if (socket.send(packet) != sf::Socket::Done)
	{
		error = L"Ошибка: не удалось\nподключиться к серверу";
		return false;
	}
	return true;
};

bool Client::receive(Lobby* lobby)
{
	sf::Packet packet;
	if (socket.receive(packet) == sf::Socket::Disconnected)
		return false;
	std::wstring data;
	packet >> data;
	if (data.find(L"delete") != std::wstring::npos)
	{
		// Определение начала числа после "delete"
		size_t numStart = data.find(L"delete") + 7; // 7 - длина слова "delete" и пробела после него

		// Определение конца числа
		size_t numEnd = data.find(L" ", numStart);

		// Извлечение подстроки с числом
		std::wstring numStr = data.substr(numStart, numEnd - numStart);

		// Преобразование строки в число
		int num = std::stoi(numStr);
		if (id > num)
			id--;
		lobby->deletePlayer(num);
	}
	else if (data.find(L"add") != std::wstring::npos)
	{
		// Extract the number after "add" 
		size_t addPos = data.find(L"add");
		size_t spacePos = data.find(L" ", addPos + 4);
		// Skip "add" and find next space 
		std::wstring numberStr = data.substr(addPos + 4, spacePos - addPos - 4);
		// Extract the number as string 
		int number = std::stoi(numberStr);
		// Convert the number to integer

		// Extract usernames after the number
		size_t pos = spacePos + 1; // Move to the character after the space
		for (int i = 0; i < number; ++i)
		{
			size_t nextSpace = data.find(L" ", pos); // Find the next space
			std::wstring username = data.substr(pos, nextSpace - pos); // Extract the username
			lobby->addPlayer(username);
			// Process the username
			pos = nextSpace + 1; // Move to the character after the next space
		}
		if(id == -1)
			id = lobby->connectedPlayers.size();
	}
	else if (data.find(L"start") != std::wstring::npos)
	{
		lobby->startGame = true;
	}
	else if (data.find(L"disconnect") != std::wstring::npos)
	{
		closeSocket();
		return false;
	}
	return true;
};

bool Client::receive(Game* game)
{
	sf::Packet packet;
	if (socket.receive(packet) == sf::Socket::Disconnected)
		return false;
	std::wstring data;
	packet >> data;
	if (data.find(L"take") != std::wstring::npos)
	{
		std::wstring idStr, countStr;
		size_t start = data.find(L"take ") + 5; // Находим позицию начала чисел после "take "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
		idStr = data.substr(start, space1 - start);
		int id = std::stoi(idStr);

		size_t countSpace = data.find(L" ", space1 + 1); // Находим позицию пробела после count
		countStr = data.substr(space1 + 1, countSpace - space1 - 1);
		int count = std::stoi(countStr);

		std::vector<Card> cards = game->my.take(count);

		if (id == this->id)
		{
			for (int i = 0; i < count; i++)
			{
				std::wstring faceStr, suitStr;
				size_t space2 = countSpace;

				size_t space3 = data.find(L" ", space2 + 1); // Находим позицию пробела после face
				faceStr = data.substr(space2 + 1, space3 - space2 - 1);

				size_t space4 = data.find(L" ", space3 + 1); // Находим позицию пробела после suit
				suitStr = data.substr(space3 + 1, space4 - space3 - 1);

				cards[i].face = std::stoi(faceStr);
				cards[i].suit = std::stoi(suitStr);

				countSpace = space4;
			}
			game->addCard(cards);
		}
		else
		{
			game->addCard(id, count);
		}

		game->updateCardCount();
	}
	else if (data.find(L"throw") != std::wstring::npos)
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
		size_t start = data.find(L"pick ") + 5; // Находим позицию начала чисел после "beat "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
		idStr = data.substr(start, space1 - start);

		int id = std::stoi(idStr);

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
		size_t start = data.find(L"pass ") + 5; // Находим позицию начала чисел после "beat "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
		idStr = data.substr(start, space1 - start);

		int id = std::stoi(idStr);

		if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size() && (game->vpl + 1) % game->players.size() == game->apl ||
			(game->currPlayer + 1) % game->players.size() == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если все игроки походили
		{
			if (game->isPicked)
				game->pickCardsFromTable(game->players[game->vpl]->id);
			else
				game->deleteCardsFromTable();
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
		closeSocket();
		return false;
	}
	else if (data.find(L"winner") != std::wstring::npos)
	{
		std::wstring idStr;
		size_t start = data.find(L"winner ") + 7; // Находим позицию начала чисел после "winner "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
		idStr = data.substr(start, space1 - start);

		int id = std::stoi(idStr);
		game->addWinner(id);
		if (game->players.size() == 1)
		{
			game->showGameResult();
		}
	}
	else if (data.find(L"update") != std::wstring::npos)
	{
		std::wstring idStr;
		size_t start = data.find(L"update ") + 7; // Находим позицию начала чисел после "update "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после id
		idStr = data.substr(start, space1 - start);

		int id = std::stoi(idStr);
		if (game->id == id)
			game->updateHandPosition();
		else
			game->updateHandPosition(id);
	}
	else if (data.find(L"order") != std::wstring::npos)
	{
		std::wstring aplStr, vplStr;
		size_t start = data.find(L"order ") + 6; // Находим позицию начала чисел после "order "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после apl
		aplStr = data.substr(start, space1 - start);

		size_t space2 = data.find(L" ", space1 + 1); // Находим позицию пробела после vpl
		vplStr = data.substr(space1 + 1, space2 - space1 - 1);

		int apl = std::stoi(aplStr);
		int vpl = std::stoi(vplStr);
		game->setOrder(apl, vpl);
	}
	else if (data.find(L"trump") != std::wstring::npos)
	{
		std::wstring faceStr, suitStr;
		size_t start = data.find(L"trump ") + 6; // Находим позицию начала чисел после "trump "

		size_t space1 = data.find(L" ", start); // Находим позицию первого пробела после face
		faceStr = data.substr(start, space1 - start);

		size_t space2 = data.find(L" ", space1 + 1); // Находим позицию пробела после suit
		suitStr = data.substr(space1 + 1, space2 - space1 - 1);

		unsigned short face = std::stoi(faceStr);
		unsigned short suit = std::stoi(suitStr);
		game->setTrump(face, suit);
	}
	return true;
};

bool Client::send(std::wstring data)
{
	sf::Packet packet;
	packet << data;
	if (socket.send(packet) == sf::Socket::Disconnected)
		return false;
	return true;
};

bool Client::sendBeatCard(unsigned short face, unsigned short suit)
{
	std::wstring data = L"beat " + std::to_wstring(id) + L" " + std::to_wstring(face) + L" " + std::to_wstring(suit);
	return send(data);
}

bool Client::sendThrowCard(unsigned short face, unsigned short suit)
{
	std::wstring data = L"throw " + std::to_wstring(id) + L" " + std::to_wstring(face) + L" " + std::to_wstring(suit);
	return send(data);
}

bool Client::sendPick()
{
	std::wstring data = L"pick " + std::to_wstring(id);
	return send(data);
}

bool Client::sendPass()
{
	std::wstring data = L"pass " + std::to_wstring(id);
	return send(data);
}

void Client::closeSocket()
{
	socket.disconnect();
};