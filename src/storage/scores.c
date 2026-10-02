#include "scores.h"

/* TODO(FR-14): файл рядом с бинарником, fallback %APPDATA%/snake/,
 * раздельно по сложности. ADR-005: обёртка не роняет игру при недоступном
 * хранилище. */

int scores_load_high(int difficulty) {
    (void)difficulty;
    return 0;
}

void scores_save_high(int difficulty, int score) {
    (void)difficulty;
    (void)score;
}
