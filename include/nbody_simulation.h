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
        fprintf(stderr, "%s:%d (%s): Mem alloc error\n", __FILE__, __LINE__, __func__); \
        exit(1);                                                                        \
    } while (0)

#define MAX_TRAIL 1500
#define COEFF_RESTITUTION 0.99f
#define GRAVIT_CONSTANT 10.0f
#define mod(a, b) (((a) % (b) + (b)) % (b))

typedef struct Trail {
    Vector2 *positions;
    Vector2 *velocities;    // velocities and accelerations are used
    Vector2 *accelerations; // when rewinding the simulation
    int begin;
    int end;
} Trail;

typedef struct Body {
    float mass;
    float radius;
    float density;
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

// The radius of each body is calculated as the cube root of mass/density
// (square cube law).
Bodies create_bodies(int n_bodies, int screen_width, int screen_height);

// Inserts a new body into the array of bodies.
void insert_body(Bodies *bodies, Body body);

// Deallocates the memory for the bodies and returns NULL.
void destroy_bodies(Bodies *bodies);

// Creates an empty trail.
Trail new_trail();

// Inserts a copy of the body into the trail in a circular array approach.
void push_to_trail(Trail *trail, Body body);

// Removes the copy of a body currently pointed at by the iterator from the trail.
void pop_from_trail(Trail *trail);

// Applies velocity to position and acceleration to velocity in each body.
void update_bodies(Bodies *bodies);

// Returns true if the bodies are colliding.
bool check_collision_bodies(Body body1, Body body2);

// Handles the colision between two 2D bodies, with the coefficient of
// restitution as defined above.
void handle_2d_collision(Body *body1, Body *body2, float coeff_restitution);

// Changes the acceleration of each body according to Newton's gravitational
// law, with the value of G as defined above.
// If there was a collision between any two of the bodies involved, calls
// collision_handler() on them.
void apply_gravitational_forces(Bodies *bodies, float G,
                                void collision_handler(Body*, Body*, float));

// Plays the simulation in reverse using the points already calculated in the trail.
// If the trail is empty, the simulation is paused.
void rewind_simulation(Bodies *bodies);

#endif // NBODY_SIMULATION_H_
