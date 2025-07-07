#include "game_core.h"
#include <stdlib.h>
#include <limits.h>

static int directions[4][2] = {
    {1, 0},  // ��
    {0, 1},  // ��
    {1, 1},  // ���Խ�
    {1, -1}  // ���Խ�
};

// ���ĳ�������ϵ�������
int count_continuous(const GameCore* game, int x, int y, int dx, int dy, Player player) {
    int count = 0;
    int nx = x + dx, ny = y + dy;
    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
        game->board[nx][ny] == player) {
        count++;
        nx += dx;
        ny += dy;
    }
    return count;
}

int evaluate_point(const GameCore* game, int x, int y, Player player) {
    if (game->board[x][y] != PLAYER_NONE) return 0;

    int score = 0;

    for (int d = 0; d < 4; ++d) {
        int dx = directions[d][0];
        int dy = directions[d][1];

        int count1 = count_continuous(game, x, y, dx, dy, player);
        int count2 = count_continuous(game, x, y, -dx, -dy, player);
        int total = count1 + count2 + 1;

        int block1 = 0, block2 = 0;

        int nx1 = x + (count1 + 1) * dx;
        int ny1 = y + (count1 + 1) * dy;
        if (nx1 < 0 || nx1 >= BOARD_SIZE || ny1 < 0 || ny1 >= BOARD_SIZE ||
            game->board[nx1][ny1] != PLAYER_NONE) block1 = 1;

        int nx2 = x - (count2 + 1) * dx;
        int ny2 = y - (count2 + 1) * dy;
        if (nx2 < 0 || nx2 >= BOARD_SIZE || ny2 < 0 || ny2 >= BOARD_SIZE ||
            game->board[nx2][ny2] != PLAYER_NONE) block2 = 1;

        int blocks = block1 + block2;

        // ��ϸ���ֲ���
        if (total >= 5) {
            score += 1000000; // ����
        }
        else if (total == 4 && blocks == 0) {
            score += 100000; // ����
        }
        else if (total == 4 && blocks == 1) {
            score += 10000; // ����
        }
        else if (total == 3 && blocks == 0) {
            score += 5000; // ����
        }
        else if (total == 3 && blocks == 1) {
            score += 1000; // ����
        }
        else if (total == 2 && blocks == 0) {
            score += 500; // ���
        }
        else if (total == 2 && blocks == 1) {
            score += 100; // ���
        }
        else {
            score += total * 10; // ��������
        }
    }

    return score;
}
Position ai_make_move(const GameCore* game) {
    int max_score = INT_MIN;
    Position best_move = { -1, -1 };

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            if (game->board[x][y] != PLAYER_NONE) continue;

            int my_score = evaluate_point(game, x, y, PLAYER_WHITE);
            int opp_score = evaluate_point(game, x, y, PLAYER_BLACK);

            // �������ȼ��Ե��ڽ���
            int total_score = my_score + (opp_score * 0.8);

            if (total_score > max_score) {
                max_score = total_score;
                best_move.x = x;
                best_move.y = y;
            }
        }
    }

    return best_move;
}
