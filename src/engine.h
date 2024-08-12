#ifndef ENGINE
#define ENGINE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "vec2d.h"

#define TEST_CYCLES 20000
#define START_COUNT 400000
#define MAX_COUNT 5000000

#define MONSTER_MAX 400000
#define BULLET_MAX 12000000

#define BULLET_RADIUS 2.0
#define BULLET_LIFE 100
#define BULLET_SPEED 45.0
#define BULLET_DAMAGE 1.5

#define MONSTER_RESET_TIME 10
#define MONSTER_RADIUS 10.0
#define MONSTER_HEALTH 100.0

typedef enum {
    entity_monster,
    entity_bullet,
} entity_type_t;

typedef struct {
    struct {
        vec2d_t pos[MONSTER_MAX];
        vec2d_t direction[MONSTER_MAX];
        double speed[MONSTER_MAX];
        uint64_t time_between_shots[MONSTER_MAX];
        uint64_t time_to_shoot[MONSTER_MAX];
        double health[MONSTER_MAX];

        size_t count;
    } alive_monsters;
    struct {
        vec2d_t pos[MONSTER_MAX];
        vec2d_t direction[MONSTER_MAX];
        double speed[MONSTER_MAX];
        uint64_t time_between_shots[MONSTER_MAX];
        uint64_t time_to_reset[MONSTER_MAX];

        size_t count;
    } dead_monsters;
    struct {
        vec2d_t pos[BULLET_MAX];
        vec2d_t direction[BULLET_MAX];
        uint64_t time_to_live[BULLET_MAX];

        size_t count;
    } bullets;
} engine_t;

#define get_prop_m(i, prop) engine.alive_monsters.prop[i]
#define get_prop_d(i, prop) engine.dead_monsters.prop[i]
#define get_prop_b(i, prop) engine.bullets.prop[i]

void engine_init();

void engine_new_monster(vec2d_t pos, vec2d_t direction, double speed, uint64_t time_between_shots);

void engine_run();

#endif // ENGINE
