#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "nbody.h"

int main(int argc, char **argv) {
    if (argc != 2)
        USAGE;

    int n_bodies = (int)strtol(argv[1], NULL, 10);
    Body *bodies = create_bodies(n_bodies);

    InitWindow(1200, 1000, "nbody");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    bool collision = false;
    while (!WindowShouldClose()) {
        if (!collision) {
            update_bodies(bodies, n_bodies);
            collision = apply_gravitational_forces(bodies, n_bodies);
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);
            draw_bodies(bodies, n_bodies);
            DrawFPS(10, 10);
            if (collision) {
                DrawText("Collision detected!",
                        GetScreenWidth()/2 - MeasureText("Collision detected!", 50)/2,
                        GetScreenHeight()/2 - 25,
                        50, MAROON);
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
