 /*
 * easybj.h
 *
 * Header files for Easy Blackjack.
 *
 * Note: change this file to implement Blackjack
 *
 * University of Toronto
 * Fall 2019
 */
 
#ifndef EASYBJ_H
#define EASYBJ_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>




using namespace std;

class Player;
class Shoe;
class Config;
class Hand;


class Hand {
    // the cards in THIS hand
    vector<char> cards;
    // total value of the hand
    int value = 0;
    // are there any aces in current hand
    bool ace = false;
    // bool for soft ace (11) vs hard ace (1))
    bool soft = false;
    // is this the player's hand?
    bool playerHand = true;
    // is this hand DOUBLED?
    bool doubled = false;
    // is this hand SURRENDERED?
    bool surrender = false;
    // is the hand busted?
    bool bust = false;
    //is it the first turn of the hand
    bool firstTurn = false;
    //Move to be done on the hand (int because of ascii characters for switch case)
    int move;
    
public:
    
    // constructor
    Hand() {}
    ~Hand() {}
    
    //friend class Blackjack; 
    
    // calculate the total value
    // convert face cards into 10
    // figure out when it's best to have a soft ace (11) vs hard ace (1)
    int calculate() {
        
        
        
        //reset flags before calculating
        ace = false;
        soft = false;
        value = 0;
        int numAces = 0;
        // check to see if cards vector is empty or not
        if (cards.empty()) {
            return value = 0;
        }

        // go through the cards
        // if there's an Ace, set ace flag to True
        // skip Ace (for now), continue adding all other values

        //check if it's a face or a ten
        for (unsigned i = 0; i < cards.size(); i++) {
            if (cards[i] == 'T' || cards[i] == 'J' || cards[i] == 'Q' \
                        || cards[i] == 'K') {
                value += 10;
            }
            else if (cards[i] == 'A') {
                ace = true;
                numAces ++;
            }
            else {
                //convert 2 to 9 chars to their int values
                value += cards[i] - 48;
            }
        }
        
        //if there were Aces, determine if they should be 11 or 1
        if (ace) {
            // cannot have more than one Ace that equals 11 (will bust otherwise))
            while (numAces > 1) {
                value += 1;
                numAces--;
            }
            // aim for high Ace if it doesn't bust the hand
            if (numAces == 1 && value + 11 <= 21) {
                value += 11;
                soft = true;
            }
            else 
                value += 1;
        }
        
        //if the card value is greater than 21, then it is busted
        if (value > 21)
            bust = true;
        
        
        return value;
    }
    
    
    bool isEmpty() const {
        if (cards.empty())
            return true;
        else return false;
    }
    
    // is there an Ace in this hand
    bool isAce() const {
        return ace;
    }
    
    // is this a soft or hard hand
    bool isSoft() const {
        return soft;
    }
    
    //if this is the dealer's hand, set playerHand to false
    void setDealerHand() {
        playerHand = false;
    }
    
    // is this hand one of the player's
    bool isPlayer() const {
        return playerHand;
    }
    
    // if hand is DOUBLED, set double to true
    void setDouble() {
        doubled = true;
    }
    
    // is this hand's bet been doubled
    bool isDouble() const {
        return doubled;
    }
    
    // if hand is SURRENDERED, set surrender to true
    void setSurrender() {
        surrender = true;
    }
    
    // if this hand surrendered?
    bool isSurrender() const {
        return surrender;
    }
    
    int getValue() const {
        return value;
    }
    
    void setBust() {
        bust = true;
    }
    
    // if this hand surrendered?
    bool isBust() const {
        return bust;
    }
    
    // set the move
    void setMove(char c) {
        move = c;
    }
    
    // get the move that is to be done
    char getMove() const {
        return move;
    }
    
    // get the cards
    vector<char> getCards() const {
        return cards;
    }
    
    void setCards(char c) {
        cards.push_back(c);
    }
    
    void deleteCard() {
        cards.erase(cards.end() - 1);
    }
    
    //Change so that it is not the first turn of the game
    void setFirstTurn() {
        if (firstTurn)
            firstTurn = false;
        else 
            firstTurn = true;
    }

    // is it the first turn of the game?
    bool isFirstTurn() const {
        return firstTurn;
    }
    
//    void hit() {
//        this->cards.push_back(shoe->pop());
//    }
    
    //output only the cards in the hand
    friend std::ostream & operator<<(std::ostream & ostr, const Hand & hand) {
        (void) hand;
        
        for (unsigned i = 0; i < hand.getCards().size(); ++i) {
            ostr << hand.getCards()[i] << " ";
        }
        //print out the total of the hand
        // first check if total is a bust or not
        if (hand.getValue() > 21) {
            ostr << "(bust)";
        }
        //Case is if there is a blackjack on the first turn
        else if (hand.isFirstTurn() && hand.getValue() == 21) {
            ostr << "(blackjack)";
        }
        else {
            ostr << "(";
            //check if ace
            if (hand.isAce()) {
                if (hand.isSoft()) {
                    ostr << "soft ";
                } 
//                else {
//                    ostr << "hard ";
//                }
            }
            ostr << hand.getValue() << ")";
        }

        // if hand has been doubled
        if (hand.isDouble()) {
            ostr << " DOUBLE\n";
        } else if (hand.isSurrender()) {
            ostr << " SURRENDER\n";
        } else {
            ostr << "\n";
        }
        return ostr;
    }
   
};

class Blackjack {
    Player * player;
    Shoe * shoe;
    Hand dealerHand;
    vector<Hand> userHands;
    int currentHandId = 0;
    int numHands = 1;
    double profit = 0;
    //bool firstTurn = true;


public:
    Blackjack(Player * p, Shoe * s);
    ~Blackjack();

    friend class Hand;
    /*
     * Start a game of Blackjack
     *
     * Returns first hand to be played, nullptr if either dealer or player's
     * initial hand is blackjack (or both)
     */
    Hand * start();

    /*
     * Returns dealer's hand
     */
    const Hand * dealer_hand() const {
        return &dealerHand;
    }

    /*
     * Returns next hand to be played (may be the same hand)
     */
    Hand * next();

    /*
     * Call once next() returns nullptr
     */
    void finish();

    void setFirst();

    bool isFirst();
    
    void hit(Hand &hand);
    
    friend std::ostream & operator<<(std::ostream &, const Blackjack &);

    // TODO: you may add more functions as appropriate
};

/*
 * Returns string representation of currency for v
 */
std::string to_currency(double v);

#endif
