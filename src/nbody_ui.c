#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_ui.h"

void translate_camera_on_m2_down(Camera2D *camera) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f/camera->zoom);
    camera->target = Vector2Add(camera->target, delta);
}
