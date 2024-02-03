#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_simulation.h"

void insert_body(Bodies *bodies, Body body) {
    if (bodies->capacity == 0) {
        bodies->data = (Body*)calloc(1, sizeof(Body));
        if (!bodies->data) {
            MEM_ERR;
        }
        bodies->capacity = 1;
    }
    if (bodies->count == bodies->capacity) {
        bodies->capacity *= 2;
        bodies->data = reallocarray(bodies->data, bodies->capacity, sizeof(Body));
    }
    bodies->data[bodies->count] = body;
    bodies->count++;
}

Bodies create_bodies(int n_bodies, int screen_width, int screen_height) {
    Bodies bodies = {0};
    
    Body curr_body = {0};
    float i_pos_x, i_pos_y;

    for (int i = 0; i < n_bodies; i++) {
        scanf("%f", &(curr_body.mass));
        scanf("%f", &(curr_body.density));
        scanf("%f", &i_pos_x);
        scanf("%f", &i_pos_y);
        scanf("%f", &(curr_body.velocity.x));
        scanf("%f", &(curr_body.velocity.y));
        curr_body.radius = cbrtf(curr_body.mass / curr_body.density);
        // Make (0,0) the center of the screen
        curr_body.position = (Vector2){screen_width/2 + i_pos_x,
                                       screen_height/2 + i_pos_y};
        curr_body.acceleration = Vector2Zero();
        curr_body.trail = new_trail();
        curr_body.trail.positions[0] = curr_body.position;
        curr_body.trail.velocities[0] = curr_body.velocity;
        curr_body.trail.accelerations[0] = curr_body.acceleration;
        curr_body.trail.begin = 0;
        curr_body.trail.end = 0;

        insert_body(&bodies, curr_body);
    }

    return bodies;
}

void destroy_bodies(Bodies *bodies) {
    for (size_t i = 0; i < bodies->count; i++) {
        free(bodies->data[i].trail.positions);
        free(bodies->data[i].trail.velocities);
        free(bodies->data[i].trail.accelerations);
    }
    free(bodies->data);
}

Trail new_trail() {
    Trail t = {0};
    t.positions = (Vector2*)malloc(MAX_TRAIL * sizeof(Vector2));
    if (!t.positions) {
        MEM_ERR;
    }
    t.velocities = (Vector2*)malloc(MAX_TRAIL * sizeof(Vector2));
    if (!t.velocities) {
        MEM_ERR;
    }
    t.accelerations = (Vector2*)malloc(MAX_TRAIL * sizeof(Vector2));
    if (!t.accelerations) {
        MEM_ERR;
    }
    return t;
}

void push_to_trail(Trail *trail, Body body) {
    trail->positions[(trail->end + 1) % MAX_TRAIL] = body.position;
    trail->velocities[(trail->end + 1) % MAX_TRAIL] = body.velocity;
    trail->accelerations[(trail->end + 1) % MAX_TRAIL] = body.acceleration;
    trail->end = (trail->end + 1) % MAX_TRAIL;
    if (trail->begin == trail->end) {
        trail->begin = (trail->begin + 1) % MAX_TRAIL;
    }
}

void pop_from_trail(Trail *trail) {
    if (trail->begin == trail->end) { // one element
        return;
    }
    trail->positions[trail->end] = Vector2Zero();
    trail->velocities[trail->end] = Vector2Zero();
    trail->accelerations[trail->end] = Vector2Zero();
    trail->end = mod(trail->end - 1, MAX_TRAIL);
}

void update_bodies(Bodies *bodies) {
    float delta_time = GetFrameTime();
    for (size_t i = 0; i < bodies->count; i++) {
        Body update = bodies->data[i];
        update.position = Vector2Add(update.position, Vector2Scale(update.velocity, delta_time));
        update.velocity = Vector2Add(update.velocity, Vector2Scale(update.acceleration, delta_time));
        update.acceleration = Vector2Zero();
        bodies->data[i] = update;
        push_to_trail(&(bodies->data[i].trail), update);
    }
}

bool check_collision_bodies(Body body1, Body body2) {
    return CheckCollisionCircles(body1.position, body1.radius, body2.position, body2.radius);
}

void handle_2d_collision(Body *body1, Body *body2, float coeff_restitution) {
    // https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional
    float sum_masses        = body1->mass + body2->mass;
    Vector2 diff_velocities = Vector2Subtract(body1->velocity, body2->velocity);
    Vector2 diff_positions  = Vector2Subtract(body1->position, body2->position);

    float scalar1 = Vector2DotProduct(diff_velocities, diff_positions)
                     / Vector2LengthSqr(diff_positions)
                     * 2 * body2->mass / sum_masses
                     * coeff_restitution;
    
    float scalar2 = Vector2DotProduct(Vector2Negate(diff_velocities), Vector2Negate(diff_positions))
                     / Vector2LengthSqr(Vector2Negate(diff_positions))
                     * 2 * body1->mass / sum_masses
                     * coeff_restitution;

    body1->velocity = Vector2Subtract(body1->velocity,
                                      Vector2Scale(diff_positions, scalar1));
    body2->velocity = Vector2Subtract(body2->velocity,
                                      Vector2Scale(Vector2Negate(diff_positions), scalar2));
}

void apply_gravitational_forces(Bodies *bodies, float G,
                                void collision_handler(Body*, Body*, float)) {
    for (size_t i = 0; i < bodies->count; i++) {
        for (size_t j = i+1; j < bodies->count; j++) {
            if (check_collision_bodies(bodies->data[i], bodies->data[j])) {
                collision_handler(&(bodies->data[i]), &(bodies->data[j]),
                                  COEFF_RESTITUTION);
            }

            Vector2 r = Vector2Subtract(bodies->data[i].position, bodies->data[j].position);
            float r_mag_sq = Vector2LengthSqr(r);
            float r_mag = sqrtf(r_mag_sq);
            Vector2 r_norm = (Vector2){r.x / r_mag, r.y / r_mag};

            float acc_i_mag = bodies->data[j].mass / r_mag_sq * G;
            float acc_j_mag = bodies->data[i].mass / r_mag_sq * G;

            bodies->data[i].acceleration.x -= acc_i_mag * r_norm.x;
            bodies->data[i].acceleration.y -= acc_i_mag * r_norm.y;
            bodies->data[j].acceleration.x += acc_j_mag * r_norm.x;
            bodies->data[j].acceleration.y += acc_j_mag * r_norm.y;
        }
    }
}

void rewind_simulation(Bodies *bodies) {
    for (size_t i = 0; i < bodies->count; i++) {
        pop_from_trail(&(bodies->data[i].trail));
        Trail t = bodies->data[i].trail;
        bodies->data[i].position = t.positions[t.end];
        bodies->data[i].velocity = t.velocities[t.end];
        bodies->data[i].acceleration = t.accelerations[t.end];
    }
}
