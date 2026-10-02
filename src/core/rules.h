#ifndef SNAKE_CORE_RULES_H
#define SNAKE_CORE_RULES_H

#include "types.h"

/* True, если b — разворот на 180° относительно a (FR-4: такой ввод
 * игнорируется). */
int rules_opposite(Direction a, Direction b);

#endif /* SNAKE_CORE_RULES_H */
