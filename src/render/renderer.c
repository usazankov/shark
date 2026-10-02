#include "renderer.h"

#include "raylib.h"

static void draw_hud(const GameState *state) {
    /* TODO(NFR-6): контраст ≥ 4.5:1 — текущие цвета проверить контраст-чекером. */
    DrawText(TextFormat("SCORE %d   LEN %d", state->score, state->snake_len), 10, 10,
             20, RAYWHITE);
}

static void draw_board(const GameState *state) {
    Color a = {28, 28, 38, 255};
    Color b = {24, 24, 32, 255};
    for (int y = 0; y < state->grid_h; y++) {
        for (int x = 0; x < state->grid_w; x++) {
            Color c = ((x + y) % 2 == 0) ? a : b;
            DrawRectangle(x * SNAKE_CELL_PX, SNAKE_HUD_PX + y * SNAKE_CELL_PX,
                          SNAKE_CELL_PX, SNAKE_CELL_PX, c);
        }
    }
}

static void draw_food(const GameState *state) {
    Color c = {220, 70, 70, 255};
    DrawRectangle(state->food.x * SNAKE_CELL_PX, SNAKE_HUD_PX + state->food.y * SNAKE_CELL_PX,
                  SNAKE_CELL_PX, SNAKE_CELL_PX, c);
}

static void draw_snake(const GameState *state) {
    Color body = {70, 160, 60, 255};
    Color head = {130, 230, 90, 255};
    for (int i = state->snake_len - 1; i >= 0; i--) {
        Color c = (i == 0) ? head : body;
        DrawRectangle(state->snake[i].x * SNAKE_CELL_PX,
                      SNAKE_HUD_PX + state->snake[i].y * SNAKE_CELL_PX,
                      SNAKE_CELL_PX, SNAKE_CELL_PX, c);
    }
}

void draw_game(const GameState *state) {
    BeginDrawing();
    ClearBackground((Color){18, 18, 24, 255});
    draw_hud(state);
    draw_board(state);
    draw_food(state);
    draw_snake(state);
    EndDrawing();
}
