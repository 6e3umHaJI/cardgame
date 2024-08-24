#include "StateManager.h"
#include <SFML/Network.hpp>
#include <regex>

StateManager::StateManager()
{
    background = new sf::Sprite(globals::textBackground);

    logo = new Logo(14, 270, 0.1f);

    std::vector<Button> buttons;
    buttons.push_back(Button(L"Создать лобби", sf::Vector2u(0, 850)));
    buttons.push_back(Button(L"Присоединиться"));
    buttons.push_back(Button(L"Выйти"));
    menu = new Menu(buttons, 30);
    menu->buttons.push_back(Button(globals::textEdit, sf::Vector2u(20, 20), sf::Vector2u(50, 50), sf::Color(0, 0, 0, 0)));

    buttonFullScreen = new Button(globals::textFull, sf::Vector2u(globals::screenWidth - 70, 20), sf::Vector2u(50, 50));

    popupWindow = new PopupWindow(L"");
    notification = new Notification(L"");

    lobbySettings = new LobbySettings;
    lobby = new Lobby;

    nickname.setFont(globals::font);
    nickname.setString(L"аноним");
    nickname.setCharacterSize(36);
    nickname.setFillColor(sf::Color(20, 20, 20));
    nickname.setStyle(sf::Text::Underlined);
    nickname.setPosition(20, 21);
    float textWidth = nickname.getGlobalBounds().width;
    this->menu->buttons.back().setPosition(sf::Vector2u(30 + textWidth, 20));
};

void StateManager::addState(State state)
{
	states.push_back(state);
};

void StateManager::removeState()
{
	states.pop_back();
};

void StateManager::handleEvent(sf::Event& event, sf::RenderWindow& window)
{
	switch (states.back())
	{
	case State::MENU:
    {
        if (event.type == sf::Event::MouseMoved)
        {
            menu->isButtonHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (menu->isButtonActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) != -1)
            {
                switch (menu->activeButtonIndex)
                {
                case 0:
                    lobbySettings->reset();
                    addState(State::LOBBYSETTINGS);
                    break;
                case 1:
                    popupWindow->reset();
                    popupWindow->updateMessage(L"Введите локальный IP-адрес сервера");
                    addState(State::POPUPWINDOWCONNECT);
                    break;
                case 2:
                    window.close();
                    break;
                case 3:
                    popupWindow->reset();
                    popupWindow->updateMessage(L"Введите новый никнейм");
                    addState(State::POPUPWINDOWNICKNAME);
                    break;
                }
            }
        }
        break;
    }
	case State::POPUPWINDOWCONNECT:
	{
        if (popupWindow->activeTextField && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::V && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            std::wstring inputText = popupWindow->inputText.getString();
            std::wstring clipboardText = sf::Clipboard::getString();
            if (!popupWindow->showCursor && clipboardText.length() + inputText.length() > globals::maxLocalAddressLength)
                clipboardText.resize(globals::maxLocalAddressLength - inputText.length());
            else if (popupWindow->showCursor && clipboardText.length() + inputText.length() > globals::maxLocalAddressLength + 1)
                clipboardText.resize(globals::maxLocalAddressLength + 1 - inputText.length());
            bool isValid = true;
            for (char c : clipboardText)
            {
                if (!isdigit(c) && c != '.')
                {
                    isValid = false;
                    break;
                }
            }
            if (isValid && inputText.length() < globals::maxLocalAddressLength && !popupWindow->showCursor)
            {
                inputText += clipboardText;
                popupWindow->inputText.setString(inputText);
            }
            else if (isValid && inputText.length() < globals::maxLocalAddressLength + 1 && popupWindow->showCursor)
            {
                inputText.pop_back();
                inputText += clipboardText;
                inputText += '|';
                popupWindow->inputText.setString(inputText);
            }
        }
        else if (popupWindow->activeTextField && event.type == sf::Event::TextEntered)
        {
            std::wstring inputText = popupWindow->inputText.getString();
            if (event.text.unicode == 8) // Backspace
            {
                if (!inputText.empty() && !popupWindow->showCursor)
                {
                    inputText.pop_back();
                    popupWindow->inputText.setString(inputText);
                }
                else if (popupWindow->showCursor && inputText.size() > 1)
                {
                    inputText.erase(inputText.size() - 2, 1);
                    popupWindow->inputText.setString(inputText);
                }
            }
            else if (event.text.unicode < 128)
            {
                char c = static_cast<char>(event.text.unicode);
                if (isdigit(c) || c == '.')
                {
                    if (inputText.length() < globals::maxLocalAddressLength && !popupWindow->showCursor)
                    {
                        inputText += c;
                        popupWindow->inputText.setString(inputText);
                    }
                    else if (inputText.length() < globals::maxLocalAddressLength + 1 && popupWindow->showCursor)
                    {
                        inputText.insert(inputText.size() - 1, 1, c);
                        popupWindow->inputText.setString(inputText);
                    }
                }
            }
        }
        if (event.type == sf::Event::MouseMoved)
        {
            if (popupWindow->isHoveredTextField(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                sf::Cursor cursor;
                cursor.loadFromSystem(sf::Cursor::Text);
                window.setMouseCursor(cursor);
            }
            else
            {
                sf::Cursor cursor;
                cursor.loadFromSystem(sf::Cursor::Arrow);
                window.setMouseCursor(cursor);
            }
            popupWindow->buttonOK.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
            popupWindow->buttonCancel.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (popupWindow->buttonOK.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {              
                std::string localAddress = popupWindow->inputText.getString();
                if (popupWindow->showCursor)
                    localAddress.pop_back();
                std::regex localIpRegex("^((127)|(10)|(192)|(172))(\.(?:1?[0-9]{1,2}|2[0-4][0-9]|25[0-5])){3}$");
                if (!std::regex_match(localAddress, localIpRegex))
                {
                    notification->reset();
                    notification->updateMessage(L"Ошибка: некорректный ip-адрес");
                    addState(State::NOTIFICATION);
                    popupWindow->buttonOK.deactivate();
                }
                else
                {
                    client = new Client(nickname.getString());
                    std::wstring error;
                    if (client->connect(localAddress, error))
                    {
                        removeState();
                        removeState();
                        lobby->reset();
                        client->receive(lobby);
                        addState(State::LOBBY);
                       
                    }
                    else
                    {
                        notification->reset();
                        notification->updateMessage(error);
                        addState(State::NOTIFICATION);
                        popupWindow->buttonOK.deactivate();
                        delete client;
                        client = nullptr;
                    }
                }
            }
            else if (popupWindow->buttonCancel.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                menu->reset();
                removeState();
            }
            else if (popupWindow->isActiveTextField(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {

            }
        }
		break;
	}
    case State::POPUPWINDOWNICKNAME:
    {
        if (popupWindow->activeTextField && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::V && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            std::wstring inputText = popupWindow->inputText.getString();
            std::wstring clipboardText = sf::Clipboard::getString();
            if (!popupWindow->showCursor && clipboardText.length() + inputText.length() > globals::maxNicknameLength)
                clipboardText.resize(globals::maxNicknameLength - inputText.length());
            else if (popupWindow->showCursor && clipboardText.length() + inputText.length() > globals::maxNicknameLength + 1) // Учитываем '|' в конце строки
                clipboardText.resize(globals::maxNicknameLength + 1 - inputText.length());
            bool isValid = true;
            for (wchar_t c : clipboardText)
            {
                if (!((c >= 48 && c <= 57) ||
                    (c >= 65 && c <= 90) ||
                    (c >= 97 && c <= 122) ||
                    (c >= 1040 && c <= 1103)))
                {
                    isValid = false;
                    break;
                }
            }
            if (isValid && inputText.length() < globals::maxNicknameLength && !popupWindow->showCursor)
            {
                inputText += clipboardText;
                popupWindow->inputText.setString(inputText);
            }
            else if (isValid && inputText.length() < globals::maxNicknameLength + 1 && popupWindow->showCursor) // Учитываем '|' в конце строки
            {
                inputText.pop_back();
                inputText += clipboardText;
                inputText += '|';
                popupWindow->inputText.setString(inputText);
            }
        }
        else if (event.type == sf::Event::TextEntered && popupWindow->activeTextField)
        {
            std::wstring inputText = popupWindow->inputText.getString();
            if (event.text.unicode == 8) // Backspace
            {
                if (!inputText.empty() && !popupWindow->showCursor)
                {
                    inputText.pop_back();
                    popupWindow->inputText.setString(inputText);
                }
                else if (popupWindow->showCursor && inputText.size() > 1)
                {
                    inputText.erase(inputText.size() - 2, 1);
                    popupWindow->inputText.setString(inputText);
                }
            }
            else if ((event.text.unicode >= 48 && event.text.unicode <= 57) || // цифры
                (event.text.unicode >= 65 && event.text.unicode <= 90) || // заглавные латинские буквы
                (event.text.unicode >= 97 && event.text.unicode <= 122) || // маленькие латинские буквы
                (event.text.unicode >= 1040 && event.text.unicode <= 1103)) // русские буквы
            {
                wchar_t c = static_cast<wchar_t>(event.text.unicode);
                if (inputText.length() < globals::maxNicknameLength && !popupWindow->showCursor)
                {
                    inputText += c;
                    popupWindow->inputText.setString(inputText);
                }
                else if (inputText.length() < globals::maxNicknameLength + 1 && popupWindow->showCursor)
                {
                    inputText.insert(inputText.size() - 1, 1, c);
                    popupWindow->inputText.setString(inputText);
                }
            }
        }
        if (event.type == sf::Event::MouseMoved)
        {
            if (popupWindow->isHoveredTextField(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                sf::Cursor cursor;
                cursor.loadFromSystem(sf::Cursor::Text);
                window.setMouseCursor(cursor);
            }
            else
            {
                sf::Cursor cursor;
                cursor.loadFromSystem(sf::Cursor::Arrow);
                window.setMouseCursor(cursor);
            }
            popupWindow->buttonOK.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
            popupWindow->buttonCancel.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (popupWindow->buttonOK.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                std::wstring str = popupWindow->inputText.getString();
                if (!str.empty() && str.back() == '|')
                    str.pop_back();
                if (!str.empty())
                {
                    nickname.setString(str);
                    float textWidth = nickname.getGlobalBounds().width;
                    this->menu->buttons.back().setPosition(sf::Vector2u(30 + textWidth, 20));
                }
                menu->reset();
                removeState();
            }
            else if (popupWindow->buttonCancel.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                menu->reset();
                removeState();
            }
            else if (popupWindow->isActiveTextField(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {

            }
        }
        break;
    }
	case State::LOBBYSETTINGS:
	{
        if (event.type == sf::Event::MouseMoved)
        {
            if (lobbySettings->buttonCancel.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
            else if (lobbySettings->buttonCreate.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
            else if (lobbySettings->menu.isButtonHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (lobbySettings->buttonCancel.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                menu->reset();
                removeState();
            }
            else if (lobbySettings->buttonCreate.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                std::string localAddress = sf::IpAddress::getLocalAddress().toString();
                if (localAddress.find("192.168.") == 0 || localAddress.find("10.") == 0 || localAddress.find("172.") == 0 || localAddress.find("127.") == 0)
                {
                    globals::count = lobbySettings->menu.activeButtonIndex + 2;

                    removeState();
                    removeState();
                    lobby->reset();
                    lobby->addPlayer(nickname.getString());
                    lobby->isHost = true; // ОБЯЗАТЕЛЬНО, ЧТОБЫ АДРЕС ОТОБРАЖАЛСЯ!!!
                    lobby->updateLocalAddress(localAddress);
                    server = new Server(globals::port, *lobby);

                    // Создание отдельного потока и запуск функции для подключения клиентов к серверу(или игроков в лобби)
                    thread = new std::thread(&StateManager::connectPlayers, this, server);

                    addState(State::LOBBY);
                }
                else
                {
                    notification->reset();
                    notification->updateMessage(L"Ошибка: отсутствует подключение к сети");
                    addState(State::NOTIFICATION);
                    lobbySettings->buttonCreate.deactivate();
                }
            }
            else if (lobbySettings->menu.isButtonActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) > -1);
        }
		break;
	}
	case State::LOBBY:
	{
        if (event.type == sf::Event::MouseMoved)
        {
            if (lobby->buttonExit.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
            else if (lobby->isHost && lobby->buttonCopy.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
            else if (lobby->isHost && lobby->buttonStart.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (lobby->buttonExit.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            { 
                if (lobby->isHost)
                {
                    server->stopConnection = true;
                    server->closeConnection();
                    delete server;
                    server = nullptr;
                }
                else
                {
                    client->closeSocket();
                    delete client;
                    client = nullptr;
                }
                menu->reset();
                removeState();
                addState(State::MENU);
            }
            else if (lobby->isHost && lobby->buttonCopy.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                sf::Clipboard::setString(lobby->localAddress.getString());
            }
            else if (lobby->isHost && lobby->buttonStart.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                server->stopConnection = true;
                thread->join();
                server->sendAll(L"start");
                game = new Game(lobby->connectedPlayers, 0);
                removeState();
                addState(State::GAME);
                server->sendTrump(game->tcard.face, game->tcard.suit);
                int apl = -1;
                int minTrump = 9;
                for (int i = 0; i < game->players.size(); i++)
                {
                    std::vector<Card> cards = game->my.take(6);
                    for (Card card : cards)
                    {
                        if (card.suit == game->trump && card.face < minTrump)
                        {
                            minTrump = card.face;
                            apl = i;
                        }
                    }
                    if (i == 0)
                    {
                        game->addCard(cards);
                        server->sendTakeCard(cards.size());
                    }
                    else
                    {
                        game->addCard(i, cards.size());
                        server->sendTakeCard(i, cards);
                    }

                    if (i == 0)
                        game->updateHandPosition();
                    else
                        game->updateHandPosition(i); 
                    server->sendUpdateCards(i);
                }
                if (apl == -1) // Если у игроков нет козырей
                    apl = 0;
                server->sendOrder(apl, (apl+1) % globals::count);
                game->setOrder(apl, (apl + 1) % globals::count);
                game->updateCardCount();
            }
        }
        if (lobby->isHost && lobby->buttonCopy.active && event.type == sf::Event::MouseButtonReleased)
        {
            lobby->buttonCopy.deactivate();
        }
		break;
	}
	case State::GAME:
	{      
        if (event.type == sf::Event::MouseMoved)
        {
            if (game->state != GameState::WAIT && game->players.size() > 1)
            {
                if(game->state == GameState::DEFENSE || game->players[game->vpl]->hand.size() > 0 || game->isPicked && game->leftCountToToss > 0)
                    game->selectCard(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
                if(game->isShowButtonAction)
                    game->buttonAction.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
            }
            if (game->buttonExit.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (game->state != GameState::WAIT && game->players.size() > 1)
            {
                int id = game->id;
                if (game->selectedCardIndex != -1)
                {
                    int face = game->players[game->currPlayer]->hand[game->selectedCardIndex]->face;
                    int suit = game->players[game->currPlayer]->hand[game->selectedCardIndex]->suit;
                    int prevPlayer = game->currPlayer;
                    switch (game->state)
                    {
                    case GameState::BEAT:
                        game->throwCard(id, face, suit);
                        game->currPlayer = game->vpl;
                        break;
                    case GameState::DEFENSE:
                        game->beatCard(id, face, suit);
                        game->currPlayer = game->apl;
                        break;
                    case GameState::TOSS:
                        game->throwCard(id, face, suit);
                        game->leftCountToToss--;
                        break;
                    }
                    if (server != nullptr)
                    {
                        if (game->state == DEFENSE)
                            server->sendBeatCard(face, suit);
                        else
                            server->sendThrowCard(face, suit);
                    }
                    else if (client != nullptr)
                    {
                        if (game->state == DEFENSE)
                            client->sendBeatCard(face, suit);
                        else
                            client->sendThrowCard(face, suit);
                    }

                    if (game->my.size == 0 && game->players.size() == 2 && game->players[prevPlayer]->hand.size() == 0)
                    {
                        game->addWinner(game->players[prevPlayer]->id);
                        game->showGameResult();
                        game->setState(GameState::WAIT);
                    }

                    if (game->state != GameState::TOSS)
                        game->setState(GameState::WAIT);
                }
                else if(game->isShowButtonAction && game->buttonAction.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
                {
                    if (game->state == DEFENSE)
                    {
                        if (server != nullptr)
                            server->sendPick();
                        else
                            client->sendPick();

                        game->leftCountToToss = game->players[game->currPlayer]->hand.size(); // Определяем количество карт, которое можно подкинуть берущему игроку
                        if (game->leftCountToToss + game->pairs.size() > 12) // Проверяем, чтобы на поле не могло лежать более 12 карт
                            game->leftCountToToss = 12 - game->pairs.size();

                        game->isPicked = true;
                        if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если ходил изначальный игрок
                        {
                            game->currPlayer = (game->currPlayer - 1 + game->players.size()) % game->players.size();
                            game->isAplToss = true;
                        }
                        else // Если изначальный игрок пассовал
                        {
                            game->currPlayer = (game->currPlayer + 1) % game->players.size();
                        }
                        game->updateClue();
                    }
                    else
                    {
                        if (server != nullptr)
                            server->sendPass();
                        else
                            client->sendPass();
                        if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size() && (game->vpl + 1) % game->players.size() == game->apl ||
                            (game->currPlayer + 1) % game->players.size() == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если все игроки походили
                        {
                            if (game->isPicked)
                            {
                                game->pickCardsFromTable(game->players[game->vpl]->id);
                            }
                            else
                                game->deleteCardsFromTable();

                            if (server != nullptr)
                                game->isDealCardsToPlayers = true;
                        }
                        else if (game->apl == game->currPlayer && !game->isAplToss) // Если нападающий игрок пассовал
                        {
                            if (game->apl == (game->vpl - 1 + game->players.size()) % game->players.size()) // Если изначальный нападающий игрок пассовал
                            {
                                game->setOrder((game->vpl + 1) % game->players.size(), game->vpl); // Нападает игрок следующий после vpl
                            }
                            else
                            {
                                game->setOrder((game->apl + 1) % game->players.size(), game->vpl); // Нападает игрок следующий после apl
                            }
                        }
                        else // Если подкидывающий игрок пассовал
                        {
                            if (game->isAplToss) // Если изначальный подкидывающий игрок пассовал
                            {
                                game->currPlayer = (game->vpl + 1) % game->players.size(); // Подкидывает игрок следующий после vpl
                                game->isAplToss = false;
                            }
                            else
                            {
                                game->currPlayer = (game->currPlayer + 1) % game->players.size(); // Подкидывает игрок следующий после currPlayer
                            }
                            game->updateClue();
                        }
                    }
                    game->setState(GameState::WAIT);
                }
            }
            if (game->buttonExit.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                if (server != nullptr)
                {
                    server->closeConnection();
                    delete server;
                    server = nullptr;
                }
                else if (client != nullptr)
                {
                    client->closeSocket();
                    delete client;
                    client = nullptr;
                }
                delete game;
                game = nullptr;
                menu->reset();
                removeState();
                addState(State::MENU);
            }
        }
		break;
	}
	case State::NOTIFICATION:
	{
        if (event.type == sf::Event::MouseMoved)
        {
            notification->buttonOK.isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (notification->buttonOK.isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window)))))
            {
                removeState();
            }
        }
		break;
	}
	}
    if (event.type == sf::Event::MouseMoved)
    {
        buttonFullScreen->isHovered(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
    }
    if (event.type == sf::Event::MouseButtonPressed)
    {
        bool buttonFullScrOldState = buttonFullScreen->active;
        buttonFullScreen->isActive(sf::Vector2u(window.mapPixelToCoords(sf::Mouse::getPosition(window))));
        bool buttonFullScrNewState = buttonFullScreen->active;
        if (!buttonFullScrOldState && buttonFullScrNewState)
        {
            sf::ContextSettings settings;
            settings.antialiasingLevel = 10;
            window.close();
            window.create(sf::VideoMode(globals::screenWidth, globals::screenHeight), "Durak", sf::Style::Fullscreen, settings);
        }
        else if (buttonFullScrOldState && !buttonFullScrNewState)
        {
            sf::ContextSettings settings;
            settings.antialiasingLevel = 10;
            window.close();
            window.create(sf::VideoMode(globals::screenWidth, globals::screenHeight), "Durak", sf::Style::Titlebar | sf::Style::Close, settings);
        }
    }
};

void StateManager::updateLobby()
{
    std::string localAddress = sf::IpAddress::getLocalAddress().toString();
    if (localAddress.find("192.168.") != 0 && localAddress.find("10.") != 0 && localAddress.find("172.") != 0 && localAddress.find("127.") != 0)
    {
        if (lobby->isHost)
        {
            server->stopConnection = true;
            delete server;
            server = nullptr;
        }
        else
        {
            delete client;
            client = nullptr;
        }

        menu->reset();
        removeState();
        addState(State::MENU);
        notification->reset();
        notification->updateMessage(L"Ошибка: отсутствует подключение к сети");
        addState(State::NOTIFICATION);
        lobbySettings->buttonCreate.deactivate();
    }
    if (!lobby->isHost)
    {
        if (!client->receive(lobby))
        {
            delete client;
            client = nullptr;
            menu->reset();
            removeState();
            addState(State::MENU);
            notification->reset();
            notification->updateMessage(L"Ошибка: сервер закрыл соединение");
            addState(State::NOTIFICATION);
            lobbySettings->buttonCreate.deactivate();
        }
        else if (lobby->startGame)
        {
            globals::count = lobby->connectedPlayers.size();
            game = new Game(lobby->connectedPlayers, client->id);
            removeState();
            addState(State::GAME);
        }
    }
};

void StateManager::updateGame()
{
    if (game->state == GameState::WAIT && game->players.size() > 1)
    {
        if (server != nullptr)
        {
            if(game->isDealCardsToPlayers)
            {
                game->isDealCardsToPlayers = false;
                int vpl = game->vpl;
                int count = game->players.size();
                for (int i = 0; i < count; i++) // раздаем карты игрокам и определяем победителей
                {
                    if (game->players.size() == 1)
                    {
                        break;
                    }
                    int index;
                    if (i == 0)
                        index = (vpl - 1 + game->players.size()) % game->players.size();
                    else if (i != game->players.size() - 1)
                        index = (vpl + i + game->players.size()) % game->players.size();
                    else
                        index = vpl;

                    int cardAmount = game->players[index]->hand.size();

                    if (game->my.size == 0 && cardAmount == 0)
                    {
                        if (index < vpl)
                            vpl--;
                        server->sendWinner(game->players[index]->id);
                        game->addWinner(game->players[index]->id);
                        continue;
                    }

                    int count = 6 - cardAmount;
                    if (game->my.size > 0 && count > 0)
                    {
                        if (game->my.size < count)
                            count = game->my.size;
                        std::vector<Card> cards = game->my.take(count);
                        if (game->players[index]->id == 0)
                        {
                            game->addCard(cards);
                            server->sendTakeCard(cards.size());
                        }
                        else
                        {
                            game->addCard(game->players[index]->id, cards.size());
                            server->sendTakeCard(game->players[index]->id, cards);
                        }
                    }

                    if (game->players[index]->id == 0)
                        game->updateHandPosition();
                    else
                        game->updateHandPosition(game->players[index]->id);
                    server->sendUpdateCards(game->players[index]->id);
                }

                if (game->players.size() == 1)
                {
                    game->showGameResult();
                    return;
                }

                if (game->isPicked)
                {
                    server->sendOrder((vpl + 1) % game->players.size(), (vpl + 2) % game->players.size());
                    game->setOrder((vpl + 1) % game->players.size(), (vpl + 2) % game->players.size());
                }
                else
                {
                    server->sendOrder((vpl + game->players.size()) % game->players.size(), (vpl + 1) % game->players.size());
                    game->setOrder((vpl + game->players.size()) % game->players.size(), (vpl + 1) % game->players.size());
                }
                game->updateCardCount();
            }
            else
                if (!server->receive(game->players[game->currPlayer]->id - 1, game))
                {
                    server->closeConnection();
                    delete server;
                    server = nullptr;
                    delete game;
                    game = nullptr;
                    menu->reset();
                    removeState();
                    addState(State::MENU);
                    notification->reset();
                    notification->updateMessage(L"Ошибка: игрок отключился");
                    addState(State::NOTIFICATION);
                }
        }
        else if (client != nullptr)
        {
            if (!client->receive(game))
            {
                client->closeSocket();
                delete client;
                client = nullptr;
                delete game;
                game = nullptr;
                menu->reset();
                removeState();
                addState(State::MENU);
                notification->reset();
                notification->updateMessage(L"Ошибка: игрок отключился");
                addState(State::NOTIFICATION);
            }
        }
    }
}

// Функция, которая будет запускаться в отдельном потоке
void StateManager::connectPlayers(Server* server) {
    server->wait(globals::count);
}

void StateManager::draw(sf::RenderWindow& window)
{
    window.draw(*background);
    buttonFullScreen->draw(window);
    for (auto state : states)
    {
        switch (state)
        {
        case MENU:
        {
            logo->update();
            logo->draw(window);
            menu->draw(window);
            window.draw(nickname);
            break;
        }
        case POPUPWINDOWCONNECT:
        {
            popupWindow->updateBlinkingCursor();
            popupWindow->draw(window);
            break;
        }
        case POPUPWINDOWNICKNAME:
        {
            popupWindow->updateBlinkingCursor();
            popupWindow->draw(window);
            break;
        }
        case LOBBYSETTINGS:
        {
            lobbySettings->draw(window);
            break;
        }
        case LOBBY:
        {
            updateLobby();
            lobby->draw(window);
            break;
        }
        case GAME:
        {
            updateGame();
            if(states.back() == State::GAME)
                game->draw(window);
            break;
        }
        case NOTIFICATION:
        {
            notification->draw(window);
            break;
        }
        }
    }
};