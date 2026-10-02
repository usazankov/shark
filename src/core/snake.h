#ifndef SNAKE_CORE_SNAKE_H
#define SNAKE_CORE_SNAKE_H

#include "types.h"

/* Клетка, в которую попадёт голова за один шаг в направлении dir.
 * Без проверок границ — коллизии живут в rules.c (FR-8). */
Point snake_next_head(const GameState *state, Direction dir);

#endif /* SNAKE_CORE_SNAKE_H */
