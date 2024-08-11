#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "vec2d.h"

engine_t engine;

void engine_init() {
    engine.count = 0;
}

void engine_new_monster(vec2d_t pos, vec2d_t direction, double speed, uint64_t time_between_shots) {
    engine.entities[engine.count].type = entity_monster;
    engine.entities[engine.count].alive = 1;
    engine.entities[engine.count].health = MONSTER_HEALTH;
    engine.entities[engine.count].time_to_live = 0;
    engine.entities[engine.count].time_to_reset = 0;
    engine.entities[engine.count].position = pos;
    engine.entities[engine.count].direction = direction;
    engine.entities[engine.count].speed = speed;
    engine.entities[engine.count].radius = MONSTER_RADIUS;
    engine.entities[engine.count].time_to_shoot = time_between_shots;
    engine.entities[engine.count].time_between_shots = time_between_shots;
    engine.count++;
}

void engine_new_bullet(vec2d_t pos, vec2d_t direction) {
    engine.entities[engine.count].type = entity_bullet;
    engine.entities[engine.count].alive = 1;
    engine.entities[engine.count].health = 0;
    engine.entities[engine.count].time_to_live = BULLET_LIFE;
    engine.entities[engine.count].time_to_reset = 0;
    engine.entities[engine.count].position = pos;
    engine.entities[engine.count].direction = direction;
    engine.entities[engine.count].speed = BULLET_SPEED;
    engine.entities[engine.count].radius = BULLET_RADIUS;
    engine.entities[engine.count].time_to_shoot = 0;
    engine.entities[engine.count].time_between_shots = 0;
    engine.count++;
}

void engine_remove(size_t i) {
    if (i >= engine.count) {
        return;
    }
    engine.count--;
    engine.entities[i] = engine.entities[engine.count];
}

void handle_movement() {
    // fprintf(stderr, "handle_movement()\n");
    for (size_t i = 0; i < engine.count; i++) {
        if (get_prop(i, type) == entity_monster && !get_prop(i, alive)) {
            continue;
        }
        vec2d_t next_point = vec2d_add(
                get_prop(i, position), 
                vec2d_mul_scl(
                    get_prop(i, direction),
                    get_prop(i, speed)));
        get_prop(i, position) = next_point;
    }
}

void handle_bullet_collision() {
    // fprintf(stderr, "handle_bullet_collision()\n");
    for (size_t i = 0; i < engine.count; i++) {
        if (get_prop(i, type) == entity_bullet) {
            for (size_t j = 0; j < engine.count; j++) {
                if (get_prop(j, type) == entity_monster && get_prop(j, alive)) {
                    double distance_to_collide = get_prop(i, radius) + get_prop(j, radius);
                    double distance = vec2d_len(vec2d_sub(get_prop(i, position), get_prop(j, position)));
                    if (distance <= distance_to_collide) {
                        get_prop(j, health) -= BULLET_DAMAGE;
                        if (get_prop(j, health) <= 0.0) {
                            get_prop(j, alive) = 0;
                            get_prop(j, time_to_reset) = MONSTER_RESET_TIME;
                        }
                        engine_remove(i);
                        if (i != 0) {
                            i--;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void handle_shooting() {
    // fprintf(stderr, "handle_shooting()\n");
    for (size_t i = 0; i < engine.count; i++) {
        if (get_prop(i, type) == entity_monster && get_prop(i, alive)) {
            if (get_prop(i, time_to_shoot) == 0) {
                get_prop(i, time_to_shoot) = get_prop(i, time_between_shots);
                engine_new_bullet(get_prop(i, position), get_prop(i, direction));
            }
            get_prop(i, time_to_shoot)--;
        }
    }
}

void handle_monster_direction_change(vec2d_t average_pos) {
    // fprintf(stderr, "handle_monster_direction_change()\n");
    for (size_t i = 0; i < engine.count; i++) {
        if (get_prop(i, type) == entity_monster && get_prop(i, alive)) {
            vec2d_t next_point = vec2d_add(
                    get_prop(i, position), 
                    vec2d_mul_scl(
                        get_prop(i, direction),
                        get_prop(i, speed)));
            vec2d_t next_direction = vec2d_norm(vec2d_add(
                    vec2d_mul_scl(average_pos, 0.05),
                    vec2d_mul_scl(next_point, 0.95)));
            get_prop(i, direction) = next_direction;
        }
    }
}

void handle_life() {
    // fprintf(stderr, "handle_life()\n");
    for (size_t i = 0; i < engine.count; i++) {
        if (get_prop(i, type) == entity_monster && !get_prop(i, alive)) {
            if (get_prop(i, time_to_reset) == 0) {
                get_prop(i, alive) = 1;
            }
            get_prop(i, time_to_reset)--;
        }
        if (get_prop(i, type) == entity_bullet) {
            if (get_prop(i, time_to_live) == 0) {
                engine_remove(i);
                if (i != 0) {
                    i--;
                }
            }
            get_prop(i, time_to_live)--;
        }
    }
}

void engine_run() {
    vec2d_t average_pos;
    uint64_t monster_count;
    for (size_t iter = 0; iter < TEST_CYCLES; iter++) {
        average_pos = vec2d_new(0, 0);
        monster_count = 0;
        for (size_t i = 0; i < engine.count; i++) {
            if (get_prop(i, type) == entity_monster && get_prop(i, alive)) {
                average_pos = vec2d_add(average_pos, get_prop(i, position));
                monster_count++;
            }
        }
        if (monster_count > 0) {
            average_pos = vec2d_div_scl(average_pos, (double) monster_count);
        }

        handle_movement();
        handle_bullet_collision();
        handle_shooting();
        handle_monster_direction_change(average_pos);
        handle_life();

        if (iter % 100 == 0) {
            fprintf(stderr, "[%lu] %lu entities\n", iter, engine.count);

            /*
            uint64_t monsters_alive = 0;
            uint64_t monsters_dead = 0;
            uint64_t bullets = 0;
            for (size_t i = 0; i < engine.count; i++) {
                if (get_prop(i, type) == entity_monster) {
                    if (get_prop(i, alive)) {
                        monsters_alive++;
                    } else {
                        monsters_dead++;
                    }
                }
                if (get_prop(i, type) == entity_bullet) {
                    bullets++;
                }
            }

            fprintf(stderr, "alive: %lu | dead: %lu | bullets: %lu\n", monsters_alive, monsters_dead, bullets);
            fprintf(stderr, "(%.2f, %.2f)\n", average_pos.x, average_pos.y);
            */
        }
    }
}

