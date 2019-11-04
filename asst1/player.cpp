/*
 * player.cpp
 *
 * Note: change this file to implement Player subclass
 *
 * University of Toronto
 * Fall 2019
 */

#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <bits/stdc++.h> 
#include <iterator>
#include <vector>
#include "player.h"
#include "config.h"
#include "easybj.h"

using namespace std;

extern int numSplits;

class autoPlayer : public Player {
private:
    const char* stratFile;
    bool duplicateCards = false;
    int totalHands;
    bool silence;
    
    unordered_map<string, unsigned> dealerMap;
    unordered_map<string, unsigned> playerMap;
    vector<vector<string>> movesDB;
    vector<string> tempVector;
    
public:

    autoPlayer(const char * strategy_file, long num_hands, bool silent) {
        stratFile = strategy_file;
        totalHands = num_hands;
        silence = silent;
    }

    ~autoPlayer() {}

    // setting up the moves database
    bool parse() {
        
        string str;
        string dealer;
        stringstream ss;
        vector<string> tempVector;
        unsigned counter = 0;
        // open the strategy table file
        // get the first line of file; first line is the dealer's possible hands
        ifstream file(stratFile);
        getline(file, str);

        ss << str;

        //Creating the dealerMap
        while (ss >> dealer) {
            dealerMap[dealer] = counter;
            ++counter;
        }
        
        // reset the counter to 0
        counter = 0;
        
        // keep getting a new line from the file, until end of file
        while (getline(file, str)) {
            //str is new file line
            stringstream ss2(str);
            //tempVector is to store the move strings inside str
            tempVector.resize(23);
            // player reps what Hand player has (ex. player has a hand value of 4, 5, ..., A6)
            string player;
            ss2 >> player;
            // map the player's hand value to the appropriate index value
            playerMap[player] = counter;

            // gets the individual moves possible with player hand
            // inserts into map
            // checks validity of move
            for (unsigned i = 0; i < dealerMap.size(); ++i) {
                // playerMove reps the individual move the player should do
                string playerMove;
                ss2 >> playerMove;
                
                // first check if move is valid
                if (!(validMove(player, playerMove))) 
                    return false;

                //store playerMove in the appropriate index location
                tempVector[i] = playerMove;
            }
            // store tempVector inside the moves database
            movesDB.push_back(tempVector);
            tempVector.clear();
            ++counter;
        }
        return true;
    }

    // check if splitting is possible on strategy table
    bool splitCheckParsing(string player) {
        // splitting only possible on 2 cards of the same value
        if (player.size() == 2) {
            char c1 = player[0];
            char c2 = player[1];
            if (c1 == c2 || ((c1 == 'T' || c1 == 'J' || c1 == 'Q' || c1 == 'K')
                    && (c2 == 'T' || c2 == 'J' || c2 == 'Q' || c2 == 'K')) || (c1 == 'A' && c2 == 'A')) {
                return true;
            }
        }
        return false;
    }



    // checks if strategy table moves are legal
    bool validMove(string player, string move) {
        // if the move isn't valid, automatically return false
        if (move.compare("H") == 0 || move.compare("S") == 0 || move.compare("R") == 0
                || move.compare("Dh") == 0 || move.compare("Ds") == 0
                || move.compare("Rh") == 0 || move.compare("Rs") == 0) {
            return true;
        }// if move is a split
        else if (move.compare("P") == 0 || move.compare("Dp") == 0 || move.compare("Rp") == 0) {
            if (splitCheckParsing(player)) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

        // check if splitting is possible for current hand
    bool splitCheck(Hand &hand) {
        char c1 = hand.getCards()[0];
        char c2 = hand.getCards()[1];
        // splitting only possible on 2 cards of the same value
        if (hand.getCards().size() == 2) {
            if (c1 == c2 || ((c1 == 'T' || c1 == 'J' || c1 == 'Q' || c1 == 'K')
                    && (c2 == 'T' || c2 == 'J' || c2 == 'Q' || c2 == 'K')) || (c1 == 'A' && c2 == 'A')) {
                return true;
            }
        }
        return false;
    }
    
    // print out the valid options player can take
    void printMove(Hand* hand) {
        // order of options
        // Stand (S) Hit (H) Double (D) Split (P) Surrender (R)

        // always print out Stand and Hit options
        cout << "Stand (S) Hit (H)";
        // double can only happen if hand only has 2 cards
        if (hand->getCards().size() == 2 && !(hand->isDouble())) {
            cout << " Double (D)";
            // check if player can split (both cards have the same value)
            if (splitCheck(*hand)) {
                cout << " Split (P)";
            }
        }

        // can only print surrender after 1st turn
        if (hand->isFirstTurn()) {
            cout << " Surrender (R)";
        }

        cout << ": ";
    }

    // obtains move from strategy table
    void play(Hand * hand, const Hand * dealer) {
        string playerStr, dealerStr;
        // if equal to true, then need to search for the hand by the value, otherwise search by the individual cards
        bool searchByValuePlayer = true, searchByValueDealer = true;
        string move;
        char finalMove;
        
        //If the value is 21, won't find in the strategy sheet because it should automatically stand
        if (hand->getValue() == 21) {
            hand->setMove('s');
            return;
        }
      
        //Checking for split and ace conditions
        if (!(hand->isAce())) {
            //Check if player has a split condition
            if (numSplits < 3 && splitCheck(*hand)) {
                // if hands has any Face cards (ie. J, Q, K, T)
                // all are the "TT" case
                if (hand->getValue() == 20) {
                    playerStr = "TT";
                }
                else {
                  playerStr += hand->getCards()[0];
                  playerStr += hand->getCards()[1];
                }
                searchByValuePlayer = false;
            }
        }
        // if hand has an ace, if soft, second char in player's hand is value - 11
        else { 
            if (hand->isSoft()) {
                playerStr += "A";
                if (hand->getValue() - 11 == 1)
                    playerStr += "A";
                else
                    playerStr += to_string(hand->getValue() - 11);
                searchByValuePlayer = false;
            }
        }
        
        // if player's hand did NOT have any aces, or split conditions
        // search for the value of the hand in the database
        if (searchByValuePlayer) {
            playerStr = to_string(hand->getValue());
        }

        // if the dealer has an ace and meets conditions, search by the cards  
        if (dealer->isAce() && dealer->isSoft() && dealer->getValue() <= 17) {
            dealerStr += "A";
            if (dealer->getValue() - 11 == 1)
                dealerStr += "A";
            else
                dealerStr += to_string(dealer->getValue() - 11);
            searchByValueDealer = false;
        }
        
        // if the dealer's hand did not have any aces, search by the value of the hand
        if (searchByValueDealer) {
            dealerStr = to_string(dealer->getValue());
        }
        
        
        //Obtain move from the map
        move = movesDB[playerMap[playerStr]][dealerMap[dealerStr]];
        finalMove = move[0];
        
        //Check for secondary moves
        if (move.size() == 2) {
            //if move[0] is a surrender --> can use move[0]]
            if (move.at(0) == 'R' && hand->isFirstTurn())
                finalMove = move[0];
            else if (move.at(0) == 'D' && hand->getCards().size() == 2)
                finalMove = move[0];
            else
                finalMove = move.at(1);
        }

        hand->setMove(tolower(finalMove));
        
        if (!silence) {
            cout << "Dealer: " << *dealer;
            cout << "Player: " << *hand;
            printMove(hand);
            cout << finalMove << endl;
        }
        
        // if that was the first turn, set firstTurn to false;
        if (hand->isFirstTurn() && dealer->isFirstTurn()) {
            hand->setFirstTurn();
            dealer->isFirstTurn();
        }
    }
    
    // keep returning true until number of hands played equals user input
     bool again() const {
        if (this->get_hands_played() < totalHands)
            return true;
        else
            return false;
    }
    
};

class manualPlayer : public Player {
private:

    bool duplicateCards = false;

public:

    manualPlayer() {
    }

    ~manualPlayer() {
    }

    char obtainMove(Hand & hand) {
        // order of options
        // Stand (S) Hit (H) Double (D) Split (P) Surrender (R)

        string move;
        // always print out Stand and Hit options
        cout << "Stand (S) Hit (H)";
        // double can only happen if hand only has 2 cards
        if (hand.getCards().size() == 2 && !hand.isDouble()) {
            cout << " Double (D)";
            // check if player can split (both cards have the same value)
            if (splitCheck(hand) && numSplits < 3) {
                cout << " Split (P)";
            }
        }

        // can only print surrender after 1st turn
        if (hand.isFirstTurn()) {
            cout << " Surrender (R)";
        }

        cout << ": ";

        //Obtain input
        getline(cin, move);

        if (tolower(move[0]) == 'h' || tolower(move[0]) == 's' || tolower(move[0]) == 'r'
                || tolower(move[0]) == 'p' || tolower(move[0]) == 'd') {
            return tolower(move[0]);
        } else return '0';
    }

    //Check if a player can split his cards

    bool splitCheck(Hand &hand) {
        char c1 = hand.getCards()[0];
        char c2 = hand.getCards()[1];
        if (hand.getCards().size() == 2) {
            if (c1 == c2 || ((c1 == 'T' || c1 == 'J' || c1 == 'Q' || c1 == 'K')
                    && (c2 == 'T' || c2 == 'J' || c2 == 'Q' || c2 == 'K')) || (c1 == 'A' && c2 == 'A')) {
                return true;
            }
        }
        return false;
    }

    void play(Hand * hand, const Hand * dealer) {
        // Always print out the dealer and player hands
        cout << "Dealer: " << *dealer;
        cout << "Player: " << *hand;


        bool breakOut = false;
        while (!breakOut && !(hand->isBust())) {
            hand->setMove(obtainMove(*hand));

            switch (hand->getMove()) {
                case 'h':
                    breakOut = true;
                    break;
                case 's':
                    breakOut = true;
                    break;
                case 'd':
                    if (hand->getCards().size() == 2 && !hand->isDouble()) {
                        breakOut = true;
                    }
                    break;
                case 'r':
                    if (hand->isFirstTurn()) {
                        breakOut = true;
                    }
                    break;
                case 'p':
                    if (splitCheck(*hand) && numSplits < 3) {
                        breakOut = true;
                    }
                    break;
            }
        }

        // if that was the first turn, set firstTurn to false;
        if (hand->isFirstTurn() && dealer->isFirstTurn()) {
            hand->setFirstTurn();
            dealer->isFirstTurn();
        }
    }

    bool again() const {
        string userIn;
        cout << "Press Any Key to Continue, (Q to Quit): ";
        getline(cin, userIn);
        //userIn = tolower(userIn);

        if (userIn.compare("q") == 0 || userIn.compare("Q") == 0) {
            return false;
        } else return true;
    }
};

Player * Player::factory(const Config * config) {

    (void) config;
    
    //Manual Mode
    if (config->strategy_file == nullptr) {
        manualPlayer * user;
        user = new manualPlayer();
        return user;
    }//Automatic Mode
    else {
        //Pointer will be of autoPlayer
        autoPlayer * user;
        user = new autoPlayer(config->strategy_file, config->num_hands, config->silent);
        // parse returns true if not parsing errors found
        if (user->parse())
            return user;
        else
            return nullptr;
    }
}


