#ifndef NBODY_SIMULATION_H_
#define NBODY_SIMULATION_H_

#define USAGE do {                                                  \
        fprintf(stderr, "Usage: %s <number_of_bodies>\n", argv[0]); \
        exit(1);                                                    \
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

extern Color body_colors[10];

// Returns an array of bodies of size n_bodies with data passed from stdin.
// The radius of each body is calculated as the cube root of its mass
// (square cube law, assuming all densities are the same).
#define MAX_TRAIL 1000
Body *create_bodies(int n_bodies);

// Deallocates the memory for the bodies and returns NULL.
void *destroy_bodies(Body *bodies, int n_bodies);

// Applies velocity to position and acceleration to velocity in each body.
void update_bodies(Body *bodies, int n_bodies);

// Handles the colision between two 2D bodies, with the coefficient of
// restitution as defined below.
#define COEFF_RESTITUTION 0.99f
void handle_2d_collision(Body *body1, Body *body2, float coeff_restitution);

// Changes the acceleration of each body according to Newton's gravitational
// law, with the value of G as defined below.
// If there was a collision between any two of the bodies involved, calls
// collision_handler() on them.
#define GRAVIT_CONSTANT 10.0f
void apply_gravitational_forces(Body *bodies, int n_bodies, float G,
                                void collision_handler(Body*, Body*, float));

// For each body, draws a circle of some color, if the body is on screen.
void draw_bodies(Body *bodies, int n_bodies);

// Draws arrows that represent the velocity and acceleration vectors of each body,
// in white and grey respectively.
void draw_arrows(Body *bodies, int n_bodies);

// Draws the trail of the bodies, up to a maximum of points (MAX_TRAIL).
void draw_trails(Body *bodies, int n_bodies);

#endif // NBODY_SIMULATION_H_
