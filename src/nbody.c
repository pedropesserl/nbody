#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "vector.h"
#include "nbody.h"

Body *create_bodies(int n_bodies) {
    Body *bodies = (Body*)malloc(n_bodies * sizeof(Body));
    if (!bodies)
        MEM_ERR;

    for (long i = 0; i < n_bodies; i++) {
        printf("Body %ld:\n", i);
        printf("    Mass: ");
        scanf("%f", &(bodies[i].mass));
        bodies[i].radius = cbrtf(bodies[i].mass);
        printf("    Position (x): ");
        scanf("%f", &(bodies[i].position.x));
        printf("    Position (y): ");
        scanf("%f", &(bodies[i].position.y));
        printf("    Velocity (x): ");
        scanf("%f", &(bodies[i].velocity.x));
        printf("    Velocity (y): ");
        scanf("%f", &(bodies[i].velocity.y));
        bodies[i].acceleration = (Vector2){0};
    }

    return bodies;
}

void update_bodies(Body *bodies, int n_bodies) {
    float delta_time = GetFrameTime();
    for (int i = 0; i < n_bodies; i++) {
        bodies[i].position.x += bodies[i].velocity.x * delta_time;
        bodies[i].position.y += bodies[i].velocity.y * delta_time;
        bodies[i].velocity.x += bodies[i].acceleration.x * delta_time;
        bodies[i].velocity.y += bodies[i].acceleration.y * delta_time;
        bodies[i].acceleration = (Vector2){0.0f, 0.0f};
    }
}

void handle_2d_collision(Body *body1, Body *body2, float coeff_restitution) {
    // https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional

    float sum_masses        = body1->mass + body2->mass;
    Vector2 diff_velocities = sub_vec2(body1->velocity, body2->velocity);
    Vector2 diff_positions  = sub_vec2(body1->position, body2->position);

    float scalar1 = dot_vec2(diff_velocities, diff_positions)
                     / mag_vec2_sq(diff_positions)
                     * 2 * body2->mass / sum_masses
                     * coeff_restitution;
    
    float scalar2 = dot_vec2(neg_vec2(diff_velocities), neg_vec2(diff_positions))
                     / mag_vec2_sq(neg_vec2(diff_positions))
                     * 2 * body1->mass / sum_masses
                     * coeff_restitution;

    body1->velocity = sub_vec2(body1->velocity,
                               mul_vec2_scalar(diff_positions, scalar1));
    body2->velocity = sub_vec2(body2->velocity,
                               mul_vec2_scalar(neg_vec2(diff_positions), scalar2));
}

void apply_gravitational_forces(Body *bodies, int n_bodies, float G,
                                void collision_handler(Body*, Body*, float)) {
    for (int i = 0; i < n_bodies; i++) {
        for (int j = i+1; j < n_bodies; j++) {
            if (CheckCollisionCircles(bodies[i].position, bodies[i].radius,
                                      bodies[j].position, bodies[j].radius)) {
                collision_handler(&(bodies[i]), &(bodies[j]), COEFF_RESTITUTION);
            }

            Vector2 r = sub_vec2(bodies[i].position, bodies[j].position);
            float r_mag_sq = r.x * r.x + r.y * r.y;
            float r_mag = sqrt(r_mag_sq);
            Vector2 r_norm = (Vector2){r.x / r_mag, r.y / r_mag};

            float acc_i_mag = bodies[j].mass / r_mag_sq * G;
            float acc_j_mag = bodies[i].mass / r_mag_sq * G;

            bodies[i].acceleration.x -= acc_i_mag * r_norm.x;
            bodies[i].acceleration.y -= acc_i_mag * r_norm.y;
            bodies[j].acceleration.x += acc_j_mag * r_norm.x;
            bodies[j].acceleration.y += acc_j_mag * r_norm.y;
        }
    }
}

Color body_colors[10] = {
    RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE, PINK, GOLD, LIME, DARKBLUE
};

void draw_bodies(Body *bodies, int n_bodies) {
    Rectangle screen = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
    for (int i = 0; i < n_bodies; i++) {
        if (CheckCollisionCircleRec(bodies[i].position, bodies[i].radius, screen)) {
            DrawCircleV(bodies[i].position, bodies[i].radius, body_colors[i % 10]);
        }
    }
}
