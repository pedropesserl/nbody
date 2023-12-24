#ifndef NBODY_H_
#define NBODY_H_

#define USAGE do {                                                  \
        fprintf(stderr, "Usage: %s <number_of_bodies>\n", argv[0]); \
        exit(1);                                                    \
    } while (0)

#define MEM_ERR do {                                                                    \
        fprintf(stderr, "Mem alloc error: %s:%d (%s)\n", __FILE__, __LINE__, __func__); \
        exit(1);                                                                        \
    } while (0)

#define ADDVEC2(a, b) (Vector2){(a).x + (b).x, (a).y + (b).y}
#define SUBVEC2(a, b) (Vector2){(a).x - (b).x, (a).y - (b).y}

typedef struct Body {
    float mass;
    float radius;
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
} Body;

extern Color body_colors[10];

// Returns an array of bodies of size n_bodies with data passed from stdin.
// The radius of each body is calculated as the cube root of its mass
// (square cube law, assuming all densities are the same).
Body *create_bodies(int n_bodies);

// Applies velocity to position and acceleration to velocity in each body.
void update_bodies(Body *bodies, int n_bodies);

#define G 4.0f

// Changes the acceleration of each body according to Newton's gravitational
// law (with G defined above)
void apply_gravitational_forces(Body *bodies, int n_bodies);

// For each body, draws a circle of some color.
void draw_bodies(Body *bodies, int n_bodies);

#endif // NBODY_H_
