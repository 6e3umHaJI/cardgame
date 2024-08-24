#ifndef GLOBALS_HPP
#define GLOBALS_HPP
#include <SFML/Graphics.hpp>

namespace globals {
    extern const float PI;
    extern const int screenWidth;
    extern const int screenHeight;
    extern const int frameLimit;
    extern const int maxNicknameLength;
    extern const int maxLocalAddressLength;
    extern int count;
    extern int port;
    extern sf::Texture textBackground, textEdit, textCopy, textUser, textFull, textLogo, textBack, textCards, textExit;
    extern sf::Font font;
    void loadTextures(); 
    void loadFont();
}
#endif // GLOBALS_HPP