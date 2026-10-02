#ifndef SNAKE_CORE_TYPES_H
#define SNAKE_CORE_TYPES_H

/* Типы ядра. Этот заголовок не тянет ничего стороннего: ядро не знает
 * о raylib, окнах и файлах (docs/architecture/overview.md, слой core). */

typedef enum {
    DIR_NONE = -1, /* «направления нет» — буфер ввода пуст */
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    int x, y;
} Point;

typedef enum {
    ST_MENU,
    ST_RUNNING,
    ST_PAUSED,
    ST_GAMEOVER
} GameStatus;

typedef enum {
    EV_ATE,
    EV_DIED,
    EV_TURNED
} GameEventType;

typedef enum {
    DEATH_WALL,
    DEATH_SELF
} DeathCause;

/* Все числа игры приходят в ядро этой структурой (источник — src/config.h).
 * В core нет «магических чисел» — правило модулей №2. */
typedef struct {
    int grid_width, grid_height;
    int start_length;
    Direction start_dir;
    int points_per_food;
    int start_tick_ms;   /* FR-11: стартовая скорость */
    int min_tick_ms;     /* FR-11: потолок скорости */
    int speed_up_every;  /* FR-11: очков между ускорениями */
    int speed_steps;     /* FR-11: ступеней от старта до потолка */
    int input_buffer_size; /* FR-5 */
} GameConfig;

typedef struct {
    GameEventType type;
    Point at;           /* EV_ATE: клетка съеденной еды */
    DeathCause cause;   /* EV_DIED */
} GameEvent;

typedef struct {
    GameEvent events[8];
    int event_count;
} TickResult;

typedef struct {
    int grid_w, grid_h;
    Point *snake;      /* snake[0] — голова; владелец массива — это состояние */
    int snake_len;
    int snake_cap;
    Direction dir;
    Point food;
    int score;
    GameStatus status;
    int tick_interval_ms;
    unsigned rng_state; /* сид xorshift-ГПСЧ: детерминизм и воспроизведение
                           багов по сиду (ADR-006) */
    GameConfig cfg;    /* копия конфига: tick читает параметры ускорения */
} GameState;

#endif /* SNAKE_CORE_TYPES_H */
