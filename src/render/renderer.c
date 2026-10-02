#include "renderer.h"

#include "raylib.h"

static void draw_hud(const GameState *state) {
    /* TODO(NFR-6): контраст ≥ 4.5:1 — цвета проверить контраст-чекером.
     * TODO: кириллица требует загрузки своего шрифта — дефолтный ASCII-only. */
    DrawText(TextFormat("SCORE %d   LEN %d", state->score, state->snake_len), 10, 10, 20,
             RAYWHITE);
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
    if (state->food.x < 0) {
        return; /* еда не существует (поле заполнено) */
    }
    DrawRectangle(state->food.x * SNAKE_CELL_PX,
                  SNAKE_HUD_PX + state->food.y * SNAKE_CELL_PX, SNAKE_CELL_PX, SNAKE_CELL_PX,
                  c);
}

static void draw_snake(const GameState *state) {
    Color body = {70, 160, 60, 255};
    Color head = {130, 230, 90, 255};
    for (int i = state->snake_len - 1; i >= 0; i--) {
        Color c = (i == 0) ? head : body;
        DrawRectangle(state->snake[i].x * SNAKE_CELL_PX,
                      SNAKE_HUD_PX + state->snake[i].y * SNAKE_CELL_PX, SNAKE_CELL_PX,
                      SNAKE_CELL_PX, c);
    }
}

static void draw_centered(const char *text, int y, int size, Color color) {
    int w = SNAKE_GRID_W * SNAKE_CELL_PX;
    DrawText(text, (w - MeasureText(text, size)) / 2, y, size, color);
}

/* FR-9: экран конца игры. */
static void draw_gameover(const GameState *state) {
    int w = SNAKE_GRID_W * SNAKE_CELL_PX;
    int h = SNAKE_GRID_H * SNAKE_CELL_PX;
    DrawRectangle(0, SNAKE_HUD_PX, w, h, (Color){0, 0, 0, 190});
    int cy = SNAKE_HUD_PX + h / 2;
    draw_centered("GAME OVER", cy - 70, 40, (Color){235, 90, 90, 255});
    draw_centered(TextFormat("SCORE %d", state->score), cy - 10, 28, RAYWHITE);
    draw_centered("Press R to restart", cy + 40, 20, GRAY);
}

/* Рамка поля: в режиме сквозных стен показывает, где проходит граница. */
static void draw_border(void) {
    DrawRectangleLines(0, SNAKE_HUD_PX, SNAKE_GRID_W * SNAKE_CELL_PX,
                       SNAKE_GRID_H * SNAKE_CELL_PX, (Color){70, 70, 95, 255});
}

void draw_game(const GameState *state) {
    BeginDrawing();
    ClearBackground((Color){18, 18, 24, 255});
    draw_hud(state);
    draw_board(state);
    draw_food(state);
    draw_snake(state);
    draw_border();
    if (state->status == ST_GAMEOVER) {
        draw_gameover(state);
    }
    EndDrawing();
}
