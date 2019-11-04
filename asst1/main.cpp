/*
 * main.cpp
 *
 * Main function for Easy Blackjack
 *
 * Note: you may not change this file
 *
 * University of Toronto
 * Fall 2019
 */
 
#include "easybj.h"
#include "config.h"
#include "player.h"
#include "shoe.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;

static void player_summary(Player * player);

int 
main(int argc, const char * argv[])
{
        // parse through the command line and obtain info
        // set up shoe file and player info
	Config config;

	if (config.process_arguments(argc, argv) < 0)
		return EXIT_FAILURE;
        
        // while there's still cards in the deck, keep going
	while (!config.shoe->over()) {
                // takes the player and shoe from config
                // config.player is either manual or auto
		Blackjack game(config.player, config.shoe);
                
                // start the game; object is Hand NOT Blackjack
                // first two cards go to the dealer, the next 2 go to the player
		Hand * hand = game.start();
		const Hand * dealer = game.dealer_hand();

		while (hand != nullptr) {
                    //go to the auto or manual player
			config.player->play(hand, dealer);
			hand = game.next();
		}
		// dealer plays here
		game.finish();	
		if (!config.silent)
			std::cout << game;	
		
		if (!config.player->again())
			break;
	}
	
	player_summary(config.player);
	return EXIT_SUCCESS;
}

static void 
player_summary(Player * player)
{
	int hands_played;
	double balance;
	double advantage;
	
	hands_played = player->get_hands_played();
	std::cout << "Hands Played: " << hands_played << std::endl;
	
	if (hands_played <= 0)
		return;

	balance = player->get_balance();
	advantage = balance / player->get_hands_played();
	std::cout << "Final Balance: " << to_currency(balance) << std::endl;
	std::cout << "Player Advantage: " << std::setprecision(4) 
			  << advantage*100 << "%" << std::endl;
}

