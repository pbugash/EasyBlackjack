/*
 * easybj.cpp
 *
 * Note: change this file to implement Blackjack logic
 *
 * University of Toronto
 * Fall 2019
 */

#include "easybj.h"
#include "player.h"
#include "shoe.h"
#include <sstream>
#include <iomanip>
#include <vector>

int numSplits = 0;

Blackjack::Blackjack(Player * p, Shoe * s)
	: player(p)
	, shoe(s) {}

Blackjack::~Blackjack() {}

 
Hand * Blackjack::start() 
{    
    Hand user;
    int dValue = 0;
    int pValue = 0;
    numSplits = 0;
    // first 2 cards go to the dealer
    hit(dealerHand);
    hit(dealerHand);
    dealerHand.setDealerHand();
    dealerHand.setFirstTurn();
    
    //next 2 go to the player
    hit(user);
    hit(user);
    userHands.push_back(user);
    //set firstTurn to true --> just started the game
    userHands[0].setFirstTurn();
    
    // check if dealer OR user has gotten 21
    dValue = dealerHand.getValue();
    pValue = userHands[0].getValue();
    
    // if dealer or player has a blackjack, end the game automatically
    if (dValue == 21 || pValue == 21) {
        return nullptr;
    }
    else
        return &(userHands[0]);
}

Hand * Blackjack::next() 
{
    // before getting the next available hand
    // performance appropriate action based on move
    switch (userHands[currentHandId].getMove()) {
        // HIT
        case 'h':
            // current hand gets another card
            hit(userHands[currentHandId]);
            // if your hand busts AND there's another hand you can go to
            // go to the next hand
            if (userHands[currentHandId].getValue() >= 21 && numHands > currentHandId + 1) {
                ++currentHandId;
                return &(userHands[currentHandId]);
            }
            // if you're below 21, you can still make another move on current hand
            else if (userHands[currentHandId].getValue() < 21) {
                return &(userHands[currentHandId]);
            }
            // if you bust and you have no more hands, go to dealer
            else if (userHands[currentHandId].getValue() >= 21 && currentHandId == numHands - 1) {
                return nullptr;
            }
            break;
        // STAND
        case 's':
            // if player has split, go to next available hand, otherwise go to dealer
            if (numHands - 1 > currentHandId) {
                currentHandId++;
                return &(userHands[currentHandId]);
            }
            else 
                return nullptr;
            break;
        // DOUBLE
        case 'd':
            // set current hand's double flag to true
            userHands[currentHandId].setDouble();
            hit(userHands[currentHandId]);
            if (numHands > currentHandId + 1) {
                ++currentHandId;
                return &(userHands[currentHandId]);
            }
            else
                return nullptr;
            break;
        // SURRENDER
        case 'r':
            // set current hand's surrender flag to true
            userHands[currentHandId].setSurrender();
            if (numHands < currentHandId) {
                currentHandId++;
                return &(userHands[currentHandId]);
            }
            else 
                return nullptr;
            break;
        // SPLIT
        case 'p':
            //Account for printing of split and resplitting aces
            ++numSplits;
            if (numHands < 4) {
                bool doubleAces = (userHands[currentHandId].getCards()[0] == userHands[currentHandId].getCards()[1]) 
                        && (userHands[currentHandId].getCards()[0] == 'A');
                
                ++numHands;
                userHands.resize(numHands);
                //Creates temporary vector for the 2nd hand
                char tempCard = userHands[currentHandId].getCards()[1];

                //Takes out card from the first hand and adds new card to that end
                userHands[currentHandId].deleteCard();
                hit(userHands[currentHandId]);

                // Creates a new hand, adds previous card from original hand; also hits
                userHands[numHands - 1].setCards(tempCard);
                hit(userHands[numHands - 1]);

                //update the values of both hands
                if (!doubleAces)
                    return &(userHands[currentHandId]);
                else return nullptr;
                break;
            }
            return &(userHands[currentHandId]);
    }
    
    return nullptr;
}

void Blackjack::hit(Hand &hand) {
    // get new card and calculate new value
    hand.setCards(shoe->pop());
    hand.calculate();
}

std::ostream & operator<<(std::ostream & ostr, const Blackjack & bj) {

    (void) bj;
    vector<Hand> handsVec = bj.userHands;
    Hand dealer = bj.dealerHand;

    //print out the dealer's hand first
    ostr << "Dealer: ";
    ostr << dealer;
    
    // make sure that player DOES have at least 1 hand
    if (bj.numHands > 0) {
        // go through each hand
        for (int i = 0; i < bj.numHands; i++) {
            // go through each card in the hand
            ostr << "Hand " << i + 1 << ": ";
            ostr << handsVec[i];
        }
    }
    
    cout << "Result: " << to_currency(bj.profit) << endl;
    cout << "Current Balance: " << to_currency(bj.player->get_balance()) << endl;
    return ostr;
}


void Blackjack::finish() {
    // reset number of Splits flag to 0
    numSplits = 0;
    //Checks if the user has a valid hand because if it does not, then the dealer does not need to draw any cards
    bool userHasValidHand = false;
    for (unsigned i = 0; i < userHands.size(); ++i) {
        if (!userHands[i].isBust() && !userHands[i].isSurrender())
            userHasValidHand = true;
    }
    //Adding first turn condition so that it accounts for blackjack
    if (userHasValidHand && !userHands[0].isFirstTurn()) {
        //Dealer hits card based upon the situation
        while (dealerHand.getValue() <= 16 || (dealerHand.getValue() == 17 && dealerHand.isAce() && dealerHand.isSoft())) {
            hit(dealerHand);
            dealerHand.setFirstTurn();
        }
    }

    // go through all of the player's hand
    // updates the profit based on win/ lose condition
    for (unsigned i = 0; i < userHands.size(); ++i) {
        //player's losing conditions
        if (userHands[i].isSurrender()) {
            profit -= 0.5;
        } 
        else if (userHands[i].isBust()) {
            if (userHands[i].isDouble())
                profit -= 2;
            else
                profit -= 1;
        }
        else if ((!dealerHand.isBust() && (dealerHand.getValue() > userHands[i].getValue()))) {
            if (userHands[i].isDouble())
                profit -= 2;
            else
                profit -= 1;
        }
        
        // if the player hand wins; not busted and greater than dealer's
        if ((dealerHand.isBust() || dealerHand.getValue() < userHands[i].getValue()) && !(userHands[i].isBust()) && !(userHands[i].isSurrender())) {
            if (userHands[i].isDouble()) {
                profit += 2;
            } else if (userHands[i].isFirstTurn() && userHands[i].getValue() == 21) {
                profit += 1.5;
            } else {
                profit += 1;
            }
        }
    }

    //create compare function each of the player's hands
    player->update_balance(profit);
    
}

std::string to_currency(double v)
{
	std::ostringstream ostr;
	
	ostr << std::fixed << std::setprecision(2);
	if (v > 0) {
		ostr << "+$" << v;
	} else if (v < 0) {
		ostr << "-$" << -v;
	}
	else {
		ostr << "$" << v;
	}
	
	return ostr.str();
}

