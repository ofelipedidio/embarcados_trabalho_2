#ifndef ENGINE
#define ENGINE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "vec2d.h"

#include "params.h"
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

    /* M"onster shooting */
    uint64_t time_to_shoot;
    uint64_t time_between_shots;
} entity_t;

typedef struct {
    entity_t entities[MAX_COUNT];
    size_t count;
    vec2d_t average_position;
} engine_t;

void engine_init();

void engine_new_monster(vec2d_t pos, vec2d_t direction, double speed, uint64_t time_between_shots);

void engine_new_bullet(vec2d_t pos, vec2d_t direction);

void engine_remove(size_t i);

void engine_run();

#define array_of_structs 1
#if array_of_structs
#define get_prop(i, prop) engine.entities[i].prop
#else
#define get_prop(i, prop) engine.entities.prop[i]
#endif

#endif // ENGINE
