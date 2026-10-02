#ifndef SNAKE_INPUT_H
#define SNAKE_INPUT_H

#include "core/types.h"

/* Очистить буфер (новая игра, выход из паузы). */
void input_reset(void);

/* Опрос клавиатуры — вызывается КАЖДЫЙ кадр: raylib-нажатия живут один
 * кадр, а тик срабатывает раз в несколько кадров, поэтому нажатия
 * копятся в буфер (FR-5), а не опрашиваются на тике. */
void input_poll(Direction heading);

/* Направление для очередного тика; DIR_NONE, если буфер пуст. */
Direction input_next_direction(void);

#endif /* SNAKE_INPUT_H */
