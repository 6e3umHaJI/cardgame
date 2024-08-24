#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include <SFML/Graphics.hpp>
#include "globals.h"
#include "Button.h"
#include "Menu.h"
#include "Logo.h"
#include "PopupWindow.h"
#include "Notification.h"
#include "LobbySettings.h"
#include "Lobby.h"
#include "Server.h"
#include "Client.h"
#include <thread>

enum State {
    MENU,
    POPUPWINDOWCONNECT,
    POPUPWINDOWNICKNAME,
    LOBBYSETTINGS,
    LOBBY,
    GAME,
    NOTIFICATION
};

class StateManager
{
public:
    std::vector<State> states;
    sf::Sprite *background;
    Logo *logo;
    Menu *menu;
    Button *buttonFullScreen;
    PopupWindow *popupWindow;
    Notification* notification;
    LobbySettings* lobbySettings;
    Lobby* lobby;
    Game* game;
    Server* server;
    Client* client;
    sf::Text nickname;
    std::thread* thread;

    StateManager();
    void addState(State state);
    void removeState();
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void updateLobby();
    void updateGame();
    void connectPlayers(Server* server);
    void draw(sf::RenderWindow& window);
};

#endif // STATEMANAGER_H
