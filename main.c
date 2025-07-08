#define _CRT_SECURE_NO_WARNINGS 1
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

void draw_button(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect btn, const char* text, SDL_Color bg_color, SDL_Color text_color) {
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 255);
    SDL_RenderFillRect(renderer, &btn);

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = {
        btn.x + (btn.w - surface->w) / 2,
        btn.y + (btn.h - surface->h) / 2,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int show_menu(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Event e;
    SDL_Color color = { 0, 0, 0, 255 };

    SDL_Rect btn_pvp = { WINDOW_WIDTH / 2 - 150, 300, 300, 60 };
    SDL_Rect btn_ai = { WINDOW_WIDTH / 2 - 150, 400, 300, 60 };
    SDL_Rect btn_load = { WINDOW_WIDTH / 2 - 150, 500, 300, 60 };

    while (1) {
        SDL_SetRenderDrawColor(renderer, 255, 220, 180, 255);
        SDL_RenderClear(renderer);

        draw_button(renderer, font, btn_pvp, "Player vs Player", (SDL_Color) { 150, 150, 150, 255 }, color);
        draw_button(renderer, font, btn_ai, "Player vs AI", (SDL_Color) { 150, 150, 150, 255 }, color);
        draw_button(renderer, font, btn_load, "Load Save", (SDL_Color) { 180, 180, 100, 255 }, color);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;

                if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_pvp)) return 1;
                if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_ai)) return 2;
                if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_load)) return 3;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("assets/font.ttf", 28);
    if (!font) {
        printf("字体加载失败: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

menu:
    GameCore* game = game_init();
    int mode = show_menu(renderer, font);
    if (mode == -1) return 0;

    if (mode == 3) {
        game_load(game, "save.txt");
        mode = (game_get_current_player(game) == PLAYER_BLACK) ? 1 : 2;
    }

    bool running = true;
    bool show_winner = false;
    const char* winner_text = "";

    SDL_Rect btn_restart = { 100, 760, 200, 60 };
    SDL_Rect btn_quit = { 500, 760, 200, 60 };
    SDL_Rect btn_undo = { 300, 760, 200, 60 };
    SDL_Rect btn_save = { 10, 760, 120, 60 };

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
                        game_free(game);
                        goto menu;
                    }
                }
                else {
                    if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_undo)) {
                        game_undo(game);
                        if (mode == 2 && game_get_current_player(game) == PLAYER_WHITE) {
                            game_undo(game);
                        }
                        continue;
                    }
                    if (SDL_PointInRect(&(SDL_Point) { mx, my }, & btn_save)) {
                        game_save(game, "save.txt");
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
            draw_button(renderer, font, btn_restart, "Restart Game", (SDL_Color) { 100, 200, 100, 255 }, color);
            draw_button(renderer, font, btn_quit, "Back to Menu", (SDL_Color) { 200, 100, 100, 255 }, color);
            draw_button(renderer, font, (SDL_Rect) { 30, 20, 300, 40 }, winner_text, (SDL_Color) { 255, 255, 255, 255 }, (SDL_Color) { 200, 0, 0, 255 });
        }
        else {
            draw_button(renderer, font, btn_undo, "Undo Move", (SDL_Color) { 120, 180, 250, 255 }, color);
            draw_button(renderer, font, btn_save, "Save", (SDL_Color) { 180, 220, 120, 255 }, color);
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
