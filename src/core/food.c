#include "food.h"

#include <stddef.h>

static unsigned xorshift32(unsigned *state) {
    unsigned x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *state = x;
}

static int cell_occupied(const GameState *state, Point p) {
    for (int i = 0; i < state->snake_len; i++) {
        if (state->snake[i].x == p.x && state->snake[i].y == p.y) {
            return 1;
        }
    }
    return 0;
}

Point food_spawn(GameState *state) {
    Point nowhere = {-1, -1};
    if (state == NULL || state->grid_w < 1 || state->grid_h < 1) {
        return nowhere;
    }

    int cells = state->grid_w * state->grid_h;

    /* FR-6: случайная (по сиду состояния) свободная клетка. Пока поле
     * не забито, отбраковка попаданий под змейкой сходится быстро. */
    for (int attempt = 0; attempt < cells; attempt++) {
        Point p = {
            (int)(xorshift32(&state->rng_state) % (unsigned)state->grid_w),
            (int)(xorshift32(&state->rng_state) % (unsigned)state->grid_h),
        };
        if (!cell_occupied(state, p)) {
            return p;
        }
    }

    /* Поле почти заполнено: гарантированный линейный проход. */
    for (int y = 0; y < state->grid_h; y++) {
        for (int x = 0; x < state->grid_w; x++) {
            Point p = {x, y};
            if (!cell_occupied(state, p)) {
                return p;
            }
        }
    }

    return nowhere; /* змейка заполнила всё поле — еды нет (v1: без победы) */
}
