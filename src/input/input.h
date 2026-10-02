#ifndef SNAKE_INPUT_H
#define SNAKE_INPUT_H

#include "core/types.h"

/* Опрос клавиатуры за кадр (raylib). Скелет: без буфера — буфер
 * направлений на 2 команды придёт с FR-5. */
void input_poll(void);

/* Направление, готовое к применению на тике; DIR_NONE, если ввода нет. */
Direction input_next_direction(void);

#endif /* SNAKE_INPUT_H */
