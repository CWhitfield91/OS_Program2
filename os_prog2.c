#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define NUM_THREADS 4

void initializeDeck();
void shuffleDeck();
void* playPairWar(void*);

int deck[52];  //card deck
pthread_t players[NUM_THREADS];  //player threads
pthread_mutex_t deckLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condi = PTHREAD_COND_INITIALIZER;

int turn = 0;
int topOfTheDeck = 0;
bool dealerDealt = false;
bool win = false;
int ROUND = 1;
bool changed = false;
typedef struct playerHand {  //the struct for hand of each player
    int card1;
    int card2;
} PLAYER_HAND;

PLAYER_HAND player1;
PLAYER_HAND player2;
PLAYER_HAND player3;
void playHand(int);
bool done = false;
int wins = 0;
int seed = 0;
FILE *output;

int main(int argc, char *argv[]) {

    output = fopen("output.txt", "w+");

    seed = atoi(argv[1]);
    srand(seed);
    
    initializeDeck();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    long t;
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_create(&players[t], &attr, playPairWar, (void *)t);
    }
    
    long i;
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(players[i], NULL);
    }

    fclose(output);

    return 0;
}

void initializeDeck() {
    int placeInDeck = 0;
    long i;
    for(i = 0; i < 4; i++) {
        long j;
        for(j = 1; j < 14; j++) {
            deck[placeInDeck] = j;
            placeInDeck++;
        }
    }
}

void shuffleDeck() {
    
    int placeInDeck = 0;
	int temp;
	int x;

    long i;
	for (i = 0; i < 52; i++) {
		x = rand() % 52;
		if (x != placeInDeck) {
			temp = deck[placeInDeck];
			deck[placeInDeck] = deck[x];
			deck[x] = temp;
		}
		placeInDeck++;
	}
}

void* playPairWar(void* tid) {
    long pid = (long)tid; 
    while(done == false){
        pthread_mutex_lock(&deckLock);
            while(pid != turn && win == false) {
                pthread_cond_wait(&condi, &deckLock);
            }
            win = false;
            if(wins == 3) {
                done = true;
                win = true;
                
            }
        while(pid == turn && win == false){

            if(win == false) {
                //printf("Hello from thread %li\n", pid);
                if(pid == 0 && dealerDealt == false) {
                    printf("Dealer deals new hand \n");
                    shuffleDeck();
                    fprintf(output, "DEALER: shuffles\n");
                    player1.card1 = deck[topOfTheDeck];
                    deck[topOfTheDeck] = 0;
                    topOfTheDeck++;

                    player2.card1 = deck[topOfTheDeck];
                    deck[topOfTheDeck] = 0;
                    topOfTheDeck++;

                    player3.card1 = deck[topOfTheDeck];
                    deck[topOfTheDeck] = 0;
                    topOfTheDeck++;

                    dealerDealt = true;
                    turn = ROUND;
                }
                else if(pid == 1) {
                    fprintf(output, "PLAYER 1: hand %i\n", player1.card1);
                    player1.card2 = deck[topOfTheDeck];
                    topOfTheDeck++;

                    printf("Player 1 draws a card.\n");
                    fprintf(output, "PLAYER 1: draws %i\n", player1.card2);

                    if(player1.card1 == player1.card2) {
                        printf("PLAYER 1 WINS! \n");
                        printf("HAND %i %i\n", player1.card1, player1.card2);
                        printf("Player 2 hand: %i. WIN no.\n", player2.card1);
                        printf("Player 3 hand: %i. WIN no.\n", player3.card1);
                        printf("Deck: ");

                        long i;
                        for(i = 0; i < 52; i++) {
                            if(i == topOfTheDeck - 1) {
                                printf("[%i]", deck[i]);
                            }
                            else{
                                printf("%i ", deck[i]);
                            }
                        }
                        printf("\n");
                        fprintf(output, "PLAYER 1: wins\n");
                        fprintf(output, "PLAYER 1: exits round\n");
                        fprintf(output, "PLAYER 2: exits round\n");
                        fprintf(output, "PLAYER 3: exits round\n");

                        deck[0] = player1.card1;
                        deck[1] = player2.card1;
                        deck[2] = player3.card1;
                        win = true;
                        ROUND++;
                        wins++;
                        dealerDealt = false;
                        topOfTheDeck = 0;
                        turn = 0;
                    }
                    else {
                        printf("Player 1 hand: %i %i. WIN no.\n", player1.card1, player1.card2);
                        fprintf(output, "PLAYER 1: discards %i\n", player1.card2);

                        long i;
                        for(i = 0; i < 52; i++) {
                            if(i == topOfTheDeck - 1) {
                                fprintf(output, "[%i]", deck[i]);
                            }
                            else{
                                fprintf(output, "%i ", deck[i]);
                            }
                        }

                        turn++;
                        if(turn >3) {
                            turn = 0;
                        } 
                    }
                }
                else if(pid == 2) {
                    fprintf(output, "PLAYER 2: hand %i\n", player2.card1);
                    player2.card2 = deck[topOfTheDeck];
                    topOfTheDeck++;

                    printf("Player 2 draws a card.\n");
                    fprintf(output, "PLAYER 2: draws %i\n", player2.card2);
                    if(player2.card1 == player2.card2) {
                        printf("PLAYER 2 WINS! \n");
                        printf("HAND %i %i\n", player2.card1, player2.card2);
                        printf("Player 1 hand: %i. WIN no.\n", player1.card1);
                        printf("Player 3 hand: %i. WIN no.\n", player3.card1);
                        printf("Deck: ");

                        long i;
                        for(i = 0; i < 52; i++) {
                            if(i == topOfTheDeck - 1) {
                                printf("[%i]", deck[i]);
                            }
                            else{
                                printf("%i ", deck[i]);
                            }
                        }
                        printf("\n");

                        fprintf(output, "PLAYER 2: wins\n");
                        fprintf(output, "PLAYER 2: exits round\n");
                        fprintf(output, "PLAYER 1: exits round\n");
                        fprintf(output, "PLAYER 3: exits round\n");

                        deck[0] = player1.card1;
                        deck[1] = player2.card1;
                        deck[2] = player3.card1;
                        win = true;
                        ROUND++;
                        wins++;
                        dealerDealt = false;
                        topOfTheDeck = 0;
                        turn = 0;
                    }
                    else {
                        printf("Player 2 hand: %i %i. WIN no.\n", player2.card1, player2.card2);
                        fprintf(output, "PLAYER 2: discards %i\n", player2.card2);

                        long i;
                        for(i = 0; i < 52; i++) {
                            if(i == topOfTheDeck - 1) {
                                fprintf(output, "[%i]", deck[i]);
                            }
                            else{
                                fprintf(output, "%i ", deck[i]);
                            }
                        }

                        turn++;
                        if(turn >3) {
                            turn = 0;
                        } 
                    }
                }
                else if(pid == 3){
                    fprintf(output, "PLAYER 3: hand %i\n", player3.card1);
                    player3.card2 = deck[topOfTheDeck];
                    topOfTheDeck++;

                    printf("Player 3 draws a card.\n");
                    fprintf(output, "PLAYER 3: draws %i\n", player3.card2);
                    if(player3.card1 == player3.card2) {
                        printf("PLAYER 3 WINS! \n");
                        printf("HAND %i %i\n", player3.card1, player3.card2);
                        printf("Player 1 hand: %i. WIN no.\n", player1.card1);
                        printf("Player 2 hand: %i. WIN no.\n", player2.card1);
                        printf("Deck: ");

                        long i;
                        for(i = 0; i < 52; i++) {
                            if(i == topOfTheDeck - 1) {
                                printf("[%i]", deck[i]);
                            }
                            else{
                                printf("%i ", deck[i]);
                            }
                        }
                        printf("\n");
                        fprintf(output, "PLAYER 3: wins\n");
                        fprintf(output, "PLAYER 3: exits round\n");
                        fprintf(output, "PLAYER 1: exits round\n");
                        fprintf(output, "PLAYER 2: exits round\n");

                        deck[0] = player1.card1;
                        deck[1] = player2.card1;
                        deck[2] = player3.card1;
                        win = true;
                        ROUND++;
                        wins++;
                        dealerDealt = false;
                        topOfTheDeck = 0;
                        turn = 0;
                    }
                    else {
                        printf("Player 3 hand: %i %i. WIN no.\n", player3.card1, player3.card2);
                        fprintf(output, "PLAYER 3: discards %i\n", player3.card2);

                        long i;
                        for(i = 0; i < 52; i++) {
                            if(i == topOfTheDeck - 1) {
                                fprintf(output, "[%i]", deck[i]);
                            }
                            else{
                                fprintf(output, "%i ", deck[i]);
                            }
                        }

                        turn++;
                        if(turn > 3) {
                            turn = 0;
                        } 
                    }
                }
                else {
                    turn++;
                }
            }
            else {
                
            }
        }
        pthread_cond_broadcast(&condi);
        pthread_mutex_unlock(&deckLock);
    }

    pthread_exit(NULL);

}