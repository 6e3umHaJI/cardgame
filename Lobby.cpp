#include "Lobby.h"
#include "globals.h"

   
ConnectedPlayer::ConnectedPlayer(std::wstring nickname, sf::Vector2f size)
{
	sprite.setTexture(globals::textUser);
	sprite.setScale(size.x / sprite.getGlobalBounds().width, size.y / sprite.getGlobalBounds().height);
	sprite.setPosition(0, 0);

	text.setFont(globals::font);
	text.setString(nickname);
	text.setCharacterSize(18);
	text.setFillColor(sf::Color::White);

	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	text.setPosition(0, 0);
};

void ConnectedPlayer::setPosition(sf::Vector2u position)
{
	sprite.setPosition(sf::Vector2f(position));
	int width = sprite.getGlobalBounds().width;
	int height = sprite.getGlobalBounds().height;
	text.setPosition(position.x + width / 2, position.y + height + 20);
};

void ConnectedPlayer::draw(sf::RenderWindow& window)
{
	window.draw(sprite);
	window.draw(text);
};


Lobby::Lobby(bool isHost)
{
	buttonStart.setPosition(sf::Vector2u((globals::screenWidth - buttonStart.shape.getGlobalBounds().width) / 2, globals::screenHeight - buttonStart.shape.getGlobalBounds().height - 100));

	buttonCopy.setPosition(sf::Vector2u(buttonExit.shape.getPosition().x + buttonExit.shape.getSize().x + 20, 20));

	localAddress.setFont(globals::font);
	localAddress.setCharacterSize(30);
	localAddress.setFillColor(sf::Color::White);
	sf::FloatRect textRect = localAddress.getLocalBounds();
	localAddress.setOrigin(0, textRect.top + textRect.height / 2.0f);
	localAddress.setPosition(buttonCopy.shape.getPosition().x + buttonCopy.shape.getSize().x + 10, buttonCopy.shape.getPosition().y + buttonCopy.shape.getSize().y / 2);

	this->isHost = isHost;
};

void Lobby::addPlayer(std::wstring nickname)
{
	connectedPlayers.push_back(ConnectedPlayer(nickname, sf::Vector2f(250, 250)));

	int playersCount = connectedPlayers.size();
	int width = connectedPlayers.back().sprite.getGlobalBounds().width;
	int distance = (globals::screenWidth - playersCount * width) / (playersCount + 1);

	for (int i = 0; i < playersCount; i++)
	{
		connectedPlayers[i].setPosition(sf::Vector2u(distance * (i + 1) + width * i, globals::screenHeight / 2 - 200));
	}
};

void Lobby::deletePlayer(int number)
{
	connectedPlayers.erase(connectedPlayers.begin() + number);

	int playersCount = connectedPlayers.size();
	int width = connectedPlayers.back().sprite.getGlobalBounds().width;
	int distance = (globals::screenWidth - playersCount * width) / (playersCount + 1);

	for (int i = 0; i < playersCount; i++)
	{
		connectedPlayers[i].setPosition(sf::Vector2u(distance * (i + 1) + width * i, globals::screenHeight / 2 - 200));
	}
};

void Lobby::updateLocalAddress(std::string newLocalAddress)
{
	localAddress.setString(newLocalAddress);
	sf::FloatRect textRect = localAddress.getLocalBounds();
	localAddress.setOrigin(0, textRect.top + textRect.height / 2.0f);
	localAddress.setPosition(buttonCopy.shape.getPosition().x + buttonCopy.shape.getSize().x + 10, buttonCopy.shape.getPosition().y + buttonCopy.shape.getSize().y / 2);
};

void Lobby::reset()
{
	connectedPlayers.clear();
	buttonExit.deactivate();
	buttonCopy.deactivate();
	buttonStart.block();
	isHost = false;
	startGame = false;
};

void Lobby::draw(sf::RenderWindow& window)
{
	for (ConnectedPlayer& player : connectedPlayers)
		player.draw(window);
	buttonExit.draw(window);
	if (isHost)
	{
		buttonStart.draw(window);
		window.draw(localAddress);
		buttonCopy.draw(window);
	}
};