# Модули и контракты

- Версия: 1.0
- Связанные документы: [Обзор архитектуры](overview.md)

## 1. Структура `src/`

```
src/
├── core/               # ЧИСТАЯ логика — без DOM, canvas, localStorage, таймеров
│   ├── types.ts        # GameState, Direction, Point, GameEvent, GameConfig
│   ├── snake.ts        # движение, рост, самопересечение
│   ├── food.ts         # спавн еды на свободной клетке (ГПСЧ с сидом)
│   ├── rules.ts        # тик: применить направление, коллизии, счёт, скорость
│   └── game.ts         # createGame(config), tick(state, inputDir) → {state, events}
├── input/
│   ├── keyboard.ts     # стрелки/WASD → буфер направлений (макс. 2)
│   └── touch.ts        # свайпы → тот же буфер
├── render/
│   └── canvas.ts       # draw(state, ctx): поле, змейка, еда, HUD, оверлеи
├── storage/
│   └── scores.ts       # getHighScore(difficulty) / setHighScore(...), safe-обёртка
├── config.ts           # ВСЕ числа игры: размеры, скорости, очки, ускорение
└── main.ts             # bootstrap: createGame, game loop, связывание слоёв
```

## 2. Ключевой контракт ядра

```ts
type Direction = 'up' | 'down' | 'left' | 'right';
type Point = { x: number; y: number };

interface GameState {
  grid: { width: number; height: number };
  snake: Point[];        // snake[0] — голова
  dir: Direction;
  food: Point;
  score: number;
  status: 'menu' | 'running' | 'paused' | 'gameover';
  tickIntervalMs: number;
  rngState: number;      // сид ГПСЧ — детерминизм и воспроизведение багов
}

type GameEvent =
  | { type: 'ate'; at: Point }
  | { type: 'died'; cause: 'wall' | 'self' }
  | { type: 'turned'; dir: Direction };

interface GameConfig {
  gridWidth: number; gridHeight: number;
  startLength: number; startDir: Direction;
  pointsPerFood: number;
  startTickMs: number; minTickMs: number; speedUpEvery: number; // FR-11
  inputBufferSize: number; // FR-5
}

// Единственная точка изменения состояния.
tick(state: GameState, bufferedDir: Direction | null): { state: GameState; events: GameEvent[] };
```

`tick` чистая по поведению: одинаковые `(state, bufferedDir)` → одинаковый
результат; состояние обновляется предсказуемо и только здесь.

## 3. Правила модулей

- `core/` не импортирует ничего вне `core/`. Конфигурация прокидывается через
  `GameConfig`, а не импортируется из `config.ts`.
- В `core/` нет «магических чисел» — все константы игры приходят из `GameConfig`.
- `render/` — чистая функция от состояния: ничего не сохраняет, не слушает,
  не мутирует.
- Side effects окружения (DOM API, таймеры, хранилище) живут только в `input/`,
  `storage/` и `main.ts`.

## 4. Куда класть новый код

| Задача                        | Куда                                             |
| ----------------------------- | ------------------------------------------------ |
| Новое правило игры            | `core/rules.ts` (+ `core/snake.ts` / `food.ts`) + тест |
| Новая отрисовка               | `render/canvas.ts` или новый файл в `render/`    |
| Новое действие игрока         | `input/` + проброс в `main.ts`                   |
| Новое сохраняемое данное      | `storage/` + тип в `core/types.ts`               |
| Смена числа/скорости/размера  | `config.ts`, без изменений логики                |
| Новая сложность               | пресет в `config.ts` + ключ в storage            |

Если изменение не ложится ни в одну строку таблицы — это сигнал пересмотреть
этот документ, а не класть код мимо правил.
