#ifndef SNAKE_CORE_SNAKE_H
#define SNAKE_CORE_SNAKE_H

#include "types.h"

/* Клетка, в которую попадёт голова за один шаг в направлении dir.
 * Без проверок границ — коллизии живут в rules.c (FR-8). */
Point snake_next_head(const GameState *state, Direction dir);

/* Продвинуть змейку: тело сдвигается, голова — на new_head. grow=1 —
 * удлинение на сегмент (без отброса хвоста); при неудаче realloc рост
 * молча отменяется, движение продолжается. Возвращает 1, если рост
 * произошёл. */
int snake_advance(GameState *state, Point new_head, int grow);

#endif /* SNAKE_CORE_SNAKE_H */
