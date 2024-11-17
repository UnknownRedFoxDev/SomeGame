/*

/////   TO DO LIST:     /////

///// END OF TO DO LIST /////

Changelog:

07-11-2024:
- Added post-turn card exchanging function named awardTurn with winner and loser parameters.
- Added a fancy card struct:
        + easy of use with 2 variables: value and suit.
        + easy of call: call the struct and just say its value and suit with having to do the { ... }.
        + fancy getName function that lets other classes and structs doesn't have to repeat said function.
        + custom behavior for the == operator of comparison, as much as for the < operator for the map ordering.

- Changed the cardValue to global variable CARD_VALUES.
- Changed the cardColours to global variable CARD_SUITS.
- Added tons of documentation to help for future me or other readers.
- Changed the vector<int> to the card structure.
- Removed two occurrences of 'printDeck' function in player and gameDeck classes.
- Added printDeck to the deck class.
- Learn about the const keyword for function, let us and the computer know it only reads and does not change any value.
- Removed now useless extra parameter of gameDeck class as getCardName is no longer useful.
- Removed getCardName function from gameDeck class.

11-11-2024:
- Added clarification on the magic value '52'
- Tackled the issue of the "draw" situation upon having two of the same value of both player's current card.
- Updated the "awardRound" function accordingly to previous line.
- Change the parameter of the awardRound.
- Added text to tell which player won the previous round.
- Finished the previous TO-DO List :
    - Implement the players as the card decks holders. ✅
    - Implement the decks. ✅
    - Display the decks in the two other tabs to their names. ✅
    - Implement clicking on a card (when in player's tab) applying said card to the player's card displayed in the center of the window. ✅
    - Implement a ready button for both tabs -> collapses the player's deck and displays a text saying which they chose. ✅
    - Based on the upper to do, implement the game logic. ✅
    - Implement that once both players pressed ready, display text between the two displayed cards telling who won. ✅
    - Learn about putting things in the heap to save such a place. ✅

- Additional changes:
    - Correctness in putting keywords such as 'const' in the right place.
    - Changing a for loop to one with an auto iterator.
    - Learn about static functions in classes. (very useful -> makes so that you don't really have to initialize the class but still can use the function itself.)
    - Used the proper way to cast types in C++. (a.k.a. using static_cast<type> thus an 'explicit cast', rather than (type) which is an 'implicit cast' from C.)
    - Removed useless import (time.h which is apparently deprecated).
    - Learned about explicit constructors.
    - Probably will learn about the malloc function to use rather than the new keyword in the future. (But rn I'm too lazy :p)
    - Now using the constexpr keyword to define constants. Even though I still don't clearly understand what it does.


///// END OF CHANGELOG /////

*/

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "data/headers/customFunctions.h"
#include <iostream>

#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

const vector<string> CARD_VALUES = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };
const vector<string> CARD_SUITS = { "Clubs", "Spade", "Heart", "Diamonds" };


/*
   ################################ ################################ ################################
   ################################                                  ################################
   ################################            STRUCTURES            ################################
   ################################                                  ################################
   ################################ ################################ ################################
*/

struct CardStruct {
    int value;
    int suit;

    CardStruct(const int v = -1, const int s = -1) : value(v), suit(s) {}

    string getName() const {
        return CARD_VALUES[value] + " of " + CARD_SUITS[suit];
    }

    bool operator==(const CardStruct& otherCard) const {
        return value == otherCard.value;
    }

    bool operator<(const CardStruct& otherCard) const {
        // Defines ordering based on `value` first, then `suit`
        if (value != otherCard.value) {
            return value < otherCard.value;
        }
        return suit < otherCard.suit;
    }
};

CardStruct DEFAULT_CARD(-1, -1);
int DEFAULT_CARD_ID = 52;
map<CardStruct, string> IMAGES = {
    { {0, 0}, "01"}, {{1, 0}, "02"}, {{2, 0}, "03"}, {{3,  0}, "04"}, {{4,  0}, "05"}, {{5,  0}, "06"}, {{6, 0}, "07"}, {{7, 0}, "08"}, {{8, 0}, "09"}, {{9, 0}, "10"}, {{10, 0}, "11"}, {{11, 0}, "12"}, {{12, 0}, "00"},
    { {0, 1}, "14" }, { {1, 1}, "15" }, { {2, 1}, "16" }, { {3,  1}, "17" }, { {4,  1}, "18" }, { {5,  1}, "19" }, { {6, 1}, "20" }, { {7, 1}, "21" }, { {8, 1}, "22" }, { {9, 1}, "23" }, { {10, 1}, "24" }, { {11, 1}, "25" }, { {12, 1}, "13" },
    { {0, 2}, "27" }, { {1, 2}, "28" }, { {2, 2}, "29" }, { {3,  2}, "30" }, { {4,  2}, "31" }, { {5,  2}, "32" }, { {6, 2}, "33" }, { {7, 2}, "34" }, { {8, 2}, "35" }, { {9, 2}, "36" }, { {10, 2}, "37" }, { {11, 2}, "38" }, { {12, 2}, "26" },
    { {0, 3}, "40" }, { {1, 3}, "41" }, { {2, 3}, "42" }, { {3,  3}, "43" }, { {4,  3}, "44" }, { {5,  3}, "45" }, { {6, 3}, "46" }, { {7, 3}, "47" }, { {8, 3}, "48" }, { {9, 3}, "49" }, { {10, 3}, "50" }, { {11, 3}, "51" }, { {12, 3}, "39" }
};

/*
 *  Definition:
 *      The DeckStruct is a struct that handles the entirety of the different action
 *      permitted upon a 'deck' of cards.
 *      It holds the main functions, often used to manipulate the insides of a deck.
 */
struct DeckStruct {
    vector<CardStruct> hand;

    CardStruct takeCard(CardStruct& card = DEFAULT_CARD) {
        if (!isDeckEmpty()) {

            // If the card isn't specified, then take the first to come
            // PS: THIS IS SUPPOSED TO BE THE ACTUAL WAY TO PLAY
            //     YOU DON'T NORMALLY CHOOSE WHICH ON YOU ARE GOING TO PLAY!
            //     BUT RIGHT NOW IT IS FOR TESTING PURPOSE, SO BEAR IN MIND THAT!!!
            if (card == DEFAULT_CARD) {
                card = hand[0];
                hand.erase(hand.begin());
                return card;
            }
            else {

                // Searching through the hand for the vector (card)
                for (int i = 0; i < hand.size(); i++) {
                    if (card == hand[i]) {
                        hand.erase(hand.begin() + i);
                        return card;
                    }
                }

                printf("The card specified was not found!\n");
            }
        }
        else {
            printf("No more cards!\n");
        }
        return card;
    }

    void removeCard(const CardStruct& card) {
        for (int i = 0; i < hand.size(); i++) {
            if (hand[i] == card) {
                hand.erase(hand.begin() + i);
                break;
            }
        }
    }

    void addCard(const CardStruct& card) {
        hand.push_back(card);
    }

    bool isDeckEmpty() const {
        return hand.empty();
    }

    int getHandSize() const {
        /* The C way: return (int)(hand.size());*/
        return static_cast<int> (hand.size());
    }

    /*Static member functions:
        Declared inside a class, a static member function can be called without creating an object of the class.
        It has access only to other static members and not to non-static members (instance variables and methods).
        Static member functions are essentially global functions that happen to be part of a class
    */
    static void printCard(const CardStruct& card) {
        printf("{%d, %d}", card.value, card.suit);
    }

    void printDeck() const {
        /*
         * The C ways to do it:
         *  for (int i = 0; i < hand.size(); i++) {
         *      printf("%s, ", hand[i].getName());
         *  }
         *
         */
        for (auto i : hand) {
            cout << i.getName() << ", ";
        }
    }

};

/* 
   ################################ ################################ ################################
   ################################                                  ################################
   ################################             CLASSES              ################################
   ################################                                  ################################
   ################################ ################################ ################################
*/

/*
 *  Definition:
 *      The GameDeckClass is a class that is used as the main frame of value 
 *      storing and card evaluation and manipulation.
 */
class GameDeckClass {
public:
    DeckStruct deck;

    void shuffleDeck() {
        // Uses a random int generator to shuffle the cards
        random_device rd;
        default_random_engine rng(rd());
        for (int i = 0; i < IMAGES.size(); i++) {
            shuffle(deck.hand.begin(), deck.hand.end(), rng);
        }
    }

    static int getCardID(const CardStruct& playerCard){
        return stoi(IMAGES.at(playerCard));
    }

    static CardStruct getCardByID(const int cardID) {
        if (cardID != 52) {
            CardStruct cardFound;
            for (const auto& pair : IMAGES) {
                if (pair.second == zeroPaddingInt(cardID)) {
                    cardFound.suit = pair.first.suit;
                    cardFound.value = pair.first.value;
                    return cardFound;
                }
            }
            return cardFound;
        }
        return { -1, -1 };
    }

    static CardStruct getCardByName(string& cardName) {
        // Example input format: "Ace of Spade"
        CardStruct result;

        // Find the position of " of " to split the string
        const size_t pos = cardName.find(" of ");
        if (pos == string::npos) {
            return result;
        }

        // Extract the value and color parts
        const string valueStr = cardName.substr(0, pos);
        const string colorStr = cardName.substr(pos + 4);

        // Find value in cardValue vector
        const auto valueIt = find(CARD_VALUES.begin(), CARD_VALUES.end(), valueStr);
        if (valueIt == CARD_VALUES.end()) {
            return result;
        }
        const int valueIndex = static_cast<int> (distance(CARD_VALUES.begin(), valueIt));

        // Find color in cardColour vector
        const auto colourIt = find(CARD_SUITS.begin(), CARD_SUITS.end(), colorStr);
        if (colourIt == CARD_SUITS.end()) {
            return result;
        }
        const int suitIndex = static_cast<int> (distance(CARD_SUITS.begin(), colourIt));

        // Construct the result vector {valueIndex, suitIndex}
        result.value = valueIndex;
        result.suit = suitIndex;

        return result;
    }

    GameDeckClass() {
        for (int i = 0; i < CARD_SUITS.size(); i++) {
            for (int j = 0; j < CARD_VALUES.size(); j++) {
                deck.addCard({ j, i });
            }
        }
    }
};

/*
 *  Definition:
 *      The CardClass is a class destined to be used as a placeholder for
 *      sprite and texture manipulation onto the screen.
 *      Its purpose is purely graphical and holds no technical functionality
 *      in relation to the deck of cards.
 */
class CardClass {
public:
    static constexpr int NUM_CARDS = 53;

    sf::Texture cardTextures[NUM_CARDS];
    sf::Sprite cardSprites[NUM_CARDS];

    sf::Sprite getCardSpriteAt(const int index) const {
        if (index < NUM_CARDS) {
            return cardSprites[index];
        }

        return cardSprites[NUM_CARDS - 1];
    }

    CardClass() {
        for (int i = 0; i < NUM_CARDS; i++) {
            string cardLoaded = "./data/textures/cards/" + zeroPaddingInt(i) + ".png";
            cardTextures[i].loadFromFile(cardLoaded);
            cardSprites[i].setTexture(cardTextures[i]);
        }
    }
};

/*
 *  Definition:
 *      The PlayerClass is a class that has a purpose in quantifying the different elements
 *      required to play a game of war.
 *      It also holds useful functions allowing the player to easily interfere with its deck.
 */
class PlayerClass {
public:
    DeckStruct playerDeck;
    DeckStruct waitingDeck;
    CardStruct currentCard = DEFAULT_CARD;
    int cardID;
    int playerID;
    int victoryCount = 0;
    bool confirmedCard = false;
    sf::Sprite cardSprite;


    void checkDeck() {
        if (playerDeck.hand.size() < 6) {
            for (int i = 0; i < waitingDeck.hand.size(); i++) {
                waitingDeck.addCard(waitingDeck.hand[i]);
                waitingDeck.hand.erase(waitingDeck.hand.begin());
            }
            if (!waitingDeck.hand.empty()) { waitingDeck.hand.clear(); }
        }
    }

    void playCard(const CardStruct& card = DEFAULT_CARD) {
        currentCard = card;
        confirmedCard = true;
    }

    explicit PlayerClass(const int& pID, const CardClass& cardGraphics) { //Sets the playerID's to the number given in argument.
        cardID = 52; // Last card in the texture deck.
        playerID = pID;  
        cardSprite.setTexture(cardGraphics.cardTextures[cardID]);
    }

};

static void playerTab(PlayerClass& player, CardClass& cardGraphics, int& newCardID, bool& showPlayerActionMenu);
static void displayPlayerDeck(PlayerClass& player, const CardClass& cardGraphics, const DeckStruct& deck);
static void awardRound(PlayerClass& winner, PlayerClass& loser, const CardClass& cardGraphics, const bool& draw, int& battleTime, DeckStruct* battleDeck);
static int changeCard(PlayerClass& dest, const int& cardID);


/* 
   ################################ ################################ ################################
   ################################                                  ################################
   ################################          MAIN FUNCTION           ################################
   ################################                                  ################################
   ################################ ################################ ################################
*/


/*
 *  Definition:
 *      The main function. While obligated to run the code, here lies
 *      the entirety of the window's logic, variable storing, game logic, and many more things.
 */
int main()
{
    float windowSize[2] = {1336.0, 780.0};
    sf::RenderWindow window(sf::VideoMode(
        static_cast<int> (windowSize[0]),
        static_cast<int> (windowSize[1])
        ),
        "Window Title",
        sf::Style::Titlebar | sf::Style::Close);

    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    window.setFramerateLimit(144);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    // Initialization of basic values;
    auto* cardGraphics = new CardClass(); // Very big class that requires it to be in the heap
    /*auto* cardGraphics = static_cast<CardClass *>(malloc(sizeof(CardClass)));*/

    GameDeckClass gameDeck;
    DeckStruct battleDeck;
    int battleTime = 0;

    // Player initialization
    PlayerClass P1(1, *cardGraphics);
    sf::Sprite* p1Card = &P1.cardSprite;
    int p1NewCardID = 52;
    bool p1ShowReadyText = true;
    bool showP1ActionMenu = true;

    PlayerClass P2(2, *cardGraphics);
    sf::Sprite* p2Card = &P2.cardSprite;
    int p2NewCardID = 52;
    bool p2ShowReadyText = true;
    bool showP2ActionMenu = true;

    bool debugWindow = true;
    bool dropDownMenu = false;
    bool playerDebugMenu = false;
    bool spriteDebugMenu = false;

    // Deck Manipulation
    gameDeck.shuffleDeck();
    const int gameDeckSize = gameDeck.deck.getHandSize();

    for (int i = 0; i < gameDeckSize/2; i++) {
        P1.playerDeck.addCard(gameDeck.deck.takeCard());
    }

    for (int i = 0; i < gameDeckSize/2; i++) {
        P2.playerDeck.addCard(gameDeck.deck.takeCard());
    }

    // Graphics management
    p1Card->setPosition(
        static_cast<float>((windowSize[0] / 2) - cardGraphics->cardTextures[0].getSize().x / 2.0), // X position
        static_cast<float>((windowSize[1] / 3) - cardGraphics->cardTextures[0].getSize().y / 2.0) // Y position
    );
    p2Card->setPosition(
        static_cast<float>((windowSize[0] / 2) - cardGraphics->cardTextures[0].getSize().x / 2.0), // X position
        static_cast<float>(((windowSize[1] * 2.075) / 3) - cardGraphics->cardTextures[0].getSize().y / 2.0) // Y position
    );

    sf::Text previousRoundWinner;

    sf::Font font;
    font.loadFromFile("arial.ttf");

    previousRoundWinner.setFont(font);                      // Set the loaded font
    previousRoundWinner.setString("No Winner Yet.");        // Set the text content
    previousRoundWinner.setCharacterSize(30);               // Set the character size
    previousRoundWinner.setFillColor(sf::Color::White);     // Set the color

    sf::FloatRect textBounds = previousRoundWinner.getLocalBounds();
    previousRoundWinner.setOrigin(textBounds.width / 2, textBounds.height / 2);
    previousRoundWinner.setPosition(windowSize[0] / 2, windowSize[1] / 2);


    // Other values
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        
        
        // ImGui Logic Here.
        ImGuiWindowFlags windowFlagsPlayers = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        constexpr float pWinSizeX = 550;
        constexpr float pWinSizeY = 500;

        if (showP1ActionMenu) {
            ImGui::SetNextWindowSize(ImVec2(pWinSizeX, pWinSizeY));
            ImGui::SetNextWindowPos(ImVec2(0,
                static_cast<float>((windowSize[1] / 3) - cardGraphics->cardTextures[0].getSize().y / 2.0)));

            ImGui::Begin("Player 1", nullptr, windowFlagsPlayers);
            if (!P1.confirmedCard) {
                if (ImGui::TreeNode("Deck:")) {
                    displayPlayerDeck(P1, *cardGraphics, P1.playerDeck);
                    ImGui::TreePop();
                }
                if (ImGui::Button("Confirm Chosen Card?")) {
                    if (P1.cardID != 52) {
                        P1.confirmedCard = true;
                        cout << "Player 1 as chosen: " << P1.currentCard.getName() << endl;
                    }
                }
            }
            if (p1ShowReadyText)
                ImGui::Text("You have not selected a card yet!");
            ImGui::End();
        }

        if (showP2ActionMenu) {
            ImGui::SetNextWindowSize(ImVec2(pWinSizeX, pWinSizeY));
            ImGui::SetNextWindowPos(ImVec2(windowSize[0] - pWinSizeX,
                static_cast<float>((windowSize[1] / 3.0) - cardGraphics->cardTextures[0].getSize().y / 2.0)));

            ImGui::Begin("Player 2", nullptr, windowFlagsPlayers);
            if (!P2.confirmedCard) {
                if (ImGui::TreeNode("Deck:")) {
                    displayPlayerDeck(P2, *cardGraphics, P2.playerDeck);
                    ImGui::TreePop();
                }

                if (ImGui::Button("Confirm Chosen Card?")) {
                    if (P2.cardID != 52) {
                        P2.confirmedCard = true;
                        cout << "Player 2 as chosen: " << P2.currentCard.getName() << endl;
                    }
                }
                if (p2ShowReadyText)
                    ImGui::Text("You have not selected a card yet!");
            }
            ImGui::End();
        }

        if (debugWindow) {
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
            ImGui::SetNextWindowSize(ImVec2(windowSize[0], 35));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowBgAlpha(100);

            ImGui::Begin("Debug Window", nullptr, windowFlags);

            ImGui::PushID(0);
            if (!dropDownMenu)
                ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.06f)));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.588f, 0.68f, 0.43f)));

            if (ImGui::Button("Debug")) dropDownMenu = !dropDownMenu;
            ImGui::PopStyleColor(1);
            ImGui::PopID();
            ImGui::End();
        }

        if (dropDownMenu) {
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
            ImGui::SetNextWindowSize(ImVec2(160, 75));
            ImGui::SetNextWindowPos(ImVec2(15, 34));
            ImGui::SetNextWindowBgAlpha(100);

            ImGui::Begin("DropDown Menu", nullptr, windowFlags);
            if (ImGui::MenuItem("Players' Values")) { playerDebugMenu = !playerDebugMenu; }
            if (ImGui::MenuItem("Sprites Grid")) { spriteDebugMenu = !spriteDebugMenu; }
            ImGui::Text("%.1f FPS", io.Framerate);

            ImGui::End();
        }

        if (playerDebugMenu) {
            ImGui::SetNextWindowBgAlpha(100);

            // Sliders to swiftly change the player's current cards.

            ImGui::Begin("Player", &playerDebugMenu);
            //Sliders to swiftly change the player's current cards.
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
                if (ImGui::BeginTabItem("Player 1")) {
                    playerTab(P1, *cardGraphics, p1NewCardID, showP1ActionMenu);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Player 2")) {
                    playerTab(P2, *cardGraphics, p2NewCardID, showP2ActionMenu);
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
            ImGui::End();
        }

        if (spriteDebugMenu) {
            ImGui::Begin("Sprite Debug Menu", &spriteDebugMenu);
            int cardOnLine = 0;
            for (int i = 0; i < 52; i++) {
                if (ImGui::ImageButton(cardGraphics->getCardSpriteAt(i))) {
                    CardStruct cardTemp = GameDeckClass::getCardByID(i);
                    cout << "clicked card: " << cardTemp.getName() << " (ID: " << zeroPaddingInt(i) << ", Vector: {" << cardTemp.value << ", " << cardTemp.suit << "})" << endl;
                }
                cardOnLine++;

                if (cardOnLine != 8) {
                    ImGui::SameLine();
                }
                else cardOnLine = 0;
            }
            ImGui::End();
        }


            //ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            //if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            //{
            //    // Debugging tab
            //    
            //}
            

        // Looping Logics Here.
        window.clear(sf::Color(18, 33, 43)); // Color background

        if (P1.cardID != 52)
            p1ShowReadyText = false;
        else
            p1ShowReadyText = true;

        if (P2.cardID != 52)
            p2ShowReadyText = false;
        else
            p2ShowReadyText = true;

        ////////////////////////////////////////////////////////////////////
        // 
        // 
        // Upon confirmation of both parties:
        // >> Start game logic HERE.
        if (P1.confirmedCard && P2.confirmedCard) {
            cout << battleTime << endl;
            if (P1.currentCard.value > P2.currentCard.value) {
                // Player 1 won
                // Winner Text Y position: (p1Card->getPosition().y + p2Card->getPosition().y) / 2
                previousRoundWinner.setString("Player 1 won!");
                if (!battleTime) awardRound(P1, P2, *cardGraphics, false, battleTime, &battleDeck);
                else awardRound(P1, P2, *cardGraphics, true, battleTime, &battleDeck);
            }
            else if (P1.currentCard.value < P2.currentCard.value){
                // Player 2 won
                previousRoundWinner.setString("Player 2 won!");
                if (!battleTime) awardRound(P2, P1, *cardGraphics, false, battleTime, &battleDeck);
                else awardRound(P2, P1, *cardGraphics, true, battleTime, &battleDeck);
            }
            else {
                // Draw
                // The order doesn't matter, it's a draw, nobody has won.
                previousRoundWinner.setString("IT'S A DRAW!");
                battleTime += 2;
                awardRound(P1, P2, *cardGraphics, true, battleTime, &battleDeck);
            }

            // Lasting thing done is the reactivation of the decks and buttons.
            P1.confirmedCard = P2.confirmedCard = false;
        }
        //
        //
        ////////////////////////////////////////////////////////////////////

        // Draw everything past here.
        window.draw(*p1Card);
        window.draw(*p2Card);
        window.draw(previousRoundWinner);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    delete cardGraphics;
    return 0;
}

/* 
   ################################ ################################ ################################
   ################################                                  ################################
   ################################         HELPER FUNCTIONS         ################################
   ################################                                  ################################
   ################################ ################################ ################################
*/


/*
 *  Description:
 *      Takes in a player, the gameDeck and the graphic card classes.
 *      It uses those classes to display a player's deck in its entirety.
 * 
 *      Moreover, when one card is clicked, its information is printed 
 *      onto the command line.
 *
 *      Returns nothing.
 */
static void displayPlayerDeck(PlayerClass& player, const CardClass& cardGraphics, const DeckStruct& deck) {
    for (int i = 0; i < deck.getHandSize(); i++) {
        if (i % 3 != 0) ImGui::SameLine();

        int cardID = GameDeckClass::getCardID(deck.hand[i]);
        string cardName = deck.hand[i].getName();
        const string cardIDStr = to_string(cardID);

        if (ImGui::ImageButton(cardGraphics.getCardSpriteAt(cardID))) {
            player.currentCard = GameDeckClass::getCardByName(cardName);
            player.cardID = cardID;

            cout << "P" << player.playerID << " clicked card: " << cardName << " (ID: " << zeroPaddingInt(cardID) << ")" << endl;
            cout << "P" << player.playerID << " current card: [" << player.currentCard.value << ", " << player.currentCard.suit << "] (ID: " << zeroPaddingInt(cardID) << ")" << endl;
        }
    }
}


/*
 *  Description:
 *      Takes in two players, one supposed round winner and the other, the round loser.
 *      This function acts on both players' values. It removes the current card of
 *      both players to put them inside the winner's waiting deck.
 *      
 *      Moreover, the winner's victory count is increased by one.
 *      
 *      Returns nothing.
 */
static void awardRound(PlayerClass& winner, PlayerClass& loser, const CardClass& cardGraphics, const bool& draw, int& battleTime, DeckStruct* battleDeck) {
    if (!draw && !battleTime) {
        // The term 'loser' is not degrading as in "boo you loser!"
        // it's more for technicality and understanding of the roles
        printf("Player %d won\n", winner.playerID);

        // 3 stones in 2 lines:
        // adding to the waiting deck of the winner, the card of both winner and loser.
        // Moreover, the cards from both parties are also removed from their deck.
        winner.waitingDeck.addCard(winner.playerDeck.takeCard(winner.currentCard));
        winner.waitingDeck.addCard(loser.playerDeck.takeCard(loser.currentCard));

        winner.cardSprite.setTexture(cardGraphics.cardTextures[winner.cardID]);
        loser.cardSprite.setTexture(cardGraphics.cardTextures[loser.cardID]);

        winner.currentCard = loser.currentCard = DEFAULT_CARD;
        winner.cardID = loser.cardID = DEFAULT_CARD_ID;

        winner.victoryCount++;
    }
    else if (draw && battleTime == 1) {

        printf("Player %d won this draw\n", winner.playerID);
        winner.waitingDeck.addCard(winner.playerDeck.takeCard(winner.currentCard));
        winner.waitingDeck.addCard(loser.playerDeck.takeCard(loser.currentCard));
        for (int i = 0; i < battleDeck->getHandSize(); i++) {
            winner.waitingDeck.addCard(battleDeck->takeCard());
        }

        winner.cardSprite.setTexture(cardGraphics.cardTextures[winner.cardID]);
        loser.cardSprite.setTexture(cardGraphics.cardTextures[loser.cardID]);

        winner.currentCard = loser.currentCard = DEFAULT_CARD;
        winner.cardID = loser.cardID = DEFAULT_CARD_ID;

        winner.victoryCount++;
        battleTime--;
    }
    else if (draw) {
        if (battleTime >= 2){
            // This prompt shenanigan is only for debug purpose
            system("cls");
            printf("---------------------------------------------------------\n");
            printf("It's a draw\n");
            printf("Your first card has been automatically played and hidden.\n");
            printf("This turn will give the winner a total of 6 cards.\n");
            printf("---------------------------------------------------------\n");

            /*
            // Quick explanation if you don't know how this works.
            // When a draw occurs, basically that both players have the same value for their current card,
            // then the way it is played is that both players put their current card to the middle,
            // take the first card in their hand and put it on their previous current card, but
            // they put this new card as hidden, so they don't show what it is.
            // and then they finally draw the third card which is decisive of who's the winner of that draw.
            */


            battleDeck->addCard(winner.playerDeck.takeCard(winner.currentCard));
            battleDeck->addCard(winner.playerDeck.takeCard()); // So that's the second card that is hidden

            battleDeck->addCard(loser.playerDeck.takeCard(loser.currentCard));
            battleDeck->addCard(loser.playerDeck.takeCard()); // So that's the second card that is hidden

            winner.currentCard = loser.currentCard = DEFAULT_CARD;
            winner.cardID = loser.cardID = DEFAULT_CARD_ID;
            battleTime--;
        }
    }
}

static int changeCard(PlayerClass& dest, const int& cardID) {
    if (cardID == 52) return 1;
    CardStruct newCard = GameDeckClass::getCardByID(cardID);

    dest.currentCard = newCard;
    dest.cardID = cardID;
    return 0;
}


static void playerTab(PlayerClass& player, CardClass& cardGraphics, int& newCardID, bool& showPlayerActionMenu) {
    ImGui::SeparatorText("Card");
    ImGui::DragInt(": New Card ID", &newCardID, 1, 0, 52, "%02d");
    ImGui::SameLine();

    if (ImGui::Button("Apply New Card")) {
        if (changeCard(player, newCardID)) cout << "The transfer was unsuccessful.\n";
    }

    ImGui::SeparatorText("Decks");
    ImGui::Checkbox("Show Player's Action Menu", &showPlayerActionMenu);
    if (ImGui::TreeNode("Deck")) {
        displayPlayerDeck(player, cardGraphics, player.playerDeck);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Waiting Deck")) {
        displayPlayerDeck(player, cardGraphics, player.waitingDeck);
        ImGui::TreePop();
    }

    ImGui::SeparatorText("Values");
    ImGui::Text("Amount of cards in deck: %d cards", player.playerDeck.getHandSize());
    ImGui::Text("Amount of cards in waiting deck: %d cards", player.waitingDeck.getHandSize());
    ImGui::Text("Amount of victories: %d", player.victoryCount);
}
