#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_simulation.h"
#include "nbody_ui.h"

int main(int argc, char **argv) {
    if (argc != 2)
        USAGE;
    int n_bodies = (int)strtol(argv[1], NULL, 10);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 900, "nbody");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    UI ui = setup_ui();
    Bodies bodies = create_bodies(n_bodies);

    while (!WindowShouldClose()) {
        if (!ui.is_paused) {
            for (int i = 0; i < ui.fast_forward_factor; i++) {
                update_bodies(&bodies);
                apply_gravitational_forces(&bodies, GRAVIT_CONSTANT, handle_2d_collision);
            }
        }

        update_ui(&ui, &camera, &bodies);
        
        BeginDrawing();
        {
            ClearBackground(COLOR_BACKGROUND);
            BeginMode2D(camera);
            {
                draw_bodies(bodies, ui);
                if (ui.trails_on) {
                    draw_trails(bodies, ui);
                }
                if (ui.arrows_on) {
                    draw_arrows(bodies);
                }
                if (ui.body_input.active) {
                    draw_input_box(ui.body_input, ui);
                }
            }
            EndMode2D();
            DrawFPS(10, 10);

            draw_ui(ui);
        }
        EndDrawing();
    }

    unload_ui(&ui);
    CloseWindow();
    destroy_bodies(&bodies);

    return 0;
}
