#include <SDL.h>
#include <stdio.h>
#include "game.h"
#include "ai.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_SIZE 15
#define CELL_SIZE (WINDOW_WIDTH / GRID_SIZE)

char board[GRID_SIZE][GRID_SIZE] = {0};

//开始界面 
void show_start_screen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255); // 浅蓝背景
    SDL_RenderClear(renderer);

    SDL_Rect button = {200, 250, 200, 100};
    SDL_SetRenderDrawColor(renderer, 50, 150, 250, 255);  // 蓝色按钮
    SDL_RenderFillRect(renderer, &button);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    int waiting = 1;
    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) exit(0);
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                if (mx >= 200 && mx <= 400 && my >= 250 && my <= 350) {
                    waiting = 0;
                }
            }
        }
        SDL_Delay(10);
    }
}

// 结束界面 
void show_end_screen(SDL_Renderer *renderer, const char *message) {
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255); // 灰背景
    SDL_RenderClear(renderer);

    SDL_Rect banner = {150, 250, 300, 100};
    SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255);  // 红色提示框
    SDL_RenderFillRect(renderer, &banner);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    int waiting = 1;
    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) exit(0);
            else if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
                waiting = 0;
            }
        }
        SDL_Delay(10);
    }
}

// 绘制棋盘 
void draw_board(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 黑色
    for (int i = 0; i < GRID_SIZE; ++i) {
        SDL_RenderDrawLine(renderer,
            CELL_SIZE / 2,
            CELL_SIZE / 2 + i * CELL_SIZE,
            WINDOW_WIDTH - CELL_SIZE / 2,
            CELL_SIZE / 2 + i * CELL_SIZE);
        SDL_RenderDrawLine(renderer,
            CELL_SIZE / 2 + i * CELL_SIZE,
            CELL_SIZE / 2,
            CELL_SIZE / 2 + i * CELL_SIZE,
            WINDOW_HEIGHT - CELL_SIZE / 2);
    }
}

//  绘制棋子 
void draw_pieces(SDL_Renderer *renderer) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (board[i][j] != 0) {
                if (board[i][j] == 1)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 黑棋
                else
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 白棋

                SDL_Rect rect = {
                    j * CELL_SIZE + CELL_SIZE / 4,
                    i * CELL_SIZE + CELL_SIZE / 4,
                    CELL_SIZE / 2,
                    CELL_SIZE / 2
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

// 主程序入口 
int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Gomoku AI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    show_start_screen(renderer); 

    int running = 1;
    int turn = 1; 

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (turn == 1) {
                    int x = e.button.x / CELL_SIZE;
                    int y = e.button.y / CELL_SIZE;
                    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && board[y][x] == 0) {
                        board[y][x] = 1;
                        if (check_win(board, y, x)) {
                            printf("Player wins!\n");
                            show_end_screen(renderer, "Player Wins!");
                            running = 0;
                        } else {
                            turn = 2;
                        }
                    }
                }
            }
        }

        // AI 落子逻辑
        if (turn == 2 && running) {
            int ai_x = -1, ai_y = -1;
            ai_move(board, &ai_x, &ai_y);
            if (ai_x != -1 && board[ai_x][ai_y] == 0) {
                board[ai_x][ai_y] = 2;
                if (check_win(board, ai_x, ai_y)) {
                    printf("AI wins!\n");
                    show_end_screen(renderer, "AI Wins!");
                    running = 0;
                } else {
                    turn = 1;
                }
            }
            SDL_Delay(300); 
        }

        SDL_SetRenderDrawColor(renderer, 245, 222, 179, 255); // 背景图
        SDL_RenderClear(renderer);
        draw_board(renderer);
        draw_pieces(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
