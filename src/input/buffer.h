#ifndef SNAKE_INPUT_BUFFER_H
#define SNAKE_INPUT_BUFFER_H

#include "core/types.h"

/* Буфер направлений (FR-5) — чистый модуль без raylib, поэтому покрыт
 * юнит-тестами в core_tests. Ёмкость — SNAKE_INPUT_BUFFER_SIZE (config.h). */

void input_buf_reset(void);

/* Положить команду. heading — текущее направление змейки; когда очередь
 * пуста, фильтр сравнивает с ним, когда нет — с последней командой в очереди.
 * Отбрасываются: DIR_NONE, дубль опорного направления, разворот на 180°
 * (FR-4 на входе), команда при полной очереди. */
void input_buf_push(Direction dir, Direction heading);

/* Старейшая команда; DIR_NONE, если очередь пуста. */
Direction input_buf_pop(void);

/* Число команд в очереди (для тестов и отладки). */
int input_buf_size(void);

#endif /* SNAKE_INPUT_BUFFER_H */
