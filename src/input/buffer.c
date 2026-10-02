#include "buffer.h"

#include "config.h"
#include "core/rules.h"

#define QUEUE_CAP SNAKE_INPUT_BUFFER_SIZE

static Direction queue[QUEUE_CAP];
static int head;
static int count;

void input_buf_reset(void) {
    head = 0;
    count = 0;
}

void input_buf_push(Direction dir, Direction heading) {
    if (dir == DIR_NONE) {
        return;
    }

    /* Опорное направление: последняя команда очереди, иначе текущий курс.
     * Именно с ним выполнится эта команда — с ним и сверяемся. */
    Direction reference = heading;
    if (count > 0) {
        reference = queue[(head + count - 1) % QUEUE_CAP];
    }

    if (dir == reference || rules_opposite(dir, reference)) {
        return; /* FR-4: разворот не попадает в очередь даже на входе */
    }
    if (count >= QUEUE_CAP) {
        return; /* FR-5: очередь до 2 команд */
    }

    queue[(head + count) % QUEUE_CAP] = dir;
    count++;
}

Direction input_buf_pop(void) {
    if (count == 0) {
        return DIR_NONE;
    }
    Direction dir = queue[head];
    head = (head + 1) % QUEUE_CAP;
    count--;
    return dir;
}

int input_buf_size(void) {
    return count;
}
