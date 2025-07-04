#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_simulation.h"
#include "nbody_ui.h"

int main(int argc, char **argv) {
    if (argc == 2 || argc > 3 || (argc == 3 && strncmp("-n", argv[1], 2) != 0))
        USAGE;
    int n_bodies = argc == 1 ? 0 : (int)strtol(argv[2], NULL, 10);

    Bodies bodies = create_bodies(n_bodies, 1200, 900);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200, 900, "nbody");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    UI ui = setup_ui();

    while (!WindowShouldClose()) {
        if (!ui.is_paused) {
            if (ui.fast_fwd_factor == 0) { // rewind
                rewind_simulation(&bodies);
            }
            for (int i = 0; i < ui.fast_fwd_factor; i++) {
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
                if (ui.body_input.is_on) {
                    draw_input_box(ui.body_input, ui);
                }
            }
            EndMode2D();
            draw_ui(ui);
        }
        EndDrawing();
    }

    unload_ui(&ui);
    CloseWindow();
    destroy_bodies(&bodies);

    return 0;
}
