#include <stddef.h>

#include "raylib.h"

#include "config.h"
#include "core/game.h"
#include "input/input.h"
#include "render/renderer.h"

int main(void) {
    GameConfig cfg = {
        .grid_width = SNAKE_GRID_W,
        .grid_height = SNAKE_GRID_H,
        .start_length = SNAKE_START_LENGTH,
        .start_dir = SNAKE_START_DIR,
        .points_per_food = SNAKE_POINTS_PER_FOOD,
        .start_tick_ms = SNAKE_START_TICK_MS,
        .min_tick_ms = SNAKE_MIN_TICK_MS,
        .speed_up_every = SNAKE_SPEED_UP_EVERY,
        .speed_steps = SNAKE_SPEED_STEPS,
        .input_buffer_size = SNAKE_INPUT_BUFFER_SIZE,
    };

    GameState *game = game_create(&cfg);
    if (game == NULL) {
        return 1;
    }
    input_reset();

    InitWindow(SNAKE_GRID_W * SNAKE_CELL_PX, SNAKE_GRID_H * SNAKE_CELL_PX + SNAKE_HUD_PX,
               "Змейка");
    SetTargetFPS(60);
    SetWindowFocused(); /* окно запускается из фона — забрать фокус клавиатуры */

    /* Fixed timestep (docs/architecture/overview.md §3): логика тикает
     * с фиксированным интервалом, рендер — каждый кадр. */
    double accumulator = 0.0;
    while (!WindowShouldClose()) {
        /* Опрос каждый кадр: нажатия живут один кадр, тик — раз в несколько
         * кадров, поэтому ввод копится в буфер (FR-5). */
        input_poll(game->dir);

        if (game->status == ST_GAMEOVER) {
            /* FR-9: рестарт по R/Enter. */
            if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER)) {
                game_destroy(game);
                game = game_create(&cfg);
                if (game == NULL) {
                    return 1;
                }
                input_reset();
                accumulator = 0.0;
            }
        } else {
            accumulator += GetFrameTime();
            double step = (double)game->tick_interval_ms / 1000.0;
            while (accumulator >= step) {
                tick(game, input_next_direction());
                accumulator -= step;
            }
        }

        draw_game(game);
    }

    CloseWindow();
    game_destroy(game);
    return 0;
}
