#include "Game.h"
#include "globals.h"

Game::Game(std::vector<ConnectedPlayer> pl, int id)
{
    for (int i = 0; i < pl.size(); i++)
    {
        players.push_back(new Player(std::to_wstring(i+1) + L"." + pl[i].text.getString(), i));
    }
    this->id = id;
    my = Deck(9, 4);
    my.shuffle();
    tcard = my.take();
    trump = tcard.suit;
    my.add(tcard);
    tcard.setFace();
    tcard.setPosition(sf::Vector2u(150, (globals::screenHeight - tcard.sprite.getGlobalBounds().height) / 2));
    bcard.setBack();
    bcard.setPosition(sf::Vector2u(100, (globals::screenHeight - tcard.sprite.getGlobalBounds().height) / 2));
    buttonAction.setPosition(sf::Vector2u((globals::screenWidth - buttonAction.shape.getGlobalBounds().width) / 2, globals::screenHeight - buttonAction.shape.getGlobalBounds().height - 260));
    int handFieldX = (globals::screenWidth - handFieldWidth) / 2;
    players[id]->text.setPosition(handFieldX + handFieldWidth/2, globals::screenHeight - 60);

    int distanceBetweenFields = (globals::screenWidth - handFieldWidth * (globals::count - 1)) / globals::count;
    int index = 0;
    for (int i = 0; i < players.size(); i++)
    {
        int indexPL = (i + id) % globals::count;
        if (indexPL == id)
            continue;
        int handFieldX = distanceBetweenFields * (index + 1) + handFieldWidth * index;
        players[indexPL]->text.setPosition(handFieldX + handFieldWidth / 2, 60);
        index++;
    }

    table.setSize(sf::Vector2f(1000, 400));
    table.setFillColor(sf::Color(0, 79, 17, 250));
    table.setPosition((globals::screenWidth - 1000) / 2, (globals::screenHeight - 400) / 2);

    setState(GameState::WAIT);

    cardCount.setFont(globals::font);
    cardCount.setString(L"36");
    cardCount.setCharacterSize(20);
    cardCount.setFillColor(sf::Color::White);

    sf::FloatRect textRect = cardCount.getLocalBounds();
    cardCount.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    cardCount.setPosition(bcard.sprite.getPosition().x + (50 + tcard.sprite.getGlobalBounds().width)/2, tcard.sprite.getPosition().y + tcard.sprite.getGlobalBounds().height + 20);

    clue.setFont(globals::font);
    clue.setCharacterSize(20);
    clue.setFillColor(sf::Color::White);


    shape.setSize(sf::Vector2f(500, 300));
    shape.setFillColor(sf::Color(255, 224, 56, 200));
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(3);
    shape.setPosition((globals::screenWidth - 500) / 2, (globals::screenHeight - 300) / 2);

    result.setFont(globals::font);
    result.setCharacterSize(20);
    result.setFillColor(sf::Color::White);
}

int Game::findIndex(int id)
{
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->id == id) {
            return i;
        }
    }
}

void Game::addWinner(int id)
{
    int index = findIndex(id);
    winners.push_back(players[index]);
    players.erase(players.begin() + index);
}

void Game::showGameResult()
{
    std::wstring res = L"Победители:\n";
    for (Player* player : winners)
    {
        res += player->nickname + L"\n";
    }
    res += L"\nПроигравший:\n";
    res += players.back()->nickname;

    result.setString(res);
    sf::FloatRect textRect = result.getLocalBounds();
    result.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    result.setPosition(shape.getPosition().x + shape.getGlobalBounds().width/2, shape.getPosition().y + shape.getGlobalBounds().height / 2);
}

void Game::setOrder(int apl, int vpl)
{
    currPlayer = apl;
    this->apl = apl;
    this->vpl = vpl;
    if (id == players[apl]->id)
        setState(GameState::BEAT);
    isAplToss = false;
    isPicked = false;
    updateClue();
}

void Game::setTrump(unsigned short face, unsigned short suit)
{
    trump = suit;
    tcard = Card(face,suit);
    tcard.setFace();
    tcard.setPosition(sf::Vector2u(150, (globals::screenHeight - tcard.sprite.getGlobalBounds().height) / 2));
}

void Game::setState(GameState state)
{
    switch (state)
    {
    case GameState::WAIT:
        isShowButtonAction = false;
        break;
    case GameState::BEAT:
        if (!pairs.empty())
        {
            buttonAction.deactivate();
            isShowButtonAction = true;
            buttonAction.updateText(L"Бито");
        }
        break;
    case GameState::DEFENSE:
        buttonAction.deactivate();
        isShowButtonAction = true;
        buttonAction.updateText(L"Беру");
        break;
    case GameState::TOSS:
        buttonAction.deactivate();
        isShowButtonAction = true;
        buttonAction.updateText(L"Пусть берет");
        break;
    }
    this->state = state;
}

void Game::addCard(int id, int count)
{
    int index = findIndex(id);
    for (int i = 0; i < count; i++)
    {
        players[index]->hand.push_back(new Card(0, 0));
        players[index]->hand.back()->setBack();
        players[index]->hand.back()->setPosition(sf::Vector2u(-1000, 0));
    }
}

void Game::addCard(std::vector<Card> cards)
{
    int index = findIndex(id);
    for (Card card : cards)
    {
        players[index]->hand.push_back(new Card(card.face, card.suit));
        players[index]->hand.back()->setFace();
        players[index]->hand.back()->setPosition(sf::Vector2u(-1000, 0));
    }
}

void Game::deleteCard(int id)
{
    int index = findIndex(id);
    delete players[index]->hand.back();
    players[index]->hand.pop_back();
}

void Game::deleteCard(unsigned short face, unsigned short suit)
{
    int index = findIndex(id);
    for (auto it = players[index]->hand.begin(); it != players[index]->hand.end(); ++it)
    {
        if ((*it)->face == face && (*it)->suit == suit)
        {
            delete *it;
            players[index]->hand.erase(it);
            break;
        }
    }
}

void Game::updateHandPosition(int id)
{
    int indexPl = findIndex(id);
    int index;
    players[indexPl]->sort();
    index = (id - this->id + globals::count) % globals::count;
    int distanceBetweenFields = (globals::screenWidth - handFieldWidth * (globals::count - 1)) / globals::count;
    int handFieldX = distanceBetweenFields * (index) + handFieldWidth * (index - 1);
    int cardsWidth = players[indexPl]->hand.size() * players[indexPl]->hand.back()->size.x;
    if (cardsWidth > handFieldWidth)
    {
        float distanceBetweenCards = (handFieldWidth - players[indexPl]->hand.back()->size.x) / (players[indexPl]->hand.size() - 1);
        int i = 1;
        for (Card* card : players[indexPl]->hand)
        {
            card->setPosition(sf::Vector2u(handFieldX + distanceBetweenCards * (i - 1), 80));
            i++;
        }
    }
    else
    {
        int distanceBetweenCards = (handFieldWidth - players[indexPl]->hand.size() * players[indexPl]->hand.back()->size.x) / (players[indexPl]->hand.size() + 1);
        int i = 1;
        for (Card* card : players[indexPl]->hand)
        {
            card->setPosition(sf::Vector2u(handFieldX + distanceBetweenCards * i + players[indexPl]->hand.back()->size.x * (i - 1), 80));
            i++;
        }
    }
}

void Game::updateHandPosition()
{
    int index = findIndex(id);
    players[index]->sort();
    int handFieldX = (globals::screenWidth - handFieldWidth) / 2;
    int cardsWidth = players[index]->hand.size() * players[index]->hand.back()->size.x;
    if (cardsWidth > handFieldWidth)
    {
        float distanceBetweenCards = (handFieldWidth - players[index]->hand.back()->size.x) / (players[index]->hand.size() - 1);
        int i = 1;
        for (Card* card : players[index]->hand)
        {
            card->setPosition(sf::Vector2u(handFieldX + distanceBetweenCards * (i - 1), globals::screenHeight - 80 - players[index]->hand.back()->size.y));
            i++;
        }
    }
    else
    {
        int distanceBetweenCards = (handFieldWidth - players[index]->hand.size() * players[index]->hand.back()->size.x) / (players[index]->hand.size() + 1);
        int i = 1;
        for (Card* card : players[index]->hand)
        {
            card->setPosition(sf::Vector2u(handFieldX + distanceBetweenCards * i + players[index]->hand.back()->size.x * (i - 1), globals::screenHeight - 80 - players[index]->hand.back()->size.y));
            i++;
        }
    }
}

void Game::selectCard(sf::Vector2u cursorPosition)
{
    int index = findIndex(id);
    int displacement = 30;
    for (int i = players[index]->hand.size() - 1; i >= 0; i--)
    {
        if (selectedCardIndex == i)
        {
            Card selected = *players[index]->hand[i];
            selected.sprite.move(0, displacement);
            
            if (selected.sprite.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
            {
                return;
            }
        }
        if (players[index]->hand[i]->sprite.getGlobalBounds().contains(sf::Vector2f(cursorPosition)))
        {
            if (selectedCardIndex != -1)
                players[index]->hand[selectedCardIndex]->sprite.move(0, displacement);
            selectedCardIndex = i; 
            if (state == GameState::DEFENSE)
            {
                if (!players[index]->hand[selectedCardIndex]->fight(*pairs.back()->first, trump))
                {
                    selectedCardIndex = -1;
                    return;
                }
            }
            else if (!pairs.empty() && (state == GameState::BEAT || state == GameState::TOSS))
            {
                bool isCardPlayable = false;
                for (Pair* pair : pairs)
                {
                    if (pair->first->face == players[index]->hand[selectedCardIndex]->face || pair->second != nullptr && pair->second->face == players[index]->hand[selectedCardIndex]->face)
                    {
                        isCardPlayable = true;
                        break;
                    }
                }
                if (!isCardPlayable)
                {
                    selectedCardIndex = -1;
                    return;
                }
            }
            players[index]->hand[selectedCardIndex]->sprite.move(sf::Vector2f(0, -displacement));
            return;
        }
    }
    if (selectedCardIndex != -1)
    {
        players[index]->hand[selectedCardIndex]->sprite.move(0, displacement);
    }
    selectedCardIndex = -1;
}

void Game::beatCard(int id, unsigned short face, unsigned short suit)
{
    Card* card = new Card(face, suit);
    pairs.back()->second = card;
    pairs.back()->second->setFace();
    if (id == Game::id)
        deleteCard(face, suit);
    else
        deleteCard(id);
    pairs.back()->second->setPosition(sf::Vector2u(pairs.back()->first->sprite.getPosition().x + 20, pairs.back()->first->sprite.getPosition().y + 20));
    selectedCardIndex = -1;
}

void Game::throwCard(int id, unsigned short face, unsigned short suit)
{
    Card* card = new Card(face, suit);
    pairs.push_back(new Pair(card));
    pairs.back()->first->setFace();
    if (id == Game::id)
        deleteCard(face, suit);
    else
        deleteCard(id);
    int distanceX = (table.getSize().x - 6 * pairs.back()->first->size.x) / 7;
    int distanceY = (table.getSize().y - 2 * pairs.back()->first->size.y) / 3;
    if(pairs.size() < 7)
        pairs.back()->first->setPosition(sf::Vector2u(table.getPosition().x + pairs.size() * distanceX + pairs.back()->first->size.x * (pairs.size() - 1), table.getPosition().y + distanceY));
    else
        pairs.back()->first->setPosition(sf::Vector2u(table.getPosition().x + (pairs.size() - 6) * distanceX + pairs.back()->first->size.x * (pairs.size() - 7), table.getPosition().y + 2 * distanceY + pairs.back()->first->size.y));
    selectedCardIndex = -1;
}

void Game::pickCardsFromTable(int id)
{
    std::vector<Card> cards;
    for (Pair* pair : pairs)
    {
        cards.push_back(Card(pair->first->face, pair->first->suit));
        if (pair->second != nullptr)
            cards.push_back(Card(pair->second->face, pair->second->suit));
    }
    if (id == this->id)
    {
        addCard(cards);
    }
    else
    {
        addCard(id, cards.size());
    }
    deleteCardsFromTable();
}

void Game::deleteCardsFromTable()
{
    while (!pairs.empty())
    {
        delete pairs.back()->first;
        if (pairs.back()->second != nullptr)
            delete pairs.back()->second;
        pairs.pop_back();
    }
}

void Game::updateClue()
{
    if (!isPicked)
        clue.setString(L"Ходит игрок " + std::to_wstring(players[currPlayer]->id + 1) + L", отбивается игрок " + std::to_wstring(players[vpl]->id + 1));
    else
        clue.setString(L"Подкидывает игрок " + std::to_wstring(players[currPlayer]->id + 1) + L", берет игрок " + std::to_wstring(players[vpl]->id + 1));
    sf::FloatRect textRect = clue.getLocalBounds();
    clue.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    clue.setPosition(globals::screenWidth/2, table.getPosition().y - 30);
}

void Game::updateCardCount()
{
    cardCount.setString(std::to_string(my.size));
    sf::FloatRect textRect = cardCount.getLocalBounds();
    cardCount.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    cardCount.setPosition(bcard.sprite.getPosition().x + (50 + tcard.sprite.getGlobalBounds().width) / 2, tcard.sprite.getPosition().y + tcard.sprite.getGlobalBounds().height + 20);
}

void Game::draw(sf::RenderWindow& window)
{
    window.draw(table);
    tcard.draw(window);
    bcard.draw(window);
    buttonExit.draw(window);
    window.draw(clue);
    window.draw(cardCount);
    if(isShowButtonAction)
        buttonAction.draw(window);
    for (Player* player : players)
    {
        player->draw(window);
    }
    for (Player* player : winners)
    {
        player->draw(window);
    }
    for (Pair* pair : pairs)
    {
        pair->draw(window);
    }
    if (players.size() == 1)
    {
        window.draw(shape);
        window.draw(result);
    }
}

Game::~Game()
{
    for (Player* player : players) {
        for(Card *card: player->hand)
            delete card;
        player->hand.clear();
        delete player;
    }
    players.clear();

    for (Player* player : winners) {
        for (Card* card : player->hand)
            delete card;
        player->hand.clear();
        delete player;
    }
    winners.clear();

    for (Pair* pair : pairs) {
        delete pair->first;
        if(pair->second != nullptr)
            delete pair->second;
        delete pair;
    }
    pairs.clear();
}