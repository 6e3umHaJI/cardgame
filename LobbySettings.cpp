#include "LobbySettings.h"
#include "globals.h"

LobbySettings::LobbySettings()
{
	int width = 450;
	int height = 230;

	shape.setSize(sf::Vector2f(width, height));
	shape.setFillColor(sf::Color(51, 133, 24));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(3.0f);

	float xPos = (globals::screenWidth - width) / 2;
	float yPos = (globals::screenHeight - height) / 2 - 100;
	shape.setPosition(xPos, yPos);

	message.setFont(globals::font);
	message.setString(L"Выберите количество игроков");
	message.setCharacterSize(22);
	message.setFillColor(sf::Color::Black);

	sf::FloatRect textRect = message.getLocalBounds();
	message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	message.setPosition(xPos + width / 2.0f, yPos + 40);

	std::vector<Button> buttons;
	buttons.push_back(Button(L"2", sf::Vector2u(xPos + 55, yPos + 70), sf::Vector2u(70, 70)));
	buttons.push_back(Button(L"3", sf::Vector2u(buttons.back().shape.getPosition().x + 90, buttons.back().shape.getPosition().y), sf::Vector2u(70, 70)));
	buttons.push_back(Button(L"4", sf::Vector2u(buttons.back().shape.getPosition().x + 90, buttons.back().shape.getPosition().y), sf::Vector2u(70, 70)));
	buttons.push_back(Button(L"5", sf::Vector2u(buttons.back().shape.getPosition().x + 90, buttons.back().shape.getPosition().y), sf::Vector2u(70, 70)));
	menu = Menu(buttons);

	buttonCancel.setPosition(sf::Vector2u(xPos + 50, yPos + height - 70));
	buttonCreate.setPosition(sf::Vector2u(xPos + width - 200, yPos + height - 70));
};

void  LobbySettings::reset()
{
	menu.reset();
	menu.buttons.front().activate();
	menu.activeButtonIndex = 0;
	buttonCancel.deactivate();
	buttonCreate.deactivate();
};

void  LobbySettings::draw(sf::RenderWindow& window)
{
	window.draw(shape);
	window.draw(message);
	menu.draw(window);
	buttonCancel.draw(window);
	buttonCreate.draw(window);
};