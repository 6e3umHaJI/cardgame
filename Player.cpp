#include "Player.h"

Player::Player(std::wstring nickname, int id)
{
	this->nickname = nickname;
	this->id = id;

	text.setFont(globals::font);
	text.setString(nickname);
	text.setCharacterSize(18);
	text.setFillColor(sf::Color::White);

	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
};

void Player::sort()
{
	std::sort(hand.begin(), hand.end(), [](Card* a, Card* b) { if (a->suit == b->suit) { return a->face < b->face; } return a->suit < b->suit; });
}

void Player::draw(sf::RenderWindow& window)
{
	for (Card* card : hand)
	{
		card->draw(window);
	}
	window.draw(text);
}