#ifndef SNAKE_CORE_FOOD_H
#define SNAKE_CORE_FOOD_H

#include "types.h"

/* Поставить еду на случайную свободную клетку, обновив сид в состоянии
 * (FR-6). Скелет: заглушка, реализация — в шаге core-логики. */
Point food_spawn(GameState *state);

#endif /* SNAKE_CORE_FOOD_H */
