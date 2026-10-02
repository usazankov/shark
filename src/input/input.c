#include "input.h"

#include "raylib.h"

#include "buffer.h"

static int raw_events;

void input_reset(void) {
    input_buf_reset();
    raw_events = 0;
}

void input_poll(Direction heading) {
    /* GetKeyPressed() выталкивает события из внутренней очереди raylib
     * (ёмкость 16) — они не привязаны к кадру, в отличие от IsKeyPressed,
     * чей «край» живёт ровно один кадр. */
    int key;
    while ((key = GetKeyPressed()) != 0) {
        raw_events++;

        Direction pressed = DIR_NONE;
        if (key == KEY_UP || key == KEY_W) {
            pressed = DIR_UP;
        } else if (key == KEY_DOWN || key == KEY_S) {
            pressed = DIR_DOWN;
        } else if (key == KEY_LEFT || key == KEY_A) {
            pressed = DIR_LEFT;
        } else if (key == KEY_RIGHT || key == KEY_D) {
            pressed = DIR_RIGHT;
        }

        if (pressed != DIR_NONE) {
            input_buf_push(pressed, heading);
        }
    }
}

Direction input_next_direction(void) {
    return input_buf_pop();
}

int input_events_raw(void) {
    return raw_events;
}
