#include "unity.h"

#include "core/game.h"
#include "core/rules.h"
#include "core/snake.h"
#include "core/types.h"
#include "input/buffer.h"

/* Конфиг тестов независим от src/config.h: ядро тестируется через
 * GameConfig (правило модулей №1). */
static GameConfig test_config(void) {
    GameConfig cfg = {
        .grid_width = 20,
        .grid_height = 20,
        .start_length = 3,
        .start_dir = DIR_RIGHT,
        .points_per_food = 10,
        .start_tick_ms = 166,
        .min_tick_ms = 67,
        .speed_up_every = 50,
        .speed_steps = 9,
        .input_buffer_size = 2,
        .wrap_walls = 1, /* продукт по умолчанию — сквозные стены */
    };
    return cfg;
}

void setUp(void) {}

void tearDown(void) {}

/* FR-2: старт — длина 3, голова в центре, направление вправо. */
static void test_fr2_start_state(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);

    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_INT(3, s->snake_len);
    TEST_ASSERT_EQUAL_INT(DIR_RIGHT, s->dir);
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].x); /* центр 20/2 */
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].y);
    TEST_ASSERT_EQUAL_INT(9, s->snake[1].x); /* тело тянется влево */
    TEST_ASSERT_EQUAL_INT(8, s->snake[2].x);
    TEST_ASSERT_EQUAL_INT(ST_RUNNING, s->status);
    TEST_ASSERT_EQUAL_INT(cfg.start_tick_ms, s->tick_interval_ms);

    game_destroy(s);
}

static void test_game_rejects_bad_config(void) {
    TEST_ASSERT_NULL(game_create(NULL));
    GameConfig cfg = test_config();
    cfg.start_length = 0;
    TEST_ASSERT_NULL(game_create(&cfg));
    cfg = test_config();
    cfg.start_length = cfg.grid_width + 1; /* тело не влезает в поле */
    TEST_ASSERT_NULL(game_create(&cfg));
}

/* Тик без ввода сдвигает змейку вперёд: голова +1, тело догоняет. */
static void test_tick_moves_snake(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);

    tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(11, s->snake[0].x); /* бывшая голова */
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].y);
    TEST_ASSERT_EQUAL_INT(3, s->snake_len);

    game_destroy(s);
}

/* FR-4: разворот на 180° игнорируется. */
static void test_fr4_ignores_reversal(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);

    tick(s, DIR_LEFT); /* вправо -> влево = 180°, должно быть отброшено */

    TEST_ASSERT_EQUAL_INT(DIR_RIGHT, s->dir);
    TEST_ASSERT_EQUAL_INT(11, s->snake[0].x); /* продолжила вправо */

    game_destroy(s);
}

static void test_tick_applies_valid_turn(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);

    tick(s, DIR_UP);

    TEST_ASSERT_EQUAL_INT(DIR_UP, s->dir);
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].x);
    TEST_ASSERT_EQUAL_INT(9, s->snake[0].y);

    game_destroy(s);
}

/* FR-4, вспомогательная функция правил. */
static void test_fr4_rules_opposite(void) {
    TEST_ASSERT_TRUE(rules_opposite(DIR_UP, DIR_DOWN));
    TEST_ASSERT_TRUE(rules_opposite(DIR_LEFT, DIR_RIGHT));
    TEST_ASSERT_FALSE(rules_opposite(DIR_UP, DIR_RIGHT));
    TEST_ASSERT_FALSE(rules_opposite(DIR_UP, DIR_UP));
}

/* Тик по не-игровому статусу ничего не меняет (контракт tick). */
static void test_tick_frozen_when_not_running(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    s->status = ST_PAUSED;
    Point head_before = s->snake[0];

    tick(s, DIR_UP);

    TEST_ASSERT_EQUAL_INT(head_before.x, s->snake[0].x);
    TEST_ASSERT_EQUAL_INT(head_before.y, s->snake[0].y);
    TEST_ASSERT_EQUAL_INT(DIR_RIGHT, s->dir);

    game_destroy(s);
}

/* FR-5: команды применяются в порядке нажатия. */
static void test_fr5_fifo_order(void) {
    input_buf_reset();
    input_buf_push(DIR_UP, DIR_RIGHT);
    input_buf_push(DIR_LEFT, DIR_RIGHT);

    TEST_ASSERT_EQUAL_INT(2, input_buf_size());
    TEST_ASSERT_EQUAL_INT(DIR_UP, input_buf_pop());
    TEST_ASSERT_EQUAL_INT(DIR_LEFT, input_buf_pop());
    TEST_ASSERT_EQUAL_INT(DIR_NONE, input_buf_pop());
}

/* FR-5: очередь до 2 команд — третья отбрасывается. */
static void test_fr5_capacity_two(void) {
    input_buf_reset();
    input_buf_push(DIR_UP, DIR_RIGHT);
    input_buf_push(DIR_LEFT, DIR_RIGHT); /* опорная теперь UP: LEFT валидна */
    input_buf_push(DIR_DOWN, DIR_RIGHT); /* очередь полна — отброшена */

    TEST_ASSERT_EQUAL_INT(2, input_buf_size());
}

/* FR-4/FR-5: дубль и разворот на 180° не занимают место в очереди. */
static void test_fr5_drops_same_and_reversal(void) {
    input_buf_reset();
    input_buf_push(DIR_RIGHT, DIR_RIGHT); /* дубль курса — мимо */
    input_buf_push(DIR_LEFT, DIR_RIGHT);  /* 180° — мимо (FR-4) */
    TEST_ASSERT_EQUAL_INT(0, input_buf_size());

    input_buf_push(DIR_UP, DIR_RIGHT);
    input_buf_push(DIR_UP, DIR_RIGHT); /* дубль последней команды — мимо */
    TEST_ASSERT_EQUAL_INT(1, input_buf_size());
}

/* Классика: быстрые UP затем LEFT при курсе RIGHT — это U-поворот двумя
 * валидными командами, а не самоубийство разворотом. */
static void test_fr5_no_180_death_through_buffer(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    input_buf_reset();
    input_buf_push(DIR_UP, DIR_RIGHT);
    input_buf_push(DIR_LEFT, DIR_RIGHT);

    tick(s, input_buf_pop());
    tick(s, input_buf_pop());

    TEST_ASSERT_EQUAL_INT(DIR_LEFT, s->dir);
    TEST_ASSERT_EQUAL_INT(9, s->snake[0].x); /* (10,10) -> UP (10,9) -> LEFT (9,9) */
    TEST_ASSERT_EQUAL_INT(9, s->snake[0].y);
    TEST_ASSERT_EQUAL_INT(ST_RUNNING, s->status);

    game_destroy(s);
}

/* Выложить еду ровно перед головой — тик обязан её съесть. */
static void place_food_ahead(GameState *s) {
    s->food = snake_next_head(s, s->dir);
}

/* FR-6: еда не под змейкой; одинаковый сид — одинаковый спавн. */
static void test_fr6_food_free_and_deterministic(void) {
    GameConfig cfg = test_config();
    GameState *a = game_create(&cfg);
    GameState *b = game_create(&cfg);

    TEST_ASSERT_NOT_NULL(a);
    for (int i = 0; i < a->snake_len; i++) {
        TEST_ASSERT_FALSE(a->food.x == a->snake[i].x && a->food.y == a->snake[i].y);
    }
    TEST_ASSERT_TRUE(a->food.x >= 0 && a->food.x < a->grid_w);
    TEST_ASSERT_TRUE(a->food.y >= 0 && a->food.y < a->grid_h);
    TEST_ASSERT_EQUAL_INT(a->food.x, b->food.x);
    TEST_ASSERT_EQUAL_INT(a->food.y, b->food.y);

    game_destroy(a);
    game_destroy(b);
}

/* FR-7: съедание — рост, счёт, новая еда, событие EV_ATE. */
static void test_fr7_eat_grow_score(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    Point old_food = s->food;
    place_food_ahead(s);

    TickResult r = tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(4, s->snake_len);      /* 3 + 1 */
    TEST_ASSERT_EQUAL_INT(10, s->score);         /* FR-7: +10 */
    TEST_ASSERT_EQUAL_INT(11, s->snake[0].x);    /* голова на еде */
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].y);
    TEST_ASSERT_FALSE(s->food.x == old_food.x && s->food.y == old_food.y);
    TEST_ASSERT_EQUAL_INT(1, r.event_count);
    TEST_ASSERT_EQUAL_INT(EV_ATE, r.events[0].type);
    TEST_ASSERT_EQUAL_INT(ST_RUNNING, s->status);

    game_destroy(s);
}

/* FR-8 (режим walls): стена — Game Over, змейка замирает на месте. */
static void test_fr8_death_by_wall(void) {
    GameConfig cfg = test_config();
    cfg.wrap_walls = 0; /* классические стены */
    GameState *s = game_create(&cfg);
    s->snake[0] = (Point){19, 10};
    s->snake[1] = (Point){18, 10};
    s->snake[2] = (Point){17, 10};

    TickResult r = tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(ST_GAMEOVER, s->status);
    TEST_ASSERT_EQUAL_INT(19, s->snake[0].x); /* не шагнула в стену */
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].y);
    TEST_ASSERT_EQUAL_INT(1, r.event_count);
    TEST_ASSERT_EQUAL_INT(EV_DIED, r.events[0].type);
    TEST_ASSERT_EQUAL_INT(DEATH_WALL, r.events[0].cause);

    game_destroy(s);
}

/* FR-8: собственное тело — Game Over. */
static void test_fr8_death_by_self(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    /* Тело стоит по курсу: новая голова (11,10) попадает в сегмент. */
    s->snake[0] = (Point){10, 10};
    s->snake[1] = (Point){11, 10};
    s->snake[2] = (Point){12, 10};

    TickResult r = tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(ST_GAMEOVER, s->status);
    TEST_ASSERT_EQUAL_INT(DEATH_SELF, r.events[0].cause);

    game_destroy(s);
}

/* FR-8, крайний случай: клетка хвоста освобождается этим же тиком —
 * заход туда легален, если не едим. */
static void test_fr8_tail_vacates(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    /* Хвост (11,10) стоит по курсу головы (10,10) вправо. */
    s->snake[0] = (Point){10, 10};
    s->snake[1] = (Point){9, 10};
    s->snake[2] = (Point){11, 10};

    tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(ST_RUNNING, s->status);
    TEST_ASSERT_EQUAL_INT(3, s->snake_len);
    TEST_ASSERT_EQUAL_INT(11, s->snake[0].x); /* голова на бывшем хвосте */
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].y);

    game_destroy(s);
}

/* FR-8 (режим wrap): выход за границу — вход с противоположной стороны. */
static void test_fr8_wrap_horizontal(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    s->snake[0] = (Point){19, 10};
    s->snake[1] = (Point){18, 10};
    s->snake[2] = (Point){17, 10};

    tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(ST_RUNNING, s->status);
    TEST_ASSERT_EQUAL_INT(0, s->snake[0].x); /* 19 +1 -> перенос на 0 */
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].y);

    game_destroy(s);
}

static void test_fr8_wrap_vertical(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    s->snake[0] = (Point){10, 0};
    s->snake[1] = (Point){11, 0};
    s->snake[2] = (Point){12, 0};
    s->dir = DIR_UP;

    tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(ST_RUNNING, s->status);
    TEST_ASSERT_EQUAL_INT(10, s->snake[0].x);
    TEST_ASSERT_EQUAL_INT(19, s->snake[0].y); /* 0 -1 -> перенос на 19 */

    game_destroy(s);
}

/* FR-8 (режим wrap): телепорт головой на своё тело — смерть, wrap не бессмертие. */
static void test_fr8_wrap_into_self(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);
    s->snake[0] = (Point){19, 10}; /* курс вправо: перенос на (0,10) */
    s->snake[1] = (Point){0, 10};
    s->snake[2] = (Point){1, 10};

    TickResult r = tick(s, DIR_NONE);

    TEST_ASSERT_EQUAL_INT(ST_GAMEOVER, s->status);
    TEST_ASSERT_EQUAL_INT(DEATH_SELF, r.events[0].cause);

    game_destroy(s);
}

/* FR-11: каждые 50 очков тик короче на ступень; ниже min_tick_ms не уходит. */
static void test_fr11_speed_steps_and_floor(void) {
    GameConfig cfg = test_config();
    GameState *s = game_create(&cfg);

    for (int eaten = 0; eaten < 5; eaten++) { /* 5 еды = 50 очков */
        place_food_ahead(s);
        tick(s, DIR_NONE);
    }
    /* ступень = (166-67)/9 = 11; 50 очков = 1 ступень */
    TEST_ASSERT_EQUAL_INT(50, s->score);
    TEST_ASSERT_EQUAL_INT(166 - 11, s->tick_interval_ms);

    s->score = 100000; /* далеко за потолком */
    place_food_ahead(s);
    tick(s, DIR_NONE);
    TEST_ASSERT_EQUAL_INT(67, s->tick_interval_ms); /* FR-11: максимум 15 кл/с */

    game_destroy(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fr2_start_state);
    RUN_TEST(test_game_rejects_bad_config);
    RUN_TEST(test_tick_moves_snake);
    RUN_TEST(test_fr4_ignores_reversal);
    RUN_TEST(test_tick_applies_valid_turn);
    RUN_TEST(test_fr4_rules_opposite);
    RUN_TEST(test_tick_frozen_when_not_running);
    RUN_TEST(test_fr5_fifo_order);
    RUN_TEST(test_fr5_capacity_two);
    RUN_TEST(test_fr5_drops_same_and_reversal);
    RUN_TEST(test_fr5_no_180_death_through_buffer);
    RUN_TEST(test_fr6_food_free_and_deterministic);
    RUN_TEST(test_fr7_eat_grow_score);
    RUN_TEST(test_fr8_death_by_wall);
    RUN_TEST(test_fr8_death_by_self);
    RUN_TEST(test_fr8_tail_vacates);
    RUN_TEST(test_fr8_wrap_horizontal);
    RUN_TEST(test_fr8_wrap_vertical);
    RUN_TEST(test_fr8_wrap_into_self);
    RUN_TEST(test_fr11_speed_steps_and_floor);
    return UNITY_END();
}
