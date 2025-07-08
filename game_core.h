#pragma once

#define BOARD_SIZE 15
#define MAX_HISTORY 225

// �������
typedef enum {
    PLAYER_NONE = 0,
    PLAYER_BLACK = 1,
    PLAYER_WHITE = 2
} Player;

// ��Ϸ״̬
typedef enum {
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_BLACK_WIN,
    GAME_STATE_WHITE_WIN,
    GAME_STATE_DRAW
} GameState;

// ����λ��
typedef struct {
    int x;
    int y;
} Position;

// ��Ϸ���Ľṹ
typedef struct {
    Player board[BOARD_SIZE][BOARD_SIZE];  // ����״̬
    GameState state;                       // ��ǰ��Ϸ״̬
    Player current_player;                 // ��ǰ���
    Position history[MAX_HISTORY];         // ������ʷ
    int history_count;                     // ��ʷ��¼��
} GameCore;

// ��ʼ����Ϸ
GameCore* game_init();

// �ͷ���Ϸ��Դ
void game_free(GameCore* game);

// ������Ϸ
void game_reset(GameCore* game);

// ��������
int game_place_stone(GameCore* game, int x, int y);

// ����
int game_undo(GameCore* game);

// ��ȡ��Ϸ״̬
GameState game_get_state(const GameCore* game);

// ��ȡ��ǰ���
Player game_get_current_player(const GameCore* game);

// ��ȡ����λ��״̬
Player game_get_cell_state(const GameCore* game, int x, int y);

// ������Ϸ״̬���ļ�
int game_save(const GameCore* game, int mode, const char* filename);

// ���ļ�������Ϸ״̬
int game_load(GameCore* game, int* mode, const char* filename);

// AI���ӽӿ�
Position ai_make_move(const GameCore* game);


