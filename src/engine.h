#ifndef ENGINE
#define ENGINE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "vec2d.h"

#define TEST_CYCLES 200
#define START_COUNT 400000
#define MAX_COUNT 5000000

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

#define array_of_structs 0
#if array_of_structs

#define get_prop(i, prop) engine.entities[i].prop

typedef struct {
    entity_type_t type;

    /* Life */
    int alive;
    double health;
    uint64_t time_to_live;
    uint64_t time_to_reset;

    /* Position and movement */
    vec2d_t position;
    vec2d_t direction;
    double speed;

    /* Size */
    double radius;

    /* Monster shooting */
    uint64_t time_to_shoot;
    uint64_t time_between_shots;
} entity_t;

typedef struct {
    entity_t entities[MAX_COUNT];
    size_t count;
    vec2d_t average_position;
} engine_t;

#else

#define get_prop(i, prop) engine.entities.prop[i]

typedef struct {
    entity_type_t type[MAX_COUNT];

    /* Life */
    int alive[MAX_COUNT];
    double health[MAX_COUNT];
    uint64_t time_to_live[MAX_COUNT];
    uint64_t time_to_reset[MAX_COUNT];

    /* Position and movement */
    vec2d_t position[MAX_COUNT];
    vec2d_t direction[MAX_COUNT];
    double speed[MAX_COUNT];

    /* Size */
    double radius[MAX_COUNT];

    /* Monster shooting */
    uint64_t time_to_shoot[MAX_COUNT];
    uint64_t time_between_shots[MAX_COUNT];
} entity_t;

typedef struct {
    entity_t entities;
    size_t count;
    vec2d_t average_position;
} engine_t;

#endif

void engine_init();

void engine_new_monster(vec2d_t pos, vec2d_t direction, double speed, uint64_t time_between_shots);

void engine_new_bullet(vec2d_t pos, vec2d_t direction);

void engine_remove(size_t i);

void engine_run();

#endif // ENGINE
