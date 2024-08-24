#include "Logo.h"
#include "globals.h"

Logo::Logo(int circleCount, int radius, float angularSpeed): radius(radius), angularSpeed(angularSpeed)
{
	sprite.setTexture(globals::textLogo);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	sprite.setPosition(globals::screenWidth / 2, globals::screenHeight / 2 - 100);
	sf::Vector2f rotationPoint(sprite.getPosition());
	for (int i = 0; i < circleCount; i++)
	{
		sf::CircleShape circle(15);
		if (i % 2 == 0)
			circle.setFillColor(sf::Color::Red);
		else
			circle.setFillColor(sf::Color::Black);

		float angle = 2 * globals::PI / circleCount * i; // угол для равномерного распределения кругов
		float x = rotationPoint.x + radius * cos(angle);
		float y = rotationPoint.y + radius * sin(angle);

		circle.setPosition(x, y);
		circle.setOrigin(circle.getLocalBounds().width / 2, circle.getLocalBounds().height / 2);
		circles.push_back(sf::CircleShape(circle));
		clock.restart();
	}
};

void Logo::update()
{
	float time = clock.restart().asSeconds();
	for (auto& circle : circles)
	{
		float angle = std::atan2(circle.getPosition().y - sprite.getPosition().y, circle.getPosition().x - sprite.getPosition().x) + angularSpeed * time;
		float x = sprite.getPosition().x + radius * cos(angle);
		float y = sprite.getPosition().y + radius * sin(angle);

		circle.setPosition(x, y);
	}
};

void Logo::draw(sf::RenderWindow& window)
{
	for (auto& circle : circles)
		window.draw(circle);
	window.draw(sprite);
};