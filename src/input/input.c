#include "input.h"

#include "raylib.h"

void input_poll(void) {
    /* TODO(FR-3/FR-5): стрелки и WASD → буфер направлений (макс. 2),
     * чтобы быстрые последовательные повороты не терялись. */
}

Direction input_next_direction(void) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        return DIR_UP;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        return DIR_DOWN;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        return DIR_LEFT;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        return DIR_RIGHT;
    }
    return DIR_NONE;
}
