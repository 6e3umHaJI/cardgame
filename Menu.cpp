#include "Menu.h"
#include "globals.h"


Menu::Menu()
{

};

Menu::Menu(std::vector<Button> buttons)
{
	this->buttons = buttons;
};

Menu::Menu(std::vector<Button> buttons, int distance)
{
	int width = 0;
	for (Button& button : buttons)
	{
		width += button.shape.getSize().x;
	}
	width += distance * (buttons.size() - 1);

	buttons.front().setPosition(sf::Vector2u((globals::screenWidth - width) / 2, buttons.front().shape.getPosition().y));
	for (int i = 1; i < buttons.size(); i++)
		buttons[i].setPosition(sf::Vector2u(buttons[i - 1].shape.getPosition().x + buttons[i - 1].shape.getSize().x + distance, buttons[i - 1].shape.getPosition().y));

	this->buttons = buttons;
};

int Menu::isButtonActive(sf::Vector2u cursorPosition)
{
	int index = -1;
	int prevActiveButtonIndex = activeButtonIndex;
	activeButtonIndex = -1;
	for (Button& button: buttons)
	{
		index++;
		if (button.isActive(cursorPosition) && activeButtonIndex == -1)
		{ 
			activeButtonIndex = index;
		}
	}
	if (activeButtonIndex == -1)
		activeButtonIndex = prevActiveButtonIndex;
	if (activeButtonIndex != prevActiveButtonIndex && prevActiveButtonIndex != -1)
		buttons[prevActiveButtonIndex].deactivate();
	return activeButtonIndex;
};

int Menu::isButtonHovered(sf::Vector2u cursorPosition)
{
	int index = -1;
	int hoveredButtonIndex = -1;
	for (Button& button : buttons)
	{
		index++;
		if (button.isHovered(cursorPosition) && hoveredButtonIndex == -1)
		{
			hoveredButtonIndex = index;
		}
	}
	return hoveredButtonIndex;
};

void Menu::reset()
{
	activeButtonIndex = -1;
	for (Button& button : buttons)
	{
		if (button.active || button.hovered)
			button.deactivate();
	}
};

void Menu::draw(sf::RenderWindow& window)
{
	for (Button& button : buttons)
		button.draw(window);
};