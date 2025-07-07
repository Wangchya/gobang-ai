#include <stdio.h>
#include "game.h"

int main() {
    char board[SIZE][SIZE];
    initBoard(board);

    int turn = 0;
    int x, y;
    char player;

    while (1) {
        printBoard(board);
        player = (turn % 2 == 0) ? 'X' : 'O';
        printf("Player %c, enter x y: ", player);
        scanf("%d %d", &x, &y);

        if (!placePiece(board, x, y, player)) {
            printf("Invalid move! Try again.\n");
            continue;
        }

        if (checkWin(board, x, y)) {
            printBoard(board);
            printf("Player %c wins!\n", player);
            break;
        }

        turn++;
        if (turn == SIZE * SIZE) {
            printf("It's a draw!\n");
            break;
        }
    }

    return 0;
}

