#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "nbody.h"

int main(int argc, char **argv) {
    if (argc != 2)
        USAGE;

    int n_bodies = (int)strtol(argv[1], NULL, 10);
    Body *bodies = create_bodies(n_bodies);

    InitWindow(1000, 800, "nbody");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        update_bodies(bodies, n_bodies);
        apply_gravitational_forces(bodies, n_bodies);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            draw_bodies(bodies, n_bodies);
            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
