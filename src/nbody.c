#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
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
    }
}

void apply_gravitational_forces(Body *bodies, int n_bodies) {
    for (int i = 0; i < n_bodies; i++) {
        for (int j = i+1; j < n_bodies; j++) {
            Vector2 r = SUBVEC2(bodies[i].position, bodies[j].position);
            float r_mag_sq = r.x * r.x + r.y * r.y;
            float r_mag = sqrt(r_mag_sq);
            Vector2 r_norm = (Vector2){r.x / r_mag, r.y / r_mag};

            float acc_i_mag = bodies[j].mass / r_mag_sq * G;
            float acc_j_mag = bodies[i].mass / r_mag_sq * G;

            bodies[i].acceleration = (Vector2){-acc_i_mag*r_norm.x, -acc_i_mag*r_norm.y};
            bodies[j].acceleration = (Vector2){acc_j_mag*r_norm.x, acc_j_mag*r_norm.y};
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
