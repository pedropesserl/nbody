#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "physics.h"

int main(int argc, char **argv) {
    int screen_width = 1000;
    int screen_height = 800;

    InitWindow(screen_width, screen_height, "nbody");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
