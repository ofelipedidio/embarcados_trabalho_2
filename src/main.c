#include <stdio.h>
#include <inttypes.h>

#include "vec2d.h"

#define START_COUNT 256
#define MAX_COUNT 1024

#define BULLET_RADIUS 10.0
#define BULLET_LIFE 1000
#define BULLET_SPEED 25.0

#define TIME_BETWEEN_SHOTS_MIN 100
#define TIME_BETWEEN_SHOTS_MAX 1000

#define MONSTER_SPEED_MIN 2.0
#define MONSTER_SPEED_MAX 8.0

#define MONSTER_RESET_TIME 10000
#define TARGET_TO_AVERAGE_SPEED 4.0

typedef enum {
    entity_monster,
    entity_bullet,
} entity_type_t;

typedef struct {
    entity_type_t type[MAX_COUNT];

    /* Life */
    int alive[MAX_COUNT];
    uint64_t time_to_live[MAX_COUNT];
    uint64_t time_to_reset[MAX_COUNT];

    /* Position and movement */
    vec2d_t posision[MAX_COUNT];
    vec2d_t target[MAX_COUNT];
    double speed[MAX_COUNT];
    double radius[MAX_COUNT];

    /* Monster shooting */
    uint64_t time_to_shoot[MAX_COUNT];
    uint64_t time_between_shots[MAX_COUNT];
} entities_t;

typedef struct {
    /* Life */
    int alive;
    uint64_t time_to_live;
    uint64_t time_to_reset;

    /* Position and movement */
    vec2d_t posision;
    vec2d_t target;
    double speed;
    double radius;

    /* Monster shooting */
    uint64_t time_to_shoot;
    uint64_t time_between_shots;
} monster_t;

typedef struct {
    uint64_t time_to_live;
    vec2d_t posision;
    vec2d_t target;
    double speed;
    double radius;
} bullet_t;

typedef struct {
    entity_type_t type;

    /* Life */
    int alive;
    uint64_t time_to_live;
    uint64_t time_to_reset;

    /* Position and movement */
    vec2d_t posision;
    vec2d_t target;
    double speed;
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

engine_t engine;

void init_engine() {
    engine.count = 0;
}

void create_bullet(vec2d_t position, vec2d_t target) {
    engine.entities[engine.count].type = entity_bullet;

    engine.entities[engine.count].alive = 1;
    engine.entities[engine.count].time_to_live = BULLET_LIFE;
    engine.entities[engine.count].time_to_reset = 0;

    engine.entities[engine.count].posision = position;
    engine.entities[engine.count].target = target;
    engine.entities[engine.count].radius = BULLET_RADIUS;
    engine.entities[engine.count].speed = BULLET_SPEED;

    engine.entities[engine.count].time_to_shoot = 0;
    engine.entities[engine.count].time_between_shots = 0;
}

void update_entity(entity_t *entity) {
    if (!entity->alive) {
        entity->time_to_reset--;
        if (entity->time_to_reset == 0) {
            entity->alive = 1;
        }
        return;
    }

    if (entity->time_to_live == 0) {
        return;
    }
    entity->time_to_live--;

    vec2d_t velocity_vector = vec2d_mul_scl(
            vec2d_norm(
                vec2d_sub(entity->target, entity->posision)),
            entity->speed);
    entity->posision = vec2d_add(entity->posision, velocity_vector);

    // Make sure bullet is always following its path
    if (entity->type == entity_bullet) {
        entity->target = vec2d_add(entity->target, velocity_vector);
    }

    if (entity->type == entity_monster) {
        entity->time_to_shoot--;
        if (entity->time_to_shoot == 0) {
            entity->time_to_shoot = entity->time_between_shots;
            create_bullet(entity->posision, entity->target);
        }
    }

    if (entity->type == entity_bullet) {
        for (size_t j = 0; j < engine.count; j++) {
            entity_t *other = &engine.entities[j];
            if (other->type == entity_monster && other->alive) {
                double distance = vec2d_len(vec2d_sub(entity->posision, other->posision));
                if (distance <= (entity->radius + other->radius)) {
                    other->alive = 0;
                    other->time_to_reset = MONSTER_RESET_TIME;
                }
            }
        }
    }
}

#define array_of_structs 1
#if array_of_structs
#define get_prop(i, prop) engine.entities[i].prop
#else
#define get_prop(i, prop) engine.entities.prop[i]
#endif

void update() {
    /*
     *
     *            ┌───────────────────────────────────────┐
     *            v                             {ttl > 0} │
     *  ┌──────────────────┐                              │             ┌───────────────────┐
     *  │ alive = 1        ├──────────────────────────────┴────────────>│ alive = 0         │
     *  │ time_to_live > 0 │ on tick: time_to_live--        {ttl == 0}  │ time_to_live = 0  │
     *  └──────────────────┘                                            │ time_to_reset > 0 │
     *                                                                  └───────────────────┘
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     */

    for (size_t i = 0; i < engine.count; i++) {
        // Handle entity life
        if (get_prop(i, alive)) {
            if (get_prop(i, time_to_live) == 0) {
                get_prop(i, alive) = 0;
            }
        }
        if (get_prop(i, alive)) {
            get_prop(i, time_to_live)--;
        } else {
            if (get_prop(i, type) == entity_monster) {
                if (get_prop(i, time_to_reset) == 0) {
                    get_prop(i, alive) = 1;
                } else {
                    get_prop(i, time_to_reset)--;
                }
            }
        }
    }
}

void run_game() {
    while (1) {
        update();
    }
}

int main() {
    fprintf(stderr, "%lu", sizeof(entity_t));
    init_engine();
    run_game();
    return 0;
}

