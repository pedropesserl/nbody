#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_simulation.h"

Bodies create_bodies_array() {
    return (Bodies){0};
}

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
    bodies->count += 1;
}

Bodies create_bodies(int n_bodies) {
    Bodies bodies = {0};
    
    Body curr_body = {0};
    float i_pos_x, i_pos_y;

    for (int i = 0; i < n_bodies; i++) {
        scanf("%f", &(curr_body.mass));
        scanf("%f", &i_pos_x);
        scanf("%f", &i_pos_y);
        scanf("%f", &(curr_body.velocity.x));
        scanf("%f", &(curr_body.velocity.y));
        curr_body.radius = cbrtf(curr_body.mass);
        // Make (0,0) the center of the screen
        curr_body.position = (Vector2){GetScreenWidth()/2 + i_pos_x,
                                       GetScreenHeight()/2 + i_pos_y};
        curr_body.acceleration = Vector2Zero();
        curr_body.trail.points = (Vector2*)malloc(MAX_TRAIL * sizeof(Vector2));
        if (!curr_body.trail.points) {
            MEM_ERR;
        }
        curr_body.trail.points[0] = curr_body.position;
        curr_body.trail.count = 1;
        curr_body.trail.iterator = 0;

        insert_body(&bodies, curr_body);
    }

    return bodies;
}

void destroy_bodies(Bodies *bodies) {
    for (size_t i = 0; i < bodies->count; i++) {
        free(bodies->data[i].trail.points);
    }
    free(bodies->data);
}

void update_bodies(Bodies *bodies) {
    float delta_time = GetFrameTime();
    for (size_t i = 0; i < bodies->count; i++) {
        Vector2 newpos = Vector2Add(bodies->data[i].position,
                                    Vector2Scale(bodies->data[i].velocity, delta_time));
        int trail_it = bodies->data[i].trail.iterator;
        bodies->data[i].trail.points[(trail_it + 1) % MAX_TRAIL] = newpos;
        bodies->data[i].trail.count += bodies->data[i].trail.count < MAX_TRAIL;
        bodies->data[i].trail.iterator = (trail_it + 1) % MAX_TRAIL;

        bodies->data[i].position = newpos;
        bodies->data[i].velocity = Vector2Add(bodies->data[i].velocity,
                                              Vector2Scale(bodies->data[i].acceleration,
                                                           delta_time));
        bodies->data[i].acceleration = Vector2Zero();
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
    
    float scalar2 = Vector2DotProduct(Vector2Negate(diff_velocities),
                                      Vector2Negate(diff_positions))
                     / Vector2LengthSqr(Vector2Negate(diff_positions))
                     * 2 * body1->mass / sum_masses
                     * coeff_restitution;

    body1->velocity = Vector2Subtract(body1->velocity,
                                      Vector2Scale(diff_positions, scalar1));
    body2->velocity = Vector2Subtract(body2->velocity,
                                      Vector2Scale(Vector2Negate(diff_positions),
                                                   scalar2));
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
