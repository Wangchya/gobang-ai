#define _CRT_SECURE_NO_WARNINGS 1
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "game_core.h"
//
//extern Position ai_make_move(const GameCore* game);
//
//void print_board(const GameCore* game) {
//    printf("   ");
//    for (int i = 0; i < BOARD_SIZE; i++) printf("%2d", i);
//    printf("\n");
//    for (int y = 0; y < BOARD_SIZE; y++) {
//        printf("%2d ", y);
//        for (int x = 0; x < BOARD_SIZE; x++) {
//            Player p = game_get_cell_state(game, x, y);
//            char c = (p == PLAYER_BLACK) ? 'B' : (p == PLAYER_WHITE) ? 'W' : '.';
//            printf(" %c", c);
//        }
//        printf("\n");
//    }
//}
//
//int main() {
//    GameCore* game = game_init();
//    if (!game) {
//        printf("游戏初始化失败！\n");
//        return 1;
//    }
//
//    int mode = 0; // 0 = 双人, 1 = 玩家 vs AI
//    printf("请选择游戏模式：\n");
//    printf("1. 玩家 vs 玩家\n");
//    printf("2. 玩家 vs AI\n");
//    printf("输入 1 或 2: ");
//    scanf("%d", &mode);
//    getchar(); // 吃掉回车
//
//    printf("游戏开始！输入格式：x y（坐标），输入 u 悔棋，q 退出。\n\n");
//
//    while (1) {
//        print_board(game);
//
//        Player current = game_get_current_player(game);
//        printf("当前玩家：%s\n", current == PLAYER_BLACK ? "黑棋 (B)" : "白棋 (W)");
//
//        if (mode == 2 && current == PLAYER_WHITE) {
//            printf("AI 正在思考...\n");
//            Position ai_move = ai_make_move(game);
//            if (ai_move.x != -1) {
//                game_place_stone(game, ai_move.x, ai_move.y);
//            }
//            else {
//                printf("AI 无法落子。\n");
//            }
//        }
//        else {
//            printf("请输入落子位置：");
//            char input[32];
//            fgets(input, sizeof(input), stdin);
//
//            if (input[0] == 'q') {
//                printf("退出游戏。\n");
//                break;
//            }
//            if (input[0] == 'u') {
//                if (game_undo(game)) {
//                    printf("悔棋成功。\n");
//                }
//                else {
//                    printf("没有可以悔的棋。\n");
//                }
//                continue;
//            }
//
//            int x, y;
//            if (sscanf(input, "%d %d", &x, &y) != 2) {
//                printf("输入格式错误，请输入两个整数。\n");
//                continue;
//            }
//
//            if (!game_place_stone(game, x, y)) {
//                printf("落子失败，请重试。\n");
//                continue;
//            }
//        }
//
//        GameState state = game_get_state(game);
//        if (state == GAME_STATE_BLACK_WIN) {
//            print_board(game);
//            printf("黑方胜利！\n");
//            break;
//        }
//        else if (state == GAME_STATE_WHITE_WIN) {
//            print_board(game);
//            printf("白方胜利！\n");
//            break;
//        }
//        else if (state == GAME_STATE_DRAW) {
//            print_board(game);
//            printf("平局！\n");
//            break;
//        }
//    }
//
//    game_free(game);
//    return 0;
//}
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game_core.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 850 
#define CELL_SIZE (WINDOW_WIDTH / BOARD_SIZE)
#define STONE_RADIUS (CELL_SIZE / 2 - 4)



// 绘制实心圆（模拟圆形棋子）
void draw_filled_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void draw_board(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(renderer, CELL_SIZE / 2, CELL_SIZE / 2 + i * CELL_SIZE,
            WINDOW_WIDTH - CELL_SIZE / 2, CELL_SIZE / 2 + i * CELL_SIZE);
        SDL_RenderDrawLine(renderer, CELL_SIZE / 2 + i * CELL_SIZE, CELL_SIZE / 2,
            CELL_SIZE / 2 + i * CELL_SIZE, WINDOW_WIDTH - CELL_SIZE / 2);
    }
}

void draw_stones(SDL_Renderer* renderer, GameCore* game) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Player p = game->board[x][y];
            if (p == PLAYER_NONE) continue;

            if (p == PLAYER_BLACK)
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            int cx = CELL_SIZE / 2 + x * CELL_SIZE;
            int cy = CELL_SIZE / 2 + y * CELL_SIZE;

            draw_filled_circle(renderer, cx, cy, STONE_RADIUS);
        }
    }
}

void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* msg, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, msg, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int show_menu(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Event e;
    SDL_Color color = { 0, 0, 0, 255 };

    SDL_Rect btn_pvp = { WINDOW_WIDTH / 2 - 150, 300, 300, 60 };
    SDL_Rect btn_ai = { WINDOW_WIDTH / 2 - 150, 400, 300, 60 };

    while (1) {
        SDL_SetRenderDrawColor(renderer, 255, 220, 180, 255);
        SDL_RenderClear(renderer);

        draw_text(renderer, font, "Gomoku", WINDOW_WIDTH / 2 - 120, 180, color);
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &btn_pvp);
        SDL_RenderFillRect(renderer, &btn_ai);
        draw_text(renderer, font, "Player vs Player", btn_pvp.x + 40, btn_pvp.y + 15, color);
        draw_text(renderer, font, "Player vs AI", btn_ai.x + 60, btn_ai.y + 15, color);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;

                if (mx >= btn_pvp.x && mx <= btn_pvp.x + btn_pvp.w &&
                    my >= btn_pvp.y && my <= btn_pvp.y + btn_pvp.h) {
                    return 1;
                }

                if (mx >= btn_ai.x && mx <= btn_ai.x + btn_ai.w &&
                    my >= btn_ai.y && my <= btn_ai.y + btn_ai.h) {
                    return 2;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("assets/font.ttf", 32);
    if (!font) {
        printf("字体加载失败: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GameCore* game = game_init();
    int mode = show_menu(renderer, font);
    if (mode == -1) return 0;

    bool running = true;
    bool show_winner = false;
    const char* winner_text = "";

    SDL_Rect btn_restart = { 100, 760, 200, 50 };
    SDL_Rect btn_quit = { 500, 760, 200, 50 };
    SDL_Rect btn_undo = { 300, 760, 200, 50 };

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;

                if (show_winner) {
                    if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_restart)) {
                        game_reset(game);
                        show_winner = false;
                        continue;
                    }
                    if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_quit)) {
                        running = false;
                        break;
                    }
                }
                else {
                    if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_undo)) {
                        game_undo(game); // 玩家悔棋
                        if (mode == 2 && game_get_current_player(game) == PLAYER_WHITE) {
                            game_undo(game); // AI 也悔棋
                        }
                        continue;
                    }

                    int gx = mx / CELL_SIZE;
                    int gy = my / CELL_SIZE;
                    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
                        if (game_place_stone(game, gx, gy)) {
                            GameState s = game_get_state(game);
                            if (s == GAME_STATE_BLACK_WIN) {
                                show_winner = true;
                                winner_text = "Black Win!";
                            }
                            else if (s == GAME_STATE_WHITE_WIN) {
                                show_winner = true;
                                winner_text = "White Win!";
                            }
                            else if (s == GAME_STATE_DRAW) {
                                show_winner = true;
                                winner_text = "Draw!";
                            }

                            if (mode == 2 && !show_winner && game_get_current_player(game) == PLAYER_WHITE) {
                                SDL_Delay(200);
                                Position move = ai_make_move(game);
                                game_place_stone(game, move.x, move.y);

                                GameState s = game_get_state(game);
                                if (s == GAME_STATE_BLACK_WIN) {
                                    show_winner = true;
                                    winner_text = "Black Win!";
                                }
                                else if (s == GAME_STATE_WHITE_WIN) {
                                    show_winner = true;
                                    winner_text = "White Win!";
                                }
                                else if (s == GAME_STATE_DRAW) {
                                    show_winner = true;
                                    winner_text = "Draw!";
                                }
                            }
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 245, 200, 130, 255);
        SDL_RenderClear(renderer);
        draw_board(renderer);
        draw_stones(renderer, game);

        SDL_Color color = { 0, 0, 0, 255 };
        if (show_winner) {
            draw_text(renderer, font, winner_text, 30, 30, (SDL_Color) { 200, 0, 0, 255 });

            SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
            SDL_RenderFillRect(renderer, &btn_restart);
            draw_text(renderer, font, "Restart game", btn_restart.x + 30, btn_restart.y + 10, color);

            SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255);
            SDL_RenderFillRect(renderer, &btn_quit);
            draw_text(renderer, font, "Quit game", btn_quit.x + 40, btn_quit.y + 10, color);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 120, 180, 250, 255);
            SDL_RenderFillRect(renderer, &btn_undo);
            draw_text(renderer, font, "Undo move", btn_undo.x + 70, btn_undo.y + 10, color);
        }

        SDL_RenderPresent(renderer);
    }

    game_free(game);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
