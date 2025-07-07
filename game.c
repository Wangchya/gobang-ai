#include "game.h"

int count_dir(char board[GRID_SIZE][GRID_SIZE], int x, int y, int dx, int dy, char player) {
    int count = 0;
    for (int step = 1; step < 5; ++step) {
        int nx = x + step * dx;
        int ny = y + step * dy;
        if (nx < 0 || ny < 0 || nx >= GRID_SIZE || ny >= GRID_SIZE)
            break;
        if (board[nx][ny] == player)
            count++;
        else
            break;
    }
    return count;
}

int check_win(char board[GRID_SIZE][GRID_SIZE], int x, int y) {
    char player = board[x][y];
    int dx[] = {1, 0, 1, 1};
    int dy[] = {0, 1, 1, -1};

    for (int d = 0; d < 4; ++d) {
        int count = 1;
        count += count_dir(board, x, y, dx[d], dy[d], player);
        count += count_dir(board, x, y, -dx[d], -dy[d], player);
        if (count >= 5)
            return 1;
    }
    return 0;
}
