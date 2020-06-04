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

//Amine Abdeljaoued and Anya Fries
//https://github.com/amine-abdeljaoued/Tic-tac-toe

extern int errno ;


int main(int argc, char *argv[]){
    
    if(argc!=2){
        printf("Wrong syntax\n");
        return 1;
    }
    
    // Set up our server
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0){
        printf("Error opening the socket\n");
        return 1;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in)); // To avoid having "garbage" from any previous actions, clean it.
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port=htons(atoi(argv[1]));

    struct sockaddr* server;
    server = (struct sockaddr*)&server_addr;

    if(bind(sockfd,server,sizeof(struct sockaddr_in)) == -1){
        printf("Could not bind socket\n");
        return 1;
    }

    // Set up for players
    int n = 0;  // Numbers of players
    char *receive_buf = malloc(buff_size1);
    
    // Greeting messages
    char g1[] = " Welcome! You are player 1 in game, you play with X \n";
    char g2[] = " Welcome! You are player 2 in game, you play with 'O' \n"; 
    g1[0]=TXT;
    g2[0]=TXT;

    // Initialise other buffers
    char mym[] = "  ";                                  // Buffer for MYM
    mym[0]=MYM;
    char bm[] = " Invalid move, please try again! \n";  // Buffer for an invalid move
    bm[0]=TXT;

    // Where we store our game
    Game game;
    printf("Waiting for connections...\n");
    // where we store our players and their sizes
    struct sockaddr clients[2];
    socklen_t lens[2];

    // Assign our two players
    while( n < 2 ){ 

        int s;
        if(n==1){
            struct sockaddr client2; // Address of client
            lens[1] = sizeof(client2);
            int play = recvfrom(sockfd, receive_buf, buff_size1, 0, &client2, &lens[1]);
            if (play<=0){
                fprintf(stderr,"Error receiving joining msg\n");
                continue;
            }
            clients[1] = client2;
            s = sendto(sockfd, g2,strlen(g2),0,&clients[1],lens[1]);
            n+=1;
            printf("Player 2 assigned.\n");
        }
        if(n==0){
            struct sockaddr client1; // Address of client
            lens[0] = sizeof(client1);
            int play = recvfrom(sockfd, receive_buf, buff_size1, 0, &client1, &lens[0]);
            if (play<=0){
                fprintf(stderr,"Error receiving joining msg\n");
                continue;
            }
            clients[0] = client1;

            s = sendto(sockfd, g1,strlen(g1),0,&clients[0],lens[0]);
            n+=1;
            printf("Player 1 assigned.\n");
        }
        if (s<0){
            fprintf(stderr,"Error sending greeting msg\n");
            return 1;
        }
        memset(receive_buf,0,buff_size1);
    }

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

    // We can start the main game loop
    while(game.play){
        // 1. Send FYI msg
        memset(receive_buf,0,buff_size1);
        char* fyi_msg = create_fyi(board);
        int s = sendto(sockfd, fyi_msg, 3*fyi_msg[1] + 2, 0, &clients[game.turn], lens[game.turn]);
        
        if (s<0){ 
            fprintf(stderr,"Error sending FYI msg %d\n", errno);
            return 1;
        }

        // 2. Send MYM, get MOV
        int valid = 0; // as long as the move received is invalid we ask for another one
        while (!valid) {
            int s = sendto(sockfd, mym, 1, 0, &clients[game.turn],lens[game.turn]);
            if (s<0){ 
                fprintf(stderr,"Error sending MYM msg\n");
                return 1;
            }
            struct sockaddr* receive = NULL;

            // note here a third party could interrupt the game
            // we are not prepared for this !
            int r = recvfrom(sockfd, receive_buf, buff_size1, 0, receive, &lens[game.turn]);
            if (r<0){ 
                fprintf(stderr,"Error receiving msg\n");
                return 1;
            }
            // now we look at what we got: is it a valid MOV?
            receive_buf[r] = '\0';
            valid = valid_mov(receive_buf, board);
            if (!valid){
                int s = sendto(sockfd, bm,strlen(bm),0,&clients[game.turn],lens[game.turn]);
                if (s<0){ 
                    fprintf(stderr,"Error sending bad move TXT msg\n");
                    return 1;
                }
            }
        }
    
        // 3. Our move can be inserted
        int col = receive_buf[1];
        int row = receive_buf[2];
        printf("Move: (%d, %d)\n", row, col);
        board[row][col] = game.char_rep[game.turn];
        printf("Move has been placed.\n\n");

        // 4. Check for END
        int term = check_terminated(board);
        printf("Terminated: %d\n", term);
        if (term != -1){
            printf("Game ended.\n");
            char end[3];
            end[0] = END;
            end[1] = term;
            end[2] = '\0';
            for (int i = 0; i<2; i++){  // tell both players the game is done
                int s = sendto(sockfd, end,3,0,&clients[i],lens[game.turn]);
                if (s<0){ 
                    fprintf(stderr,"Error sending END msg\n");
                    return 1;
                }
            }
            game.play = 0; 
        }
        // switch players 
        game.turn = game.turn ^ 1; 
    }
    free(receive_buf);

    return 0;
}