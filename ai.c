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

// ������ʽ�������㣺����ĳһ������������ӷ�
//int evaluate_point(const GameCore* game, int x, int y, Player player) {
//    if (game->board[x][y] != PLAYER_NONE) return 0;
//
//    int score = 0;
//
//    for (int d = 0; d < 4; ++d) {
//        int dx = directions[d][0];
//        int dy = directions[d][1];
//
//        int count1 = count_continuous(game, x, y, dx, dy, player);
//        int count2 = count_continuous(game, x, y, -dx, -dy, player);
//        int total = count1 + count2 + 1;
//
//        switch (total) {
//        case 5: score += 100000; break;
//        case 4: score += 10000; break;
//        case 3: score += 1000; break;
//        case 2: score += 100; break;
//        case 1: score += 10; break;
//        }
//    }
//
//    return score;
//}
//int evaluate_point(const GameCore* game, int x, int y, Player player) {
//    if (game->board[x][y] != PLAYER_NONE) return 0;
//
//    int score = 0;
//
//    for (int d = 0; d < 4; ++d) {
//        int dx = directions[d][0];
//        int dy = directions[d][1];
//
//        int count1 = count_continuous(game, x, y, dx, dy, player);
//        int count2 = count_continuous(game, x, y, -dx, -dy, player);
//        int total = count1 + count2 + 1;
//
//        int block1 = 0, block2 = 0;
//
//        int nx1 = x + (count1 + 1) * dx;
//        int ny1 = y + (count1 + 1) * dy;
//        if (nx1 < 0 || nx1 >= BOARD_SIZE || ny1 < 0 || ny1 >= BOARD_SIZE ||
//            game->board[nx1][ny1] != PLAYER_NONE) block1 = 1;
//
//        int nx2 = x - (count2 + 1) * dx;
//        int ny2 = y - (count2 + 1) * dy;
//        if (nx2 < 0 || nx2 >= BOARD_SIZE || ny2 < 0 || ny2 >= BOARD_SIZE ||
//            game->board[nx2][ny2] != PLAYER_NONE) block2 = 1;
//
//        int blocks = block1 + block2;
//
//        // ��ϸ���ֲ���
//        if (total >= 5) {
//            score += 1000000; // ����
//        }
//        else if (total == 4 && blocks == 0) {
//            score += 100000; // ����
//        }
//        else if (total == 4 && blocks == 1) {
//            score += 10000; // ����
//        }
//        else if (total == 3 && blocks == 0) {
//            score += 5000; // ����
//        }
//        else if (total == 3 && blocks == 1) {
//            score += 1000; // ����
//        }
//        else if (total == 2 && blocks == 0) {
//            score += 500; // ���
//        }
//        else if (total == 2 && blocks == 1) {
//            score += 100; // ���
//        }
//        else {
//            score += total * 10; // ��������
//        }
//    }
//
//    return score;
//}
int evaluate_point(const GameCore* game, int x, int y, Player player) {
    if (game->board[x][y] != PLAYER_NONE) return 0;

    int live_three_count = 0;
    int block_three_count = 0;
    int live_four_count = 0;
    int block_four_count = 0;
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

        // ����ʶ��ͳ��
        if (total >= 5) {
            return 1000000; // ֱ�ӳ��壬��������
        }
        else if (total == 4 && blocks == 0) {
            live_four_count++;
        }
        else if (total == 4 && blocks == 1) {
            block_four_count++;
        }
        else if (total == 3 && blocks == 0) {
            live_three_count++;
        }
        else if (total == 3 && blocks == 1) {
            block_three_count++;
        }

        // �����÷֣��������죩
        score += total * 10;
    }

    // �ӷֲ���
    if (live_four_count >= 1) {
        score += 100000;  // ���ģ���ɱ��
    }
    if (block_four_count >= 1) {
        score += 10000;
    }
    if (live_three_count >= 1) {
        score += 5000;
    }
    if (block_three_count >= 1) {
        score += 1000;
    }

    // ���ɱ�����
    if (live_three_count >= 2) {
        score += 30000;  // ˫��������Σ��
    }
    if (live_three_count >= 1 && block_four_count >= 1) {
        score += 20000;  // ���� + ����
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
