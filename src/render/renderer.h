#ifndef SNAKE_RENDERER_H
#define SNAKE_RENDERER_H

#include "config.h"
#include "core/types.h"

/* Нарисовать кадр по состоянию. Только чтение: рендер никогда не мутирует
 * GameState (правило слоёв). */
void draw_game(const GameState *state);

#endif /* SNAKE_RENDERER_H */
