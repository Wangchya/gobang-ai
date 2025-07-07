#include "game_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ��ʼ����Ϸ
GameCore* game_init() {
    GameCore* game = (GameCore*)malloc(sizeof(GameCore));
    if (!game) return NULL;

    game_reset(game);
    return game;
}

// �ͷ���Ϸ��Դ
void game_free(GameCore* game) {
    if (game) {
        free(game);
    }
}

// ������Ϸ
void game_reset(GameCore* game) {
    // �������
    memset(game->board, PLAYER_NONE, sizeof(game->board));

    // ������Ϸ״̬
    game->state = GAME_STATE_PLAYING;
    game->current_player = PLAYER_BLACK;
    game->history_count = 0;
}

// ���ָ�������ϵ�������������
static int check_direction(const GameCore* game, int x, int y, int dx, int dy) {
    Player player = game->board[x][y];
    int count = 1;  // ��ǰλ���Ѿ���һ������

    // ������
    int cx = x + dx;
    int cy = y + dy;
    while (cx >= 0 && cx < BOARD_SIZE && cy >= 0 && cy < BOARD_SIZE &&
        game->board[cx][cy] == player) {
        count++;
        cx += dx;
        cy += dy;
    }

    // ������
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

// ����Ƿ��ʤ
static int check_win(const GameCore* game, int x, int y) {
    // ����ĸ�����ˮƽ����ֱ�������Խ���
    int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };

    for (int i = 0; i < 4; i++) {
        int count = check_direction(game, x, y, directions[i][0], directions[i][1]);
        if (count >= 5) {
            return 1;  // ��������
        }
    }

    return 0;  // δ��ʤ
}

// ��������
int game_place_stone(GameCore* game, int x, int y) {
    // �����Ϸ�Ƿ��ڽ�����
    if (game->state != GAME_STATE_PLAYING) {
        return 0;  // ��Ϸδ�ڽ�����
    }

    // ���λ���Ƿ���Ч
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return 0;  // λ����Ч
    }

    // ����λ���Ƿ���������
    if (game->board[x][y] != PLAYER_NONE) {
        return 0;  // λ���ѱ�ռ��
    }

    // ����
    game->board[x][y] = game->current_player;

    // ��¼��ʷ
    if (game->history_count < MAX_HISTORY) {
        game->history[game->history_count].x = x;
        game->history[game->history_count].y = y;
        game->history_count++;
    }

    // ����Ƿ��ʤ
    if (check_win(game, x, y)) {
        game->state = (game->current_player == PLAYER_BLACK) ?
            GAME_STATE_BLACK_WIN : GAME_STATE_WHITE_WIN;
        return 1;  // ���ӳɹ�����Ϸ����
    }

    // ����Ƿ�ƽ�֣�����������
    if (game->history_count >= BOARD_SIZE * BOARD_SIZE) {
        game->state = GAME_STATE_DRAW;
        return 1;  // ���ӳɹ�����Ϸ����
    }

    // �л����
    game->current_player = (game->current_player == PLAYER_BLACK) ?
        PLAYER_WHITE : PLAYER_BLACK;

    return 1;  // ���ӳɹ�
}

// ����
int game_undo(GameCore* game) {
    if (game->history_count < 1) {
        return 0;  // û�пɻ���Ĳ���
    }

    // ��ȡ���һ�����λ��
    Position last = game->history[game->history_count - 1];

    // �Ƴ�����
    game->board[last.x][last.y] = PLAYER_NONE;
    game->history_count--;

    // �ָ���Ϸ״̬
    game->state = GAME_STATE_PLAYING;

    // �л�����һ�����
    game->current_player = (game->current_player == PLAYER_BLACK) ?
        PLAYER_WHITE : PLAYER_BLACK;

    return 1;
}

// ��ȡ��Ϸ״̬
GameState game_get_state(const GameCore* game) {
    return game->state;
}

// ��ȡ��ǰ���
Player game_get_current_player(const GameCore* game) {
    return game->current_player;
}

// ��ȡ����λ��״̬
Player game_get_cell_state(const GameCore* game, int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return PLAYER_NONE;
    }
    return game->board[x][y];
}

// ������Ϸ״̬���ļ�
int game_save(const GameCore* game, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }

    // д����Ϸ״̬
    fwrite(game, sizeof(GameCore), 1, file);

    fclose(file);
    return 1;
}

// ���ļ�������Ϸ״̬
int game_load(GameCore* game, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }

    // ��ȡ��Ϸ״̬
    size_t read = fread(game, sizeof(GameCore), 1, file);
    fclose(file);

    return (read == 1);
}