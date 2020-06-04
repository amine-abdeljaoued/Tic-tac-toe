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
#include "board_functions.c"

//Amine Abdeljaoued and Anya Fries
//https://github.com/amine-abdeljaoued/Tic-tac-toe

int main(int argc, char *argv[]){
    // Set up our socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        fprintf(stderr, "Socket creation failed");
        return 1;
    }

    // set up the information of the server to who we will connect
    struct sockaddr_in ipv4_addr;   
    socklen_t len = (sizeof(struct sockaddr_in));
    memset(&ipv4_addr, 0, len);
    ipv4_addr.sin_family=AF_INET;
    ipv4_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &(ipv4_addr.sin_addr));

    struct sockaddr* serv_addr;
    serv_addr = (struct sockaddr*)&ipv4_addr;

    // to store the info from recvfrom
    struct sockaddr_in server;

    char *msg = malloc(buff_size1);
    int msg_type;

    // Send a message to server to be able to play
    char *hello = "Hello! I want to play tic-tac-toe! \n";
    int s = sendto(sockfd, hello, strlen(hello), 0, serv_addr, sizeof(struct sockaddr_in));
    if (s < 0){
        fprintf(stderr, "Sending failed.");
        return 1;
    }

    // for if we leave a game
    char left[2];
    const int leave = 6;
    left[0] = leave;
    left[1] = '\0';

    while(1){
        memset(msg,0,buff_size1);   // reset msg to all 0s to avoid any problems from previous messages
        int receive = recvfrom(sockfd, msg, buff_size1, 0, (struct sockaddr*)&server, &len);
        if (receive<0){
            fprintf(stderr, "Error receiving message creation failed");
            return 1;
        }
        msg[receive] = '\0';   // end of the message
        msg_type = msg[0];

        if(msg_type == FYI){
            /* Show the state of the board */
            parse_fyi(msg);
        }
        else if(msg_type == TXT){
            /* Display in the console the string contained in msg */
            printf("Message from server: %s\n", msg);   // small comment - maybe need to ignore first number? 
        }
        else if(msg_type == MYM){
            /* Ask user for a new move (column and row) 
             * send a MOV message to the server with the col. and row numbers */
            int row, col;
            printf("Insert column: (255 to quit)\n");
            scanf("%d", &col);
            printf("Insert row: (255 to quit)\n");
            scanf("%d", &row);
            // player wants to leave the game
            if( (row == 255) | (col == 255) ){
                printf("You left the game.\n");
                int s = sendto(sockfd, left, 1, 0, serv_addr, sizeof(struct sockaddr_in));
                if (s < 0){
                    fprintf(stderr, "Sending failed.");
                    return 1;
                }
                break;
            }
            printf("You played (%d, %d)\n", row, col);
            // player sends move
            char move[4];
            move[0] = MOV;
            move[1] = col;
            move[2] = row;
            move[3] = '\0';
            int s = sendto(sockfd, move, 3, 0, serv_addr, sizeof(struct sockaddr_in));
            if (s < 0){
                fprintf(stderr, "Sending failed.");
                return 1;
            }

        }
        else if(msg_type == END){
            /* Indicate winner to user (if any), in msg[1]
             * 0 = draw, 1/2 winning player, 3 for graceful termination
             * Close socket and terminate */
           int game_state = msg[1];
           if (game_state==0){ printf("End of game: This game was not won by anyone! \n"); } // for tie or graceful termination
           else{ printf("Player %d won the game! Good game. \n", game_state); }
           close(sockfd);
           free(msg);
           return 0;
        }
    }
}

/*
Useful to check the hex version of an array:

// to check what we recevied in hexadecimal
        int j;
        printf("< ");
        for (j = 0; j < msg_size + 1; j++) {
        if (j%16 == 0 && j != 0) {
            printf("\n  ");
        }
        printf("%.2x ", msg[j]);
        }
        printf("\n");
// ----------------------------------------

*/