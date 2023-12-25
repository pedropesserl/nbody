#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_ui.h"

void translate_camera_on_m2(Camera2D *camera) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f/camera->zoom);
    camera->target = Vector2Add(camera->target, delta);
}

void zoom_camera_on_mouse_wheel(Camera2D *camera, float wheel) {
    Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouse_world_pos;
    const float zoom_increment = 0.125f;
    camera->zoom += wheel * zoom_increment;
    if (camera->zoom < zoom_increment)
        camera->zoom = zoom_increment;
}
