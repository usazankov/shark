#ifndef SNAKE_STORAGE_SCORES_H
#define SNAKE_STORAGE_SCORES_H

/* Рекорды по сложности (FR-14). Скелет: in-memory заглушки, файл — в шаге
 * хранения. */

int scores_load_high(int difficulty);

void scores_save_high(int difficulty, int score);

#endif /* SNAKE_STORAGE_SCORES_H */
