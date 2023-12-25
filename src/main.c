#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "nbody_simulation.h"
#include "nbody_ui.h"

int main(int argc, char **argv) {
    if (argc != 2)
        USAGE;
    int n_bodies = (int)strtol(argv[1], NULL, 10);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 1000, "nbody");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    Camera2D camera = {
        .offset = (Vector2){0},
        .target = (Vector2){0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    Body *bodies = create_bodies(n_bodies);

    while (!WindowShouldClose()) {
        update_bodies(bodies, n_bodies);
        apply_gravitational_forces(bodies, n_bodies, GRAVIT_CONSTANT, handle_2d_collision);

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            translate_camera_on_m2_down(&camera);
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode2D(camera);
            {
                draw_bodies(bodies, n_bodies);
                draw_trails(bodies, n_bodies);
                draw_arrows(bodies, n_bodies);
            }
            EndMode2D();
            DrawFPS(10, 10);
            /* draw_UI(); */
        }
        EndDrawing();
    }

    CloseWindow();
    bodies = destroy_bodies(bodies, n_bodies);

    return 0;
}
