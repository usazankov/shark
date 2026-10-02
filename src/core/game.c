#include "game.h"

#include <stdlib.h>

#include "food.h"
#include "rules.h"
#include "snake.h"

static int config_valid(const GameConfig *cfg) {
    return cfg != NULL && cfg->grid_width >= 1 && cfg->grid_height >= 1 &&
           cfg->start_length >= 1 && cfg->start_length <= cfg->grid_width &&
           cfg->points_per_food >= 0 && cfg->start_tick_ms > 0 &&
           cfg->min_tick_ms > 0 && cfg->min_tick_ms <= cfg->start_tick_ms &&
           cfg->speed_up_every >= 1 && cfg->speed_steps >= 1;
}

/* FR-11: линейные ступени от start_tick_ms до min_tick_ms. */
static void update_speed(GameState *state) {
    int step_down =
        (state->cfg.start_tick_ms - state->cfg.min_tick_ms) / state->cfg.speed_steps;
    int steps = state->score / state->cfg.speed_up_every;
    int interval = state->cfg.start_tick_ms - steps * step_down;
    if (interval < state->cfg.min_tick_ms) {
        interval = state->cfg.min_tick_ms;
    }
    state->tick_interval_ms = interval;
}

GameState *game_create(const GameConfig *cfg) {
    if (!config_valid(cfg)) {
        return NULL;
    }

    GameState *state = calloc(1, sizeof(*state));
    if (state == NULL) {
        return NULL;
    }

    state->cfg = *cfg;
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

    state->food = food_spawn(state);

    return state;
}

void game_destroy(GameState *state) {
    if (state != NULL) {
        free(state->snake);
        free(state);
    }
}

static void die(GameState *state, TickResult *result, DeathCause cause) {
    state->status = ST_GAMEOVER;
    result->events[result->event_count].type = EV_DIED;
    result->events[result->event_count].cause = cause;
    result->event_count++;
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

    Point new_head = snake_next_head(state, state->dir);

    /* FR-8: границы поля — два режима. */
    if (state->cfg.wrap_walls) {
        if (new_head.x < 0) {
            new_head.x += state->grid_w;
        } else if (new_head.x >= state->grid_w) {
            new_head.x -= state->grid_w;
        }
        if (new_head.y < 0) {
            new_head.y += state->grid_h;
        } else if (new_head.y >= state->grid_h) {
            new_head.y -= state->grid_h;
        }
    } else if (new_head.x < 0 || new_head.x >= state->grid_w || new_head.y < 0 ||
               new_head.y >= state->grid_h) {
        die(state, &result, DEATH_WALL);
        return result; /* змейка замирает в момент столкновения */
    }

    int eating = (new_head.x == state->food.x && new_head.y == state->food.y);

    /* FR-8: собственное тело. Хвост в этот же тик освобождает клетку,
     * поэтому при движении без еды последний сегмент из проверки исключён. */
    int check_len = state->snake_len - (eating ? 0 : 1);
    for (int i = 0; i < check_len; i++) {
        if (state->snake[i].x == new_head.x && state->snake[i].y == new_head.y) {
            die(state, &result, DEATH_SELF);
            return result;
        }
    }

    int grew = snake_advance(state, new_head, eating);

    /* FR-7 + FR-6: еда съедена — рост, счёт, новая еда. */
    if (eating && grew) {
        state->score += state->cfg.points_per_food;
        result.events[result.event_count].type = EV_ATE;
        result.events[result.event_count].at = new_head;
        result.event_count++;
        state->food = food_spawn(state);
        update_speed(state); /* FR-11 */
    }

    return result;
}
