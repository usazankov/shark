#include "rules.h"

int rules_opposite(Direction a, Direction b) {
    return (a == DIR_UP && b == DIR_DOWN) || (a == DIR_DOWN && b == DIR_UP) ||
           (a == DIR_LEFT && b == DIR_RIGHT) || (a == DIR_RIGHT && b == DIR_LEFT);
}
