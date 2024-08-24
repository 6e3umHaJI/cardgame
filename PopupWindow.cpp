#include "PopupWindow.h"
#include "globals.h"

PopupWindow::PopupWindow(std::wstring messageText)
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
	message.setString(messageText);
	message.setCharacterSize(22);
	message.setFillColor(sf::Color::Black);

	sf::FloatRect textRect = message.getLocalBounds();
	message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	message.setPosition(xPos + width / 2.0f, yPos + 70);

	textField.setSize(sf::Vector2f(width - 40, 40));
	textField.setFillColor(sf::Color::White);
	textField.setOutlineColor(sf::Color::Black);
	textField.setOutlineThickness(2.0f);
	
	textField.setPosition(xPos + 20, yPos + height - 130); 

	inputText.setFont(globals::font);
	inputText.setCharacterSize(20);
	inputText.setFillColor(sf::Color::Black);

	inputText.setPosition(xPos + 30, yPos + height - 120); 

	buttonCancel.setPosition(sf::Vector2u(xPos + 50, yPos + height - 70));
	buttonOK.setPosition(sf::Vector2u(xPos + width - 200, yPos + height - 70));
};

bool PopupWindow::isActiveTextField(sf::Vector2u cursorPosition)
{
	if (activeTextField && !textField.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
	{
		activeTextField = false;
		
		std::wstring str = inputText.getString();
		if (showCursor)
		{
			str.pop_back();
			inputText.setString(str);
		}
		showCursor = false;
		return false;
	}
	if (hoveredTextField || textField.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
	{
		activeTextField = true;
		//showCursor = false;
		clock.restart();
		return true;
	}
	return false;
};

bool PopupWindow::isHoveredTextField(sf::Vector2u cursorPosition)
{
	if (textField.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
	{
		hoveredTextField = true;
		return true;
	}
	else if (hoveredTextField)
	{
		hoveredTextField = false;
	}
	return false;
};

void PopupWindow::updateMessage(std::wstring newText)
{
	message.setString(newText);
	sf::FloatRect textRect = message.getLocalBounds();
	message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
};

void PopupWindow::reset()
{
	inputText.setString("");
	hoveredTextField = false;
	activeTextField = true;
	buttonCancel.deactivate();
	buttonOK.deactivate();
	showCursor = false;
	clock.restart();
};

void PopupWindow::updateBlinkingCursor()
{
	if (activeTextField && clock.getElapsedTime() >= sf::milliseconds(500))
	{
		clock.restart();
		showCursor = !showCursor;
		std::wstring str = inputText.getString();
		if(showCursor)
			str.push_back('|');
		else
			str.pop_back();
		inputText.setString(str);
	}
}

void PopupWindow::draw(sf::RenderWindow& window)
{
	window.draw(shape);
	window.draw(textField);
	window.draw(message);
	window.draw(inputText);
	buttonCancel.draw(window);
	buttonOK.draw(window);
};