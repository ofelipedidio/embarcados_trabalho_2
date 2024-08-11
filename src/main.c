#include <stdio.h>
#include <inttypes.h>

#include "vec2d.h"
#include "engine.h"

uint64_t state = 0xa6ea0366;

uint64_t randint() {
	uint64_t x = state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    state = x;
    return x;
}

uint64_t randwithin(uint64_t low, uint64_t high) {
    uint64_t val = low + (randint() % (high - low + 1));
    // fprintf(stderr, "[%lu-%lu] %lu\n", low, high, val);
    return val;
}

int main() {
    fprintf(stderr, "%lu\n", sizeof(entity_t));
    fprintf(stderr, "%lu\n", sizeof(engine_t));
    engine_init();

    for (size_t i = 0; i < START_COUNT; i++) {
        uint64_t val = randint();
        engine_new_monster(
                vec2d_new(
                    ((double) randwithin(0, 1000)) / 10.0,
                    ((double) randwithin(0, 1000)) / 10.0),
                vec2d_norm(vec2d_new(
                    ((double) randwithin(0, 100)) - 50.0,
                    ((double) randwithin(0, 100)) - 50.0)),
                (double) randwithin(2, 6),
                randwithin(3, 25));
    }

    engine_run();
    return 0;
}
