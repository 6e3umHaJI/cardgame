#ifndef LOGO_H
#define LOGO_H
#include <SFML/Graphics.hpp>
#include <vector>

class Logo
{
public:
    sf::Sprite sprite;
    std::vector<sf::CircleShape> circles;
    float angle = 0;
    int radius;
    float angularSpeed;
    sf::Clock clock;

    Logo(int circleCount, int radius, float angularSpeed);
    void update();
    void draw(sf::RenderWindow& window);
};

#endif // LOGO_H
