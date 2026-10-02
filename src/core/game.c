#include "game.h"

#include <stdlib.h>

#include "rules.h"
#include "snake.h"

GameState *game_create(const GameConfig *cfg) {
    if (cfg == NULL || cfg->start_length < 1 || cfg->start_length > cfg->grid_width) {
        return NULL;
    }

    GameState *state = calloc(1, sizeof(*state));
    if (state == NULL) {
        return NULL;
    }

    state->grid_w = cfg->grid_width;
    state->grid_h = cfg->grid_height;
    state->dir = cfg->start_dir;
    state->status = ST_RUNNING;
    state->tick_interval_ms = cfg->start_tick_ms;
    state->rng_state = 0x9E3779B9u; /* фиксированный сид: детерминизм тестов */

    state->snake_cap = cfg->start_length;
    state->snake = malloc(sizeof(Point) * (size_t)state->snake_cap);
    if (state->snake == NULL) {
        free(state);
        return NULL;
    }

    /* FR-2: голова в центре, тело тянется назад по направлению старта. */
    int cx = state->grid_w / 2;
    int cy = state->grid_h / 2;
    for (int i = 0; i < cfg->start_length; i++) {
        state->snake[i].x = cx - i; /* старт вправо: голова самая правая */
        state->snake[i].y = cy;
    }
    state->snake_len = cfg->start_length;

    state->food = (Point){5, 5}; /* TODO(FR-6): заменить на food_spawn() */

    return state;
}

void game_destroy(GameState *state) {
    if (state != NULL) {
        free(state->snake);
        free(state);
    }
}

TickResult tick(GameState *state, Direction buffered_dir) {
    TickResult result = {0};
    if (state == NULL || state->status != ST_RUNNING) {
        return result;
    }

    if (buffered_dir != DIR_NONE && buffered_dir != state->dir &&
        !rules_opposite(buffered_dir, state->dir)) {
        state->dir = buffered_dir;
        result.events[result.event_count].type = EV_TURNED;
        result.event_count++;
    }

    /* Сдвиг: тело догоняет голову, голова — на новую клетку. */
    Point new_head = snake_next_head(state, state->dir);
    for (int i = state->snake_len - 1; i > 0; i--) {
        state->snake[i] = state->snake[i - 1];
    }
    state->snake[0] = new_head;

    /* TODO(FR-6..FR-8): еда и рост, столкновение со стеной и с собой,
     * счёт и ускорение (FR-11). */

    return result;
}
