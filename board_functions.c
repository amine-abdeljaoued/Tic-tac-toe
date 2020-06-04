#include "board_functions.h"

//Rules: 
//Player 1 has the 'X'
//Player 2 has the 'O'
//The board will only be stored on the server

//Function on board:

//parse_fyi : shows a 3x3 board with 'X' and 'O' from a buffer containing a FYI message
//  show : Shows board to user

//create_fyi : given a board, send fyi message

//valid_mov : returns 1 if the move is valid

//update_mov : update a given board with a MOV 

//check_terminated : check if game is terminated


void show(char board [3][3]){
    /* Displays the state of the board given an array of chars of 'X', 'O' and ' ' */
    printf("\n");
    printf("    %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
    printf("   ---+---+--- \n");
    printf("    %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
    printf("   ---+---+--- \n");
    printf("    %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
    printf("\n");
}


void parse_fyi(char *fyi_msg){
    /*
     * Given a FYI message, display the state of the board
     * Structure of FYI msg:
     * +--------+--------+--------+--------+--------+--- ... ---+--------+--------+--------+
     * | FYI    | n      | play_1 | col_1  | row_1  |           | play_n | col_n  | row_n  |
     * +--------+--------+--------+--------+--------+--- ... ---+--------+--------+--------+
     * 
     * Sends filled positions in the board. n indicates the number of filled positions; 
     * for each position (from 1 to n), the player (1 or 2), the column (0, 1, 2) and 
     * the row (0, 1, 2) are indicated
     * 
     * We display content of the board
     */

    char board[3][3] = {
        {' ',' ',' '},
        {' ',' ',' '},
        {' ',' ',' '}
    };
    int n = fyi_msg[1];
    int row, col, player;
    for(int i = 0; i<n; i++){
        player = fyi_msg[2 + 3*i];
        col = fyi_msg[3 + 3*i];
        row = fyi_msg[4 + 3*i];
        if(player==1){
            board[row][col]='X';
        }
        else{
            board[row][col]='O';
        }
    }
    printf("\n");
    printf("%d filled positions.\n", n);
    show(board);
}

char *create_fyi(char board[3][3]){
    
    int n=0,a=0,b=0;
    int player1[20];
    int player2[20];
    for(int z = 0;z<20;z++){
        player1[z]=-1;
        player2[z]=-1;
    }
    for(int i = 0;i<3;i++){
        for(int j = 0;j<3;j++){
            char play=board[i][j];
            if(play==' '){
                continue;
            }
            else{
                n++;
                if(play=='X'){
                    player1[a]=j;
                    player1[a+1]=i;
                    a+=2;
                }
                else{
                    player2[b]=j;
                    player2[b+1]=i;
                    b+=2;
                }
            }
        }
    }
    int length = n*3 + 3; // 3 and not 2 for the '\0'
    char *fyi_msg = malloc(length);
    *fyi_msg = FYI;
    *(fyi_msg+1) = n;
    
    //Then addind colums and rows
    int z = 0, index = 2;
    while( (player1[z]!= -1) && (player1[z+1]!= -1) ){
        fyi_msg[index]=1;
        fyi_msg[index+1]=player1[z];
        fyi_msg[index+2]=player1[z+1];
        index +=3;
        z+=2;
    }
    z = 0;
    while( (player2[z]!= -1) && (player2[z+1]!= -1) ){
        fyi_msg[index]=2;
        fyi_msg[index+1]=player2[z];
        fyi_msg[index+2]=player2[z+1];
        index +=3;
        z+=2;
    }
    fyi_msg[length-1]='\0';
    return fyi_msg;
}

int valid_mov(char *mov_msg, char board[3][3]){
    int col = mov_msg[1];
    int row = mov_msg[2];
    if( (col<3)&&(col>=0) ){
        if( (row<3)&&(row>=0) ){
            if (board[row][col] == ' '){ return 1; }
        }
    }
    return 0;

}

void update_mov(char (*board)[3], char*mov_msg, int player){
    int j = mov_msg[1];
    int i = mov_msg[2];
    
    if(player==1){
        board[i][j]='X';
    }
    else{
        board[i][j]='O';
    }
}  

int check_terminated(char (*board)[3]){
    int n=0; //Number of plays
    int p1[2]={0,0};
    int p2[2]={0,0};

    // Check if a player has completed a row or column
    for(int i=0;i<3;i++){
        for(int j=0; j<3; j++){
            if(board[i][j]=='X'){
                p1[0]+=1; //Checking i-th row
            }
            if(board[j][i]=='X'){
                p1[1]+=1; // Checking i-th column
            }
            if(board[i][j]=='O'){
                p2[0]+=1;
            }
            if(board[j][i]=='O'){
                p2[1]+=1;
            }
        }
        if((p1[0]==3) | (p1[1]==3)){
            return 1;
        }
        if((p2[0]==3) | (p2[1]==3)){
            return 2;
        }
        p1[0]=0, p1[1]=0;
        p2[0]=0, p2[1]=0;
    }
    // Check diagonals
    if(board[1][1] != ' '){
        if(board[1][1]=='X'){
            if((board[0][0]=='X')&&(board[2][2]=='X')){
                return 1;
            }
            if((board[0][2]=='X')&&(board[2][0]=='X')){
                return 1;
            }
        else{
            if((board[0][0]=='O')&&(board[2][2]=='O')){
                return 2;
            }
            if((board[0][2]=='O')&&(board[2][0]=='O')){
                return 2;
            }

        }
        }
    }
    // Check for draw
    for(int i = 0;i<3;i++){
        for(int j = 0;j<3;j++){
            if(board[i][j]!=' '){
                n+=1;
            }
        }
    }
    if(n==9){
        return 0;
    }
    return -1; //Not finished
}