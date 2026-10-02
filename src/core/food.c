#include "food.h"

Point food_spawn(GameState *state) {
    /* TODO(FR-6): случайная свободная клетка через xorshift(state->rng_state),
     * не под змейкой. Пока — фиксированная точка, чтобы рендерер имел
     * что рисовать. */
    (void)state;
    Point p = {5, 5};
    return p;
}
