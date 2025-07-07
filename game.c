#include <stdio.h>
#include "game.h"

void initBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = '.';
}

void printBoard(char board[SIZE][SIZE]) {
    printf("   ");
    for (int j = 0; j < SIZE; j++)
        printf("%2d", j);
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%2d ", i);
        for (int j = 0; j < SIZE; j++) {
            printf("%2c", board[i][j]);
        }
        printf("\n");
    }
}

int placePiece(char board[SIZE][SIZE], int x, int y, char player) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || board[x][y] != '.') {
        return 0;  // 无效落子
    }
    board[x][y] = player;
    return 1;
}

int countDirection(char board[SIZE][SIZE], int x, int y, int dx, int dy, char player) {
    int count = 0;
    for (int step = 1; step < 5; step++) {
        int nx = x + step * dx;
        int ny = y + step * dy;
        if (nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE)
            break;
        if (board[nx][ny] == player)
            count++;
        else
            break;
    }
    return count;
}

int checkWin(char board[SIZE][SIZE], int x, int y) {
    char player = board[x][y];
    if (player == '.') return 0;

    int dx[] = {0, 1, 1, 1};
    int dy[] = {1, 0, 1, -1};

    for (int d = 0; d < 4; d++) {
        int count = 1;
        count += countDirection(board, x, y, dx[d], dy[d], player);
        count += countDirection(board, x, y, -dx[d], -dy[d], player);
        if (count >= 5)
            return 1;
    }
    return 0;
}
