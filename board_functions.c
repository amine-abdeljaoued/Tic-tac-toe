#include "board_functions.h"

//Understand exact syntax of messages

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
    printf("%d filled positions.\n", n);
    show(board);
}
