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


int main(int argc, char *argv[]){
    
    if(argc!=2){
        printf("Wrong syntax");
        return 1;
    }
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0){
        printf("Error opening the socket");
        return 1;
    }
    socklen_t len = (sizeof(struct sockaddr_in));
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, len); //To avoid having "garbage" from any previous actions, clean it.

    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    server_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sockfd,(struct sockaddr * ) &server_addr,len) == -1){
        printf("Could not bind socket");
        return 1;
    }

    //We start here
    int n = 0; //Numbers of players
    struct sockaddr_in client_addr; //Address of client
    memset(&client_addr, 0, len);
    char *receive_buf = malloc(buff_size1);
    char *greeting_msg1 = "Welcome! You are player 1 in game, you play with X";
    char *greeting_msg2 = "Welcome! You are player 2 in game, you play with O"; 
    Player player1, player2;

    printf("Waiting for connections...\n");
    while(n<2){
        int play = recvfrom(sockfd, receive_buf, buff_size1, 0, (struct sockaddr*)&client_addr, &len);
        sendto(sockfd, greeting_msg2,strlen(greeting_msg2),0,(struct sockaddr*)&client_addr,len);

        printf("received");
        if(play>0){
            if(n==1){
                
                player2.address = (struct sockaddr*)&client_addr;
                sendto(sockfd, greeting_msg2,strlen(greeting_msg2),0,player2.address,len);
                n++;
                printf("Player 2 assigned.");
            }
            if(n==0){
                
                player1.address = (struct sockaddr*)&client_addr;
                sendto(sockfd, greeting_msg1,strlen(greeting_msg1),0,player1.address,len);
                n++;
                printf("Player 1 assigned.");

            }

        memset(&client_addr, 0, len);
        receive_buf-=play;
        }
    }

    char board[3][3] = {
        {' ',' ',' '},
        {' ',' ',' '},
        {' ',' ',' '}
    }; //Our board
    
    int turn = 1; //Player 1 starts
    char mym=MYM;
    //Then here we have our 2 players and we can start the main game loop
/*     while(1){

        char* fyi_msg = create_fyi(board);
        sendto(sockfd, fyi_msg,strlen(greeting_msg1),0,player1.address,len);
        sendto(sockfd, MYM ,1,0,player1.address,len);
        recvfrom(sockfd, receive_buf, buff_size1, 0, (struct sockaddr*)&client_addr, &len);
    } */

    //Don't forget to free buffers

    
/*     //memset
    int x; //msg size
    struct sockaddr_in client;
    memset(&client, 0, len);
    while(1){
        x  = recvfrom(sockfd, receive_buf, buff_size, 0, (struct sockaddr*)&client,&len);

        printf("server received : %s\n",receive_buf);

        x = sendto(sockfd, receive_buf, buff_size, 0, (struct sockaddr*)&client,len);

    }
 */

    return 0;
}