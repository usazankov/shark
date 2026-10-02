# Модули и контракты

- Версия: 1.1 (стек C + Raylib)
- Связанные документы: [Обзор архитектуры](overview.md)

## 1. Структура проекта

```
.
├── CMakeLists.txt           # цель snake (app) + цель core_tests (Unity)
├── CMakePresets.json        # debug/release пресеты (Ninja, MinGW)
├── src/
│   ├── core/                # ЧИСТАЯ ЛОГИКА — без raylib, файлов, системных вызовов
│   │   ├── types.h          # GameState, Direction, Point, GameEvent, GameConfig, TickResult
│   │   ├── game.h/.c        # game_create/game_destroy, tick — единственная точка изменения состояния
│   │   ├── snake.h/.c       # движение, рост, самопересечение
│   │   ├── food.h/.c        # спавн еды на свободной клетке (xorshift-ГПСЧ из состояния)
│   │   └── rules.h/.c       # применение направления, коллизии, счёт, ускорение
│   ├── input/
│   │   ├── buffer.h/.c    # буфер направлений (FR-5) — чистый, без raylib, покрыт тестами
│   │   └── input.h/.c       # raylib-клавиатура (стрелки/WASD, FR-3) → буфер (FR-4/FR-5)
│   ├── render/
│   │   └── renderer.h/.c    # draw_game(const GameState*) через raylib: поле, змейка, еда, HUD, оверлеи
│   ├── storage/
│   │   └── scores.h/.c      # load/save рекордов; файл рядом с exe, fallback %APPDATA%/snake/
│   ├── config.h             # ВСЕ числа игры: размеры, скорости, очки, ускорение, цвета
│   └── main.c               # raylib-окно, game loop, связывание слоёв
└── tests/
    └── core_tests.c         # Unity: тесты core по требованиям (FR-…)
```

## 2. Ключевой контракт ядра

```c
// core/types.h
typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;
typedef struct { int x, y; } Point;

typedef enum { ST_MENU, ST_RUNNING, ST_PAUSED, ST_GAMEOVER } GameStatus;
typedef enum { EV_ATE, EV_DIED, EV_TURNED } GameEventType;
typedef enum { DEATH_WALL, DEATH_SELF } DeathCause;

typedef struct {
    int grid_width, grid_height;
    int start_length;
    Direction start_dir;
    int points_per_food;
    int start_tick_ms, min_tick_ms, speed_up_every, speed_steps; /* FR-11 */
    int input_buffer_size; /* FR-5  */
} GameConfig;

typedef struct { GameEventType type; Point at; DeathCause cause; } GameEvent;

typedef struct {
    int grid_w, grid_h;
    Point *snake;        /* snake[0] — голова; владелец массива — это состояние */
    int snake_len, snake_cap;
    Direction dir;
    Point food;
    int score;
    GameStatus status;
    int tick_interval_ms;
    unsigned rng_state;  /* сид xorshift-ГПСЧ — детерминизм и воспроизведение багов */
    GameConfig cfg;      /* копия конфига: tick читает параметры ускорения */
} GameState;

// core/game.h
GameState *game_create(const GameConfig *cfg);      /* выделение + стартовое состояние */
void       game_destroy(GameState *state);          /* полная очистка */

/* Единственная точка изменения состояния.
   buffered_dir == DIR_NONE (-1/const), если буфер ввода пуст. */
typedef struct { GameEvent events[8]; int event_count; } TickResult;
TickResult tick(GameState *state, Direction buffered_dir);
```

`tick` детерминирован: одинаковые `(state, buffered_dir)` → одинаковый результат
и одинаковый следующий сид.

## 3. Правила модулей

- `core/` не включает `raylib.h` и не делает системных вызовов. Конфигурация
  приходит через `GameConfig` (структура), а не через `#include "config.h"`.
- В `core/` нет «магических чисел» — все константы игры приходят из `GameConfig`.
- `render/` — чистая функция от `const GameState*`: ничего не сохраняет, не
  слушает ввод, не мутирует.
- Side effects (окно, ввод, файлы) живут только в `input/`, `storage/`,
  `render/` и `main.c` — и всё через raylib/stdlib, скрытые за заголовками слоёв.
- Память: владелец `snake` — `GameState`; всё, что `game_create` выделил,
  освобождает `game_destroy`. Остальные слои состояние только читают.

## 4. Куда класть новый код

| Задача                        | Куда                                             |
| ----------------------------- | ------------------------------------------------ |
| Новое правило игры            | `core/rules.c` (+ `snake.c` / `food.c`) + тест в `tests/core_tests.c` |
| Новая отрисовка               | `render/renderer.c`                             |
| Новое действие игрока         | `input/input.c` + проброс в `main.c`            |
| Новое сохраняемое данное      | `storage/scores.c` + тип в `core/types.h`       |
| Смена числа/скорости/размера  | `config.h`, без изменений логики                |
| Новая сложность               | пресет в `config.h` + ключ в storage            |

Если изменение не ложится ни в одну строку таблицы — это сигнал пересмотреть
этот документ, а не класть код мимо правил.
