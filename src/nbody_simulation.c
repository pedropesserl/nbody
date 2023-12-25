#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#define RAYMATH_IMPLEMENTATION
#include "nbody_simulation.h"

Body *create_bodies(int n_bodies) {
    Body *bodies = (Body*)malloc(n_bodies * sizeof(Body));
    if (!bodies)
        MEM_ERR;
    
    float i_pos_x, i_pos_y;

    for (int i = 0; i < n_bodies; i++) {
        printf("Body %d:\n", i);
        printf("    Mass: ");
        scanf("%f", &(bodies[i].mass));
        printf("    Position (x): ");
        scanf("%f", &i_pos_x);
        printf("    Position (y): ");
        scanf("%f", &i_pos_y);
        printf("    Velocity (x): ");
        scanf("%f", &(bodies[i].velocity.x));
        printf("    Velocity (y): ");
        scanf("%f", &(bodies[i].velocity.y));
        bodies[i].radius = cbrtf(bodies[i].mass);
        // Make (0,0) the center of the screen
        bodies[i].position = (Vector2){GetScreenWidth()/2 + i_pos_x,
                                       GetScreenHeight()/2 + i_pos_y};
        bodies[i].acceleration = Vector2Zero();
        bodies[i].trail.points = (Vector2*)malloc(MAX_TRAIL * sizeof(Vector2));
        if (!bodies[i].trail.points)
            MEM_ERR;
        bodies[i].trail.points[0] = bodies[i].position;
        bodies[i].trail.count = 1;
        bodies[i].trail.iterator = 0;
    }

    return bodies;
}

void *destroy_bodies(Body *bodies, int n_bodies) {
    for (int i = 0; i < n_bodies; i++) {
        free(bodies[i].trail.points);
    }
    free(bodies);
    return NULL;
}

void update_bodies(Body *bodies, int n_bodies) {
    float delta_time = GetFrameTime();
    for (int i = 0; i < n_bodies; i++) {
        Vector2 newpos = Vector2Add(bodies[i].position,
                                    Vector2Scale(bodies[i].velocity, delta_time));
        bodies[i].position = newpos;
        int trail_it = bodies[i].trail.iterator;
        bodies[i].trail.points[(trail_it + 1) % MAX_TRAIL] = newpos;
        bodies[i].trail.count += bodies[i].trail.count < MAX_TRAIL;
        bodies[i].trail.iterator = (trail_it + 1) % MAX_TRAIL;
        bodies[i].velocity.x += bodies[i].acceleration.x * delta_time;
        bodies[i].velocity.y += bodies[i].acceleration.y * delta_time;
        bodies[i].acceleration = Vector2Zero();
    }
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

void apply_gravitational_forces(Body *bodies, int n_bodies, float G,
                                void collision_handler(Body*, Body*, float)) {
    for (int i = 0; i < n_bodies; i++) {
        for (int j = i+1; j < n_bodies; j++) {
            if (CheckCollisionCircles(bodies[i].position, bodies[i].radius,
                                      bodies[j].position, bodies[j].radius)) {
                collision_handler(&(bodies[i]), &(bodies[j]), COEFF_RESTITUTION);
            }

            Vector2 r = Vector2Subtract(bodies[i].position, bodies[j].position);
            float r_mag_sq = Vector2LengthSqr(r);
            float r_mag = sqrtf(r_mag_sq);
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
    for (int i = 0; i < n_bodies; i++) {
        DrawCircleV(bodies[i].position, bodies[i].radius, body_colors[i % 10]);
    }
}

void draw_arrows(Body *bodies, int n_bodies) {
    for (int i = 0; i < n_bodies; i++) {
        Vector2 horizontal = (Vector2){1.0f, 0.0f};

        // Velocity arrow
        float cos_v_rotation = Vector2DotProduct(horizontal, bodies[i].velocity)
                               / Vector2Length(bodies[i].velocity);
        float v_rotation = -acosf(cos_v_rotation) * RAD2DEG;
        if (bodies[i].velocity.y > 0) {
            v_rotation = 360.0f - v_rotation;
        }
        Vector2 v_arrow_end = Vector2Add(bodies[i].position, bodies[i].velocity);
        DrawLineEx(bodies[i].position, v_arrow_end, 2.0f, RAYWHITE);
        DrawPoly(v_arrow_end, 3, 4.5f, v_rotation, RAYWHITE);

        // Acceleration arrow
        float cos_a_rotation = Vector2DotProduct(horizontal, bodies[i].acceleration)
                               / Vector2Length(bodies[i].acceleration);
        float a_rotation = -acosf(cos_a_rotation) * RAD2DEG;
        if (bodies[i].acceleration.y > 0) {
            a_rotation = 360.0f - a_rotation;
        }
        Vector2 a_arrow_end = Vector2Add(bodies[i].position, bodies[i].acceleration);
        DrawLineEx(bodies[i].position, a_arrow_end, 2.0f, GRAY);
        DrawPoly(a_arrow_end, 3, 4.5f, a_rotation, GRAY);
    }
}

#define mod(a, b) (((a) % (b) + (b)) % (b))

void draw_trails(Body *bodies, int n_bodies) {
    for (int i = 0; i < n_bodies; i++) {
        int trail_it = bodies[i].trail.iterator;
        int count = bodies[i].trail.count;

        for (int j = 1; j < count; j++) {
            DrawLineV(bodies[i].trail.points[mod(trail_it + j, count)],
                      bodies[i].trail.points[mod(trail_it + 1 + j, count)],
                      body_colors[i % 10]);
        }
    }
}
