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

//Understand exact synthax of messages

//Rules: 
//Player 1 has the 'X'
//Player 2 has the 'O'
//The board will only be stored on the server

//This file will be renamed in .h, or split into .c .h ...


//Function on board:

//parse_fyi : shows a 3x3 board with 'X' and 'O' from a buffer containing a FYI message
//  show : Shows board to user

//create_fyi : given a board, send fyi message

//valid_move : returns 1 if the move is valid

//update_mov : update a given board with a MOV 

//check_terminated : check if game is terminated


void show(char board [3][3]){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            printf("%c",board[i][j]);
        }
        printf("\n");
    }
}


void parse_fyi(char *fyi_msg){
    char board[3][3] = {
        {'-','-','-'},
        {'-','-','-'},
        {'-','-','-'}
    };
    int n = fyi_msg[4] - '0';
    int x, y, player;
    for(int i = 0;i<n;i++){
        y = fyi_msg[8+6*i] - '0';
        x = fyi_msg[10+6*i] - '0';
        player = fyi_msg[6+6*i] - '0';
        if(player==1){
            board[x][y]='X';
        }
        else{
            board[x][y]='O';
        }
    }
    show(board);
}






int main(){ //For testing


    //printf("%d",board[0][1]);
    char fyi_msg[30]="FYI 3 1 2 2 1 2 1 2 0 0";
    parse_fyi(fyi_msg);
    return 0;
}