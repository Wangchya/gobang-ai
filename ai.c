#include "ai.h"
#include <stdlib.h>

int score_point(char board[GRID_SIZE][GRID_SIZE], int x, int y, char ai, char player) {
    int score = 0;
    int dx[] = {1, 0, 1, 1};
    int dy[] = {0, 1, 1, -1};

    for (int d = 0; d < 4; ++d) {
        int ai_count = 0, player_count = 0;

        for (int step = -4; step <= 4; ++step) {
            int nx = x + step * dx[d];
            int ny = y + step * dy[d];
            if (nx < 0 || ny < 0 || nx >= GRID_SIZE || ny >= GRID_SIZE) continue;
            if (board[nx][ny] == ai) ai_count++;
            if (board[nx][ny] == player) player_count++;
        }

        score += ai_count * 10 + player_count * 5; // 攻击权重高于防守
    }

    return score;
}

void ai_move(char board[GRID_SIZE][GRID_SIZE], int *x, int *y) {
    int maxScore = -1;
    char ai = 2;      // AI 下白棋
    char player = 1;  // 玩家是黑棋

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (board[i][j] == 0) {
                int score = score_point(board, i, j, ai, player);
                if (score > maxScore) {
                    maxScore = score;
                    *x = i;
                    *y = j;
                }
            }
        }
    }
}
