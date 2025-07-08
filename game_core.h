#pragma once

#define BOARD_SIZE 15
#define MAX_HISTORY 225

// 玩家类型
typedef enum {
    PLAYER_NONE = 0,
    PLAYER_BLACK = 1,
    PLAYER_WHITE = 2
} Player;

// 游戏状态
typedef enum {
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_BLACK_WIN,
    GAME_STATE_WHITE_WIN,
    GAME_STATE_DRAW
} GameState;

// 棋盘位置
typedef struct {
    int x;
    int y;
} Position;

// 游戏核心结构
typedef struct {
    Player board[BOARD_SIZE][BOARD_SIZE];  // 棋盘状态
    GameState state;                       // 当前游戏状态
    Player current_player;                 // 当前玩家
    Position history[MAX_HISTORY];         // 落子历史
    int history_count;                     // 历史记录数
} GameCore;

// 初始化游戏
GameCore* game_init();

// 释放游戏资源
void game_free(GameCore* game);

// 重置游戏
void game_reset(GameCore* game);

// 尝试落子
int game_place_stone(GameCore* game, int x, int y);

// 悔棋
int game_undo(GameCore* game);

// 获取游戏状态
GameState game_get_state(const GameCore* game);

// 获取当前玩家
Player game_get_current_player(const GameCore* game);

// 获取棋盘位置状态
Player game_get_cell_state(const GameCore* game, int x, int y);

// 保存游戏状态到文件
int game_save(const GameCore* game, int mode, const char* filename);

// 从文件加载游戏状态
int game_load(GameCore* game, int* mode, const char* filename);

// AI落子接口
Position ai_make_move(const GameCore* game);


