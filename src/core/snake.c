#include "snake.h"

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
