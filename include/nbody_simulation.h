#ifndef NBODY_SIMULATION_H_
#define NBODY_SIMULATION_H_

#include "raylib.h"

#define USAGE do {                                                     \
        fprintf(stderr, "Usage: %s\n", argv[0]);                       \
        fprintf(stderr, "       OR\n");                                \
        fprintf(stderr, "       %s -n <number_of_bodies>\n", argv[0]); \
        exit(1);                                                       \
    } while (0)

#define MEM_ERR do {                                                                    \
        fprintf(stderr, "Mem alloc error: %s:%d (%s)\n", __FILE__, __LINE__, __func__); \
        exit(1);                                                                        \
    } while (0)

typedef struct Trail {
    Vector2 *points;
    int count;
    int iterator;
} Trail;

typedef struct Body {
    float mass;
    float radius;
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Trail trail;
} Body;

typedef struct Bodies {
    Body *data;
    size_t count;
    size_t capacity;
} Bodies;

// Creates an empty array of bodies.
#define MAX_TRAIL 1500
Bodies create_bodies_array();

Bodies create_bodies(int n_bodies);

// Inserts a new body into the array of bodies.
// The radius of each body is calculated as the cube root of its mass
// (square cube law, assuming all densities are the same).
void insert_body(Bodies *bodies, Body body);

// Deallocates the memory for the bodies and returns NULL.
void destroy_bodies(Bodies *bodies);

// Applies velocity to position and acceleration to velocity in each body.
void update_bodies(Bodies *bodies);

// Handles the colision between two 2D bodies, with the coefficient of
// restitution as defined below.
#define COEFF_RESTITUTION 0.99f
void handle_2d_collision(Body *body1, Body *body2, float coeff_restitution);

// Changes the acceleration of each body according to Newton's gravitational
// law, with the value of G as defined below.
// If there was a collision between any two of the bodies involved, calls
// collision_handler() on them.
#define GRAVIT_CONSTANT 10.0f
void apply_gravitational_forces(Bodies *bodies, float G,
                                void collision_handler(Body*, Body*, float));

#endif // NBODY_SIMULATION_H_
