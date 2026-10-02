#include "input.h"

#include "raylib.h"

#include "buffer.h"

void input_reset(void) {
    input_buf_reset();
}

void input_poll(Direction heading) {
    Direction pressed = DIR_NONE;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        pressed = DIR_UP;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        pressed = DIR_DOWN;
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        pressed = DIR_LEFT;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        pressed = DIR_RIGHT;
    }

    if (pressed != DIR_NONE) {
        input_buf_push(pressed, heading);
    }
}

Direction input_next_direction(void) {
    return input_buf_pop();
}
