#ifndef GAME_H
#define GAME_H

#define SIZE 15

void initBoard(char board[SIZE][SIZE]);
void printBoard(char board[SIZE][SIZE]);
int placePiece(char board[SIZE][SIZE], int x, int y, char player);
int checkWin(char board[SIZE][SIZE], int x, int y);

#endif
