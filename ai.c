#include "game_core.h"
#include <stdlib.h>
#include <limits.h>

static int directions[4][2] = {
    {1, 0},  // 横
    {0, 1},  // 纵
    {1, 1},  // 正对角
    {1, -1}  // 反对角
};

// 检查某个方向上的连续数
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

// 简单启发式分数计算：根据某一方向的连续数加分
int evaluate_point(const GameCore* game, int x, int y, Player player) {
    if (game->board[x][y] != PLAYER_NONE) return 0;

    int score = 0;

    for (int d = 0; d < 4; ++d) {
        int dx = directions[d][0];
        int dy = directions[d][1];

        int count1 = count_continuous(game, x, y, dx, dy, player);
        int count2 = count_continuous(game, x, y, -dx, -dy, player);
        int total = count1 + count2 + 1;

        switch (total) {
        case 5: score += 100000; break;
        case 4: score += 10000; break;
        case 3: score += 1000; break;
        case 2: score += 100; break;
        case 1: score += 10; break;
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

            // 防守优先级略低于进攻
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
