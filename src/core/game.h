#ifndef SNAKE_CORE_GAME_H
#define SNAKE_CORE_GAME_H

#include "types.h"

/* Создать игру в стартовом состоянии (FR-1, FR-2). NULL при ошибке
 * выделения памяти или некорректном конфиге. */
GameState *game_create(const GameConfig *cfg);

/* Полная очистка состояния (парна game_create — правило владения памятью). */
void game_destroy(GameState *state);

/* Единственная точка изменения состояния. Детерминирована: одинаковые
 * (state, buffered_dir) дают одинаковый результат и следующий сид.
 *
 * Скелет: применяет направление (без разворота на 180°, FR-4) и сдвигает
 * змейку на клетку. Еда/рост/смерть/счёт — FR-6..FR-8, следующий шаг. */
TickResult tick(GameState *state, Direction buffered_dir);

#endif /* SNAKE_CORE_GAME_H */
