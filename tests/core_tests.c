#include "unity.h"

#include "core/game.h"
#include "core/rules.h"
#include "core/types.h"

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
        .input_buffer_size = 2,
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fr2_start_state);
    RUN_TEST(test_game_rejects_bad_config);
    RUN_TEST(test_tick_moves_snake);
    RUN_TEST(test_fr4_ignores_reversal);
    RUN_TEST(test_tick_applies_valid_turn);
    RUN_TEST(test_fr4_rules_opposite);
    RUN_TEST(test_tick_frozen_when_not_running);
    return UNITY_END();
}
