/*
 * config.cpp
 *
 * Parses program arguments and returns Config object
 *
 * Note: change this file to parse program arguments
 *
 * University of Toronto
 * Fall 2019
 */

#include "config.h"
#include "player.h"
#include "shoe.h"
#include "getopt.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

static int
usage(const char * prog) {
    fprintf(stderr, "usage: %s [-h] [-f FILE|-i SEED [-r FILE]] [[-s] -a FILE NUM]\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -h:\tDisplay this message\n");
    fprintf(stderr, " -f:\tUse file-based shoe\n");
    fprintf(stderr, " -i:\tUse random-based shoe (default)\n");
    fprintf(stderr, " -r:\tRecord random-based shoe to file\n");
    fprintf(stderr, " -a:\tPlay automatically using strategy chart\n");
    fprintf(stderr, " -s:\tSilent mode\n");
    fprintf(stderr, " FILE:\tFile name for associated option\n");
    fprintf(stderr, " SEED:\trandom seed\n");
    fprintf(stderr, " NUM:\tnumber of hands to be played\n");
    return -1;
}

int Config::process_arguments(int argc, const char * argv[]) {

    //Print help message
    if (argc == 2 && !strcmp(argv[1], "-h")) {
        return usage(argv[0]);
    }
    
    //Declared flags for different conditions
    int fFlag = 0;
    int iFlag = 0;
    int rFlag = 0;
    int aFlag = 0;

    //Variables set in const char* to be later converted to int
    char* iSeed = nullptr;
    const char* numHandsChar = nullptr;

    //Parsing through user input to set flags for different options
    int c;
    while ((c = getopt(argc, (char *const*) argv, "hf:i:r:sa:")) != -1) {
        switch (c) {
            case 'h':
                return usage(argv[0]);
                break;
            case 'f':
                fFlag = 1;
                shoe_file = optarg;
                break;
            case 'i':
                iFlag = 1;
                iSeed = optarg;
                break;
            case 'r':
                rFlag = 1;
                record_file = optarg;
                break;
            case 's':
                //sFlag = 1;
                silent = true;
                break;
            case 'a':
                aFlag = 1;
                strategy_file = optarg;
                numHandsChar = argv[optind];

                opterr = 0;
                if (numHandsChar == nullptr)
                    fprintf(stderr, "Error: must specify number of hands when playing automatically.\n");
                break;
        }
    }
    
    //There is an error if none of the correct options are selected
    if (fFlag == 0 && iFlag == 0 && rFlag == 0 && silent == false && aFlag == 0)
        return -1;
    
    //Convert char* variables to integers
    stringstream ss;
    ss << numHandsChar;
    ss >> num_hands;
    
    stringstream rs;
    rs << iSeed;
    rs >> random_seed;
    
    //Error checking based on the flags
    if (aFlag == 0 && silent) {
        fprintf(stderr, "Error: silent mode is only available when playing automatically.\n");
        return -1;
    } else if (iFlag == 1 && fFlag == 1) {
        fprintf(stderr, "Error: cannot choose both file and random-based shoe.\n");
        return -1;
    } else if (random_seed < 0 && iFlag == 1) {
        fprintf(stderr, "Error: SEED must be a non-negative integer.\n");
        return -1;
    } else if (rFlag == 1 && fFlag == 1) {
        fprintf(stderr, "Error: recording is only available for random-based shoe.\n");
        return -1;
    } else if (rFlag == 1 && fFlag == 1) {
        fprintf(stderr, "Error: The r option is only available if the f option is not chosen.\n");
        return -1;
    } else if ((floor(num_hands) != num_hands || num_hands <= 0) && aFlag == 1) {
        fprintf(stderr, "Error: NUM must be a natural number.\n");
        return -1;
    }
    
    player = Player::factory(this);
    if (player == nullptr) {
        fprintf(stderr, "Error: cannot instantiate Player. (bad file?)\n");
        return usage(argv[0]);
    }

    shoe = Shoe::factory(this);
    if (shoe == nullptr) {
        fprintf(stderr, "Error: cannot instantiate Shoe. (bad file?)\n");
        return usage(argv[0]);
    }

    return 0;
}

Config::~Config() {
    if (player != nullptr)
        delete player;

    if (shoe != nullptr)
        delete shoe;
}


