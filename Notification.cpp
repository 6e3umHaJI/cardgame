#include "Notification.h"
#include "globals.h"

Notification::Notification(std::wstring messageText)
{
	int width = 440;
	int height = 200;

	shape.setSize(sf::Vector2f(width, height));
	shape.setFillColor(sf::Color(0, 79, 17, 250));

	float xPos = (globals::screenWidth - width) / 2;
	float yPos = (globals::screenHeight - height) / 2 - 100;
	shape.setPosition(xPos, yPos);

	message.setFont(globals::font);
	message.setString(messageText);
	message.setCharacterSize(22);
	message.setFillColor(sf::Color::White);

	sf::FloatRect textRect = message.getLocalBounds();
	message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	message.setPosition(xPos + width / 2.0f, yPos + 80);

	buttonOK.setPosition(sf::Vector2u(xPos + (width - buttonOK.shape.getSize().x) / 2, yPos + height - 70));
};

void Notification::updateMessage(std::wstring newText)
{
	message.setString(newText);
	sf::FloatRect textRect = message.getLocalBounds();
	message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
};

void Notification::reset()
{
	buttonOK.deactivate();
};

void Notification::draw(sf::RenderWindow& window)
{
	window.draw(shape);
	window.draw(message);
	buttonOK.draw(window);
};