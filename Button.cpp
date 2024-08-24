#include "Button.h"

Button::Button(std::wstring wstr, sf::Vector2u position, sf::Vector2u size, sf::Color colorDefault, sf::Color colorHovered, sf::Color colorActive)
{
	shape.setPosition(position.x, position.y);
	shape.setSize(sf::Vector2f(size));

	this->colorDefault = colorDefault;
	this->colorHovered = colorHovered;
	this->colorActive = colorActive;
	shape.setFillColor(colorDefault);

	text.setFont(globals::font);
	text.setString(wstr);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::Black);

	sf::FloatRect textBounds = text.getGlobalBounds();

	float x = position.x + (shape.getGlobalBounds().width - textBounds.width) / 2;
	float y = position.y + (shape.getGlobalBounds().height - textBounds.height) / 2 - 3;

	text.setPosition((int)x, (int)y);
};

Button::Button(sf::Texture &texture, sf::Vector2u position, sf::Vector2u size, sf::Color colorDefault, sf::Color colorHovered, sf::Color colorActive)
{
	shape.setSize(sf::Vector2f(size));
	sprite.setTexture(texture);
	sprite.setScale(size.x / sprite.getGlobalBounds().width, size.y / sprite.getGlobalBounds().height);
	sprite.setPosition(position.x, position.y);
	shape.setPosition(position.x, position.y);

	this->colorDefault = colorDefault;
	this->colorHovered = colorHovered;
	this->colorActive = colorActive;
	shape.setFillColor(colorDefault);
};

void Button::setPosition(sf::Vector2u position)
{
	shape.setPosition(sf::Vector2f(position));
	sprite.setPosition(sf::Vector2f(position));

	sf::FloatRect textBounds = text.getGlobalBounds();

	float x = position.x + (shape.getGlobalBounds().width - textBounds.width) / 2;
	float y = position.y + (shape.getGlobalBounds().height - textBounds.height) / 2 - 3;

	text.setPosition((int)x, (int)y);

};

bool Button::isActive(sf::Vector2u cursorPosition)
{
	if (blocked)
		return false;
	if (active && shape.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
	{
		deactivate();
		return false;
	}
	if (hovered || shape.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
	{
		activate();
			return true;
	}
	return false;
};

bool Button::isHovered(sf::Vector2u cursorPosition)
{
	if (blocked)
		return false;
	if (!active && shape.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
	{
		hovered = true;
		shape.setFillColor(colorHovered);
		return true;
	}	
	else if (hovered)
	{
		hovered = false;
		shape.setFillColor(colorDefault);
	}
	return false;
};

void Button::updateText(std::wstring newText)
{

	text.setString(newText);

	sf::FloatRect textBounds = text.getGlobalBounds();

	float x = shape.getPosition().x + (shape.getGlobalBounds().width - textBounds.width) / 2;
	float y = shape.getPosition().y + (shape.getGlobalBounds().height - textBounds.height) / 2 - 3;

	text.setPosition((int)x, (int)y);
}

void Button::block()
{
	shape.setFillColor(colorBlocked);
	active = false;
	hovered = false;
	blocked = true;
};

void Button::activate()
{
	shape.setFillColor(colorActive);
	active = true;
	hovered = false;
};

void Button::deactivate()
{
	shape.setFillColor(colorDefault);
	active = false;
	hovered = false;
	blocked = false;
};

void Button::draw(sf::RenderWindow& window)
{
	window.draw(shape);
	window.draw(sprite);
	if(!text.getString().isEmpty())
		window.draw(text);
};