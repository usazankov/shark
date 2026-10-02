#include "snake.h"

#include <stdlib.h>

Point snake_next_head(const GameState *state, Direction dir) {
    Point head = state->snake[0];
    switch (dir) {
    case DIR_UP:
        head.y -= 1;
        break;
    case DIR_DOWN:
        head.y += 1;
        break;
    case DIR_LEFT:
        head.x -= 1;
        break;
    case DIR_RIGHT:
        head.x += 1;
        break;
    case DIR_NONE:
        break;
    }
    return head;
}

int snake_advance(GameState *state, Point new_head, int grow) {
    if (grow && state->snake_len == state->snake_cap) {
        int new_cap = state->snake_cap * 2;
        Point *grown = realloc(state->snake, sizeof(Point) * (size_t)new_cap);
        if (grown == NULL) {
            grow = 0; /* без роста, но движение продолжается */
        } else {
            state->snake = grown;
            state->snake_cap = new_cap;
        }
    }

    int new_len = state->snake_len + (grow ? 1 : 0);
    for (int i = new_len - 1; i > 0; i--) {
        state->snake[i] = state->snake[i - 1];
    }
    state->snake[0] = new_head;
    state->snake_len = new_len;
    return grow;
}
