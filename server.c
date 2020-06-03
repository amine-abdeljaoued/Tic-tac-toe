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
#include <stdbool.h> 

#include "board_functions.c"


int main(int argc, char *argv[]){
    
    if(argc!=2){
        printf("Wrong syntax");
        return 1;
    }
    
    // Set up our server
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0){
        printf("Error opening the socket");
        return 1;
    }
    socklen_t len = (sizeof(struct sockaddr_in));
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, len); // To avoid having "garbage" from any previous actions, clean it.
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port=htons(atoi(argv[1]));

    struct sockaddr* server;
    server = (struct sockaddr*)&server_addr;

    if(bind(sockfd,server,len) == -1){
        printf("Could not bind socket");
        return 1;
    }

    // Set up for players
    int n = 0;                      // Numbers of players
    char *receive_buf = malloc(buff_size1);
    
    // Greeting messages
    char g1[] = " Welcome! You are player 1 in game, you play with X \0";
    char g2[] = " Welcome! You are player 2 in game, you play with O \0"; 
    g1[0]=TXT;
    g2[0]=TXT;
    //char *g1 = malloc(strlen(greeting_msg1));
    //g1 = greeting_msg1; 
    //*g1=TXT;
    //memcpy(g1+1, greeting_msg1, strlen(greeting_msg1));
    //char *g2 = malloc(strlen(greeting_msg2));
    //g2 = greeting_msg2;
    //*g2 = TXT;
    //memcpy(g2+1, greeting_msg2, strlen(greeting_msg2));

    // Where we store our game
    Game game;
    printf("Waiting for connections...\n");

    struct sockaddr_in *client_addr1 = malloc(sizeof(struct sockaddr_in)); // Address of client
    struct sockaddr_in *client_addr2 = malloc(sizeof(struct sockaddr_in)); // Address of client


    // Assign our two players
    while( n < 2 ){ 

        int s;
        if(n==1){

            int play = recvfrom(sockfd, receive_buf, buff_size1, 0, (struct sockaddr*)&client_addr2, &len);
            if (play<=0){
                fprintf(stderr,"Error receiving joining msg");
                continue;
            }
            

            game.players[1].address = (struct sockaddr*) &client_addr2;
            s = sendto(sockfd, g2,strlen(g2),0,game.players[1].address,len);
            n+=1;
            printf("Player 2 assigned.\n");
        }
        if(n==0){
            int play = recvfrom(sockfd, receive_buf, buff_size1, 0, (struct sockaddr*)&client_addr1, &len);
            if (play<=0){
                fprintf(stderr,"Error receiving joining msg");
                continue;
            }
            
            
            game.players[0].address = (struct sockaddr*) &client_addr1;
            s = sendto(sockfd, g1,strlen(g1),0,game.players[0].address,len);
            n+=1;
            printf("Player 1 assigned.\n");
        }

        if (s<0){
        fprintf(stderr,"Error sending greeting msg");
        return 1;
        }
        memset(receive_buf,0,buff_size1);
/*         if (game.players[0].address == game.players[1].address){
            printf("we have the same address\n");
        } */
    }

    //free(g1);
    //free(g2);

    // initialise our game
    char board[3][3] = {            // Our board
        {' ',' ',' '},
        {' ',' ',' '},
        {' ',' ',' '}
    };              
    game.turn = 0;              // Player 1 starts
    game.play = 1;              // Game is ready to start
    game.char_rep[0] = 'X';     // Char representation of each player
    game.char_rep[1] = 'O';

    //Initialise buffers
    char mym[] = "  ";                                  // Buffer for MYM
    mym[0]=MYM;
    //*mym = MYM;
    char bm[] = "Invalid move, please try again! \n";  // Buffer for an invalid move
    bm[0]=TXT;
    //char* bm = malloc(strlen(bad_move + 1));
    //*bm = TXT;
    //memcpy(bm+1, bad_move, strlen(bad_move));
    //char *fyi_msg;                     // Buffer for FYI

    // We can start the main game loop
    while(game.play){
        // 1. Send FYI msg
        //memset(fyi_msg,0,buff_size1);   // reset to avoid problems
        memset(receive_buf,0,buff_size1);
        char* fyi_msg = create_fyi(board);
        int s = sendto(sockfd, fyi_msg, 3*fyi_msg[1] + 3, 0, game.players[game.turn].address, len);
        if (s<0){ 
            fprintf(stderr,"Error sending FYI msg\n");
            return 1;
        }

        // 2. Send MYM, get MOV
        int valid = 0; // as long as the move received is invalid we ask for another one
        while (!valid) {
            int s = sendto(sockfd, mym, 1, 0, game.players[game.turn].address,len);
            if (s<0){ 
                fprintf(stderr,"Error sending MYM msg");
                return 1;
            }
            struct sockaddr* receive = NULL;

            // note here a third party could interrupt the game
            // we are not prepared for this !
            int r = recvfrom(sockfd, receive_buf, buff_size1, 0, receive, &len);
            if (r<0){ 
                fprintf(stderr,"Error receiving msg");
                return 1;
            }
            // now we look at what we got: is it a valid MOV?
            receive_buf[r] = '\0';
            valid = valid_mov(receive_buf, board);
            if (!valid){
                int s = sendto(sockfd, bm,strlen(bm),0,game.players[game.turn].address,len);
                if (s<0){ 
                    fprintf(stderr,"Error sending bad move TXT msg");
                    return 1;
                }
            }
        }
    
        // 3. Our move can be inserted
        int col = receive_buf[1];
        int row = receive_buf[2];
        printf("Move: (%d, %d)\n", row, col);
        board[row][col] = game.char_rep[game.turn];
        printf("Move has been placed.\n");

        // 4. Check for END
        int term = check_terminated(board);
        printf("term = %d\n", term);
        if (term != -1){
            char end[3];
            end[0] = END;
            end[1] = term;
            end[2] = '\0';
            for (int i = 0; i<2; i++){  // tell both players the game is done
                int s = sendto(sockfd, end,3,0,game.players[i].address,len);
                if (s<0){ 
                    fprintf(stderr,"Error sending END msg");
                    return 1;
                }
            }
            game.play = 0; 
        }
        // switch players 
        game.turn = game.turn ^ 1; 
    }

    //free(bm);
    //free(mym);
    //free(fyi_msg);
    free(receive_buf);

    // for some reason we can't free the follow two: (they're not seen as on the heap?? super confused)
    // probably linked to how I defined them earlier 
    // and if we get the two different clients to be diff it will be okay

    //free(game.players[0].address);
    //free(game.players[1].address);

    return 0;
}