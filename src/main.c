#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "physics.h"

int main() {
    int screen_width = 800;
    int screen_height = 600;

    InitWindow(screen_width, screen_height, "nbody");
    /* SetWindowState(FLAG_VSYNC_HINT); */
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawCircleV(ball_position, ball_radius, MAROON);
            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
