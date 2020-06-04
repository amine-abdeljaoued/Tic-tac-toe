#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//Amine Abdeljaoued and Anya Fries
//https://github.com/amine-abdeljaoued/Tic-tac-toe

/*
 * How the structure of messages in general works
 * For example we can consider:
                 +--------+--------+
   char msg[2] = | END    | winner |
                 +--------+--------+
 * This means that for msg we have 
 * msg[0] = 3 = END
 * msg[1] = 1 or 2 (whichever player won)
 * The structures show are entries in an array
 */   

// from server to client
#define TXT 0x04     
#define MYM 0x02    
#define END 0x03
#define FYI 0x01      

// from client to server
#define MOV 0x05
#define LFT 0x06

// buffer length, will be the same for client and server 
#define buff_size1 100

typedef struct Game{
    // player 1 is turn 0 and players[0]
    // player 2 is turn 1 and players[1]
    int turn;
    int play;
    char char_rep[2];
} Game;

char* msgtype[] = {"FYI", "MYM", "END", "TXT", "MOV", "LFT"};

void show(char board [3][3]);   // displays the board
void parse_fyi(char *fyi_msg);  // parses an FYI message
int valid_mov(char *mov_msg, char board[3][3]);
void update_mov(char (*board)[3], char*mov_msg, int player);
int check_terminated(char (*board)[3]);