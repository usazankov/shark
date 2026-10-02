#ifndef SNAKE_CONFIG_H
#define SNAKE_CONFIG_H

#include "core/types.h"

/* Все числа игры (правило модулей: в core — ни одного «магического числа»).
 * Основание значений — docs/requirements.md, FR-11/FR-12. */

#define SNAKE_GRID_W 20
#define SNAKE_GRID_H 20
#define SNAKE_START_LENGTH 3
#define SNAKE_START_DIR DIR_RIGHT
#define SNAKE_POINTS_PER_FOOD 10
#define SNAKE_START_TICK_MS 166 /* ~6 клеток/с (FR-11) */
#define SNAKE_MIN_TICK_MS 67    /* ~15 клеток/с (FR-11) */
#define SNAKE_SPEED_UP_EVERY 50 /* очков между ускорениями (FR-11) */
#define SNAKE_SPEED_STEPS 9     /* ступеней от 166мс до 67мс (FR-11) */
#define SNAKE_INPUT_BUFFER_SIZE 2
#define SNAKE_WRAP_WALLS 1 /* FR-8: 1 = сквозные стены, 0 = смерть о стену */

/* Отрисовка (используется только render/ и main, не core). */
#define SNAKE_CELL_PX 30
#define SNAKE_HUD_PX 40

#endif /* SNAKE_CONFIG_H */
