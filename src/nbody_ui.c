#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "nbody_simulation.h"
#include "nbody_ui.h"

void resize_image_to_rectangle(Image *image, Vector2 rectangle_size, float scale) {
    rectangle_size = Vector2Scale(rectangle_size, scale);
    int new_width, new_height;
    float image_ratio = (float)image->width / image->height;
    float rectangle_ratio = rectangle_size.x / rectangle_size.y;
    if (image_ratio > rectangle_ratio) {
        new_width = (int)rectangle_size.x;
        new_height = (int)rectangle_size.x * image->height / image->width;
    } else {
        new_width = (int)rectangle_size.y * image->width / image->height;
        new_height = (int)rectangle_size.y;
    }
    ImageResize(image, new_width, new_height);
}

UI setup_ui(void) {
    UI ui = {
        .arrows_on = false,
        .trails_on = false,
        .body_colors = {
            RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE, PINK, GOLD, LIME, DARKBLUE
        },
        .toggle_arrows = (Button){
            .box = (Rectangle){
                .x = HUD_BUTTON_MARGIN,
                .y = (float)GetScreenHeight()/2.0f
                     - HUD_BUTTON_MARGIN/2.0f - HUD_BUTTON_SIZE,
                .width = HUD_BUTTON_SIZE,
                .height = HUD_BUTTON_SIZE,
            },
            .roundness = 0.3f,
            .is_pressed = false,
            .is_hovered = false,
            .color = COLOR_HUD_BUTTON_INITIAL,
            .has_border = true,
            .border_color = COLOR_HUD_BUTTON_BORDER,
            .border_thickness = 2.0f,
        },
        .toggle_trails = (Button){
            .box = (Rectangle){
                .x = HUD_BUTTON_MARGIN,
                .y = (float)GetScreenHeight()/2.0f + HUD_BUTTON_MARGIN/2.0f,
                .width = HUD_BUTTON_SIZE,
                .height = HUD_BUTTON_SIZE,
            },
            .roundness = 0.3f,
            .is_pressed = false,
            .is_hovered = false,
            .color = COLOR_HUD_BUTTON_INITIAL,
            .has_border = true,
            .border_color = COLOR_HUD_BUTTON_BORDER,
            .border_thickness = 2.0f,
        },
        .icons = {{0}} // initialized after we load and resize the images
    };

    Image icons_img[4] = {
        LoadImage("./img/icon_arrows_off.png"),
        LoadImage("./img/icon_arrows_on.png"),
        LoadImage("./img/icon_trails_off.png"),
        LoadImage("./img/icon_trails_on.png"),
    };
    Vector2 toggle_arrows_size = (Vector2){ ui.toggle_arrows.box.width,
                                            ui.toggle_arrows.box.height };
    Vector2 toggle_trails_size = (Vector2){ ui.toggle_trails.box.width,
                                            ui.toggle_trails.box.height };
    resize_image_to_rectangle(&(icons_img[0]), toggle_arrows_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[1]), toggle_arrows_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[2]), toggle_trails_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[3]), toggle_trails_size, 0.8f);
    for (int i = 0; i < 4; i++) {
        ui.icons[i] = LoadTextureFromImage(icons_img[i]);
        UnloadImage(icons_img[i]);
    }
    ui.toggle_arrows.icon_index = 0;
    ui.toggle_trails.icon_index = 2;

    return ui;
}

void unload_ui(UI *ui) {
    for (int i = 0; i < 4; i++) {
        UnloadTexture(ui->icons[i]);
    }
}

void draw_bodies(Body *bodies, int n_bodies, UI ui) {
    for (int i = 0; i < n_bodies; i++) {
        DrawCircleV(bodies[i].position, bodies[i].radius, ui.body_colors[i % 10]);
    }
}

void draw_arrows(Body *bodies, int n_bodies) {
    for (int i = 0; i < n_bodies; i++) {
        Vector2 horizontal = (Vector2){1.0f, 0.0f};

        // Velocity arrow
        float cos_v_rotation = Vector2DotProduct(horizontal, bodies[i].velocity)
                               / Vector2Length(bodies[i].velocity);
        float v_rotation = -acosf(cos_v_rotation) * RAD2DEG;
        if (bodies[i].velocity.y > 0) {
            v_rotation = 360.0f - v_rotation;
        }
        Vector2 v_arrow_end = Vector2Add(bodies[i].position, bodies[i].velocity);
        DrawLineEx(bodies[i].position, v_arrow_end, 2.0f, COLOR_VELOCITY_ARROW);
        DrawPoly(v_arrow_end, 3, 4.5f, v_rotation, COLOR_VELOCITY_ARROW);

        // Acceleration arrow
        float cos_a_rotation = Vector2DotProduct(horizontal, bodies[i].acceleration)
                               / Vector2Length(bodies[i].acceleration);
        float a_rotation = -acosf(cos_a_rotation) * RAD2DEG;
        if (bodies[i].acceleration.y > 0) {
            a_rotation = 360.0f - a_rotation;
        }
        Vector2 a_arrow_end = Vector2Add(bodies[i].position, bodies[i].acceleration);
        DrawLineEx(bodies[i].position, a_arrow_end, 2.0f, COLOR_ACCELERATION_ARROW);
        DrawPoly(a_arrow_end, 3, 4.5f, a_rotation, COLOR_ACCELERATION_ARROW);
    }
}

#define mod(a, b) (((a) % (b) + (b)) % (b))

void draw_trails(Body *bodies, int n_bodies, UI ui) {
    for (int i = 0; i < n_bodies; i++) {
        int trail_it = bodies[i].trail.iterator;
        int count = bodies[i].trail.count;

        for (int j = 1; j < count; j++) {
            DrawLineV(bodies[i].trail.points[mod(trail_it + j, count)],
                      bodies[i].trail.points[mod(trail_it + 1 + j, count)],
                      ui.body_colors[i % 10]);
        }
    }
}

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

void update_ui(UI *ui, Camera2D *camera) {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        translate_camera_on_m2(camera);
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
        zoom_camera_on_mouse_wheel(camera, wheel);

    ui->toggle_arrows.box.x = HUD_BUTTON_MARGIN;
    ui->toggle_arrows.box.y = (float)GetScreenHeight()/2.0f
                              - HUD_BUTTON_MARGIN/2.0f - HUD_BUTTON_SIZE;
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, ui->toggle_arrows.box)) { // hovering
        ui->toggle_arrows.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // clicked
            ui->arrows_on = !ui->arrows_on;
            ui->toggle_arrows.icon_index = (ui->toggle_arrows.icon_index + 1) % 2;
        }
    } else { // not hovering
        ui->toggle_arrows.color = ui->arrows_on ? COLOR_HUD_BUTTON_PRESSED
                                                : COLOR_HUD_BUTTON_INITIAL;
    }

    ui->toggle_trails.box.x = HUD_BUTTON_MARGIN;
    ui->toggle_trails.box.y = (float)GetScreenHeight()/2.0f + HUD_BUTTON_MARGIN/2.0f;
    if (CheckCollisionPointRec(mouse, ui->toggle_trails.box)) { // hovering
        ui->toggle_trails.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // clicked
            ui->trails_on = !ui->trails_on;
            ui->toggle_trails.icon_index = (ui->toggle_trails.icon_index + 1) % 2 + 2;
        }
    } else { // not hovering
        ui->toggle_trails.color = ui->trails_on ? COLOR_HUD_BUTTON_PRESSED
                                                : COLOR_HUD_BUTTON_INITIAL;
    }
}

void draw_button(Button b, UI ui) {
    const int segments = 20;

    DrawRectangleRounded(b.box, b.roundness, segments, b.color);
    if (b.has_border) {
        DrawRectangleRoundedLines(b.box, b.roundness, segments,
                                  b.border_thickness, b.border_color);
    }
   
    // TODO: find a way to perfectly scale the image given the button roundness
    Vector2 texture_pos = (Vector2){
        .x = b.box.x + (b.box.width - ui.icons[b.icon_index].width) / 2.0f,
        .y = b.box.y + (b.box.height - ui.icons[b.icon_index].height) / 2.0f,
    };
    DrawTextureV(ui.icons[b.icon_index], texture_pos, WHITE);
}

void draw_ui(UI ui) {
    draw_button(ui.toggle_arrows, ui);
    draw_button(ui.toggle_trails, ui);
}
