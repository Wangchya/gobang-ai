#include "game_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化游戏
GameCore* game_init() {
    GameCore* game = (GameCore*)malloc(sizeof(GameCore));
    if (!game) return NULL;

    game_reset(game);
    return game;
}

// 释放游戏资源
void game_free(GameCore* game) {
    if (game) {
        free(game);
    }
}

// 重置游戏
void game_reset(GameCore* game) {
    // 清空棋盘
    memset(game->board, PLAYER_NONE, sizeof(game->board));

    // 重置游戏状态
    game->state = GAME_STATE_PLAYING;
    game->current_player = PLAYER_BLACK;
    game->history_count = 0;
}

// 检查指定方向上的连续棋子数量
static int check_direction(const GameCore* game, int x, int y, int dx, int dy) {
    Player player = game->board[x][y];
    int count = 1;  // 当前位置已经有一个棋子

    // 正向检查
    int cx = x + dx;
    int cy = y + dy;
    while (cx >= 0 && cx < BOARD_SIZE && cy >= 0 && cy < BOARD_SIZE &&
        game->board[cx][cy] == player) {
        count++;
        cx += dx;
        cy += dy;
    }

    // 反向检查
    cx = x - dx;
    cy = y - dy;
    while (cx >= 0 && cx < BOARD_SIZE && cy >= 0 && cy < BOARD_SIZE &&
        game->board[cx][cy] == player) {
        count++;
        cx -= dx;
        cy -= dy;
    }

    return count;
}

// 检查是否获胜
static int check_win(const GameCore* game, int x, int y) {
    // 检查四个方向：水平、垂直、两条对角线
    int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };

    for (int i = 0; i < 4; i++) {
        int count = check_direction(game, x, y, directions[i][0], directions[i][1]);
        if (count >= 5) {
            return 1;  // 五子连珠
        }
    }

    return 0;  // 未获胜
}

// 尝试落子
int game_place_stone(GameCore* game, int x, int y) {
    // 检查游戏是否在进行中
    if (game->state != GAME_STATE_PLAYING) {
        return 0;  // 游戏未在进行中
    }

    // 检查位置是否有效
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return 0;  // 位置无效
    }

    // 检查该位置是否已有棋子
    if (game->board[x][y] != PLAYER_NONE) {
        return 0;  // 位置已被占用
    }

    // 落子
    game->board[x][y] = game->current_player;

    // 记录历史
    if (game->history_count < MAX_HISTORY) {
        game->history[game->history_count].x = x;
        game->history[game->history_count].y = y;
        game->history_count++;
    }

    // 检查是否获胜
    if (check_win(game, x, y)) {
        game->state = (game->current_player == PLAYER_BLACK) ?
            GAME_STATE_BLACK_WIN : GAME_STATE_WHITE_WIN;
        return 1;  // 落子成功，游戏结束
    }

    // 检查是否平局（棋盘已满）
    if (game->history_count >= BOARD_SIZE * BOARD_SIZE) {
        game->state = GAME_STATE_DRAW;
        return 1;  // 落子成功，游戏结束
    }

    // 切换玩家
    game->current_player = (game->current_player == PLAYER_BLACK) ?
        PLAYER_WHITE : PLAYER_BLACK;

    return 1;  // 落子成功
}

// 悔棋
int game_undo(GameCore* game) {
    if (game->history_count < 1) {
        return 0;  // 没有可悔棋的操作
    }

    // 获取最后一步棋的位置
    Position last = game->history[game->history_count - 1];

    // 移除棋子
    game->board[last.x][last.y] = PLAYER_NONE;
    game->history_count--;

    // 恢复游戏状态
    game->state = GAME_STATE_PLAYING;

    // 切换回上一个玩家
    game->current_player = (game->current_player == PLAYER_BLACK) ?
        PLAYER_WHITE : PLAYER_BLACK;

    return 1;
}

// 获取游戏状态
GameState game_get_state(const GameCore* game) {
    return game->state;
}

// 获取当前玩家
Player game_get_current_player(const GameCore* game) {
    return game->current_player;
}

// 获取棋盘位置状态
Player game_get_cell_state(const GameCore* game, int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return PLAYER_NONE;
    }
    return game->board[x][y];
}

// 保存游戏状态到文件
int game_save(const GameCore* game, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }

    // 写入游戏状态
    fwrite(game, sizeof(GameCore), 1, file);

    fclose(file);
    return 1;
}

// 从文件加载游戏状态
int game_load(GameCore* game, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }

    // 读取游戏状态
    size_t read = fread(game, sizeof(GameCore), 1, file);
    fclose(file);

    return (read == 1);
}