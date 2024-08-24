#include "globals.h"
namespace globals {
    const float PI = 3.1415926;
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    const int frameLimit = 60;
    const int maxNicknameLength = 15;
    const int maxLocalAddressLength = 15;
    int port = 52000;
    int count = 2;
    sf::Texture textBackground, textEdit, textCopy, textUser, textFull, textLogo, textBack, textCards, textExit;
    sf::Font font;
    void loadTextures() {
        textBackground.loadFromFile("images/bg.png");
        textEdit.loadFromFile("images/edit.png");
        textCopy.loadFromFile("images/copy.png");
        textUser.loadFromFile("images/user.png");
        textFull.loadFromFile("images/full.png");
        textLogo.loadFromFile("images/logo.png");
        textBack.loadFromFile("images/back.png");
        textCards.loadFromFile("images/cards.png");
        textExit.loadFromFile("images/exit.png");
    }
    void loadFont()
    {
        font.loadFromFile("fonts/Roboto-Medium.ttf");
    }
}