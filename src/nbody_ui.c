#include <stdio.h>
#include <stdlib.h>
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

#define LOAD_ICONS { \
            LoadImage("./img/icon_pause.png"),      \
            LoadImage("./img/icon_play.png"),       \
            LoadImage("./img/icon_arrows_off.png"), \
            LoadImage("./img/icon_arrows_on.png"),  \
            LoadImage("./img/icon_trails_off.png"), \
            LoadImage("./img/icon_trails_on.png"),  \
        }

UI setup_ui(void) {
    UI ui = {
        .is_paused = false,
        .arrows_on = false,
        .trails_on = false,
        .fast_forward_factor = 1,
        .body_colors = {
            RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE, PINK, GOLD, LIME, DARKBLUE
        },
        .play_pause = (Button){
            .box = (Rectangle){
                .x = (float)GetScreenWidth()/2.0f - HUD_SECONDARY_BUTTON_SIZE,
                .y = HUD_BUTTON_MARGIN,
                .width = HUD_SECONDARY_BUTTON_SIZE,
                .height = HUD_SECONDARY_BUTTON_SIZE,
            },
            .roundness = 0.3f,
            .is_pressed = false,
            .is_hovered = false,
            .color = COLOR_HUD_BUTTON_INITIAL,
            .has_border = false,
            .border_color = COLOR_HUD_BUTTON_BORDER,
            .border_thickness = 2.0f,
        },
        .toggle_arrows = (Button){
            .box = (Rectangle){
                .x = HUD_BUTTON_MARGIN,
                .y = (float)GetScreenHeight()/2.0f
                     - HUD_BUTTON_MARGIN/2.0f - HUD_PRIMARY_BUTTON_SIZE,
                .width = HUD_PRIMARY_BUTTON_SIZE,
                .height = HUD_PRIMARY_BUTTON_SIZE,
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
                .width = HUD_PRIMARY_BUTTON_SIZE,
                .height = HUD_PRIMARY_BUTTON_SIZE,
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

    Image icons_img[ICON_COUNT] = LOAD_ICONS;
    Vector2 play_pause_size = (Vector2) { ui.play_pause.box.width,
                                          ui.play_pause.box.height };
    Vector2 toggle_arrows_size = (Vector2){ ui.toggle_arrows.box.width,
                                            ui.toggle_arrows.box.height };
    Vector2 toggle_trails_size = (Vector2){ ui.toggle_trails.box.width,
                                            ui.toggle_trails.box.height };
    resize_image_to_rectangle(&(icons_img[0]), play_pause_size, 0.6f);
    resize_image_to_rectangle(&(icons_img[1]), play_pause_size, 0.6f);
    resize_image_to_rectangle(&(icons_img[2]), toggle_arrows_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[3]), toggle_arrows_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[4]), toggle_trails_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[5]), toggle_trails_size, 0.8f);
    for (int i = 0; i < ICON_COUNT; i++) {
        ui.icons[i] = LoadTextureFromImage(icons_img[i]);
        UnloadImage(icons_img[i]);
    }
    ui.play_pause.icon_index = 0;
    ui.toggle_arrows.icon_index = 2;
    ui.toggle_trails.icon_index = 4;

    return ui;
}

void unload_ui(UI *ui) {
    for (int i = 0; i < ICON_COUNT; i++) {
        UnloadTexture(ui->icons[i]);
    }
}

void draw_bodies(Bodies bodies, UI ui) {
    for (size_t i = 0; i < bodies.count; i++) {
        DrawCircleV(bodies.data[i].position, bodies.data[i].radius,
                    ui.body_colors[i % 10]);
    }
}

static void draw_vector_arrow(Vector2 vector, Vector2 position, Color color) {
        Vector2 horizontal = (Vector2){1.0f, 0.0f};

        float cos_rotation = Vector2DotProduct(horizontal, vector) / Vector2Length(vector);
        float rotation = -acosf(cos_rotation) * RAD2DEG;
        if (vector.y > 0) {
            rotation = 360.0f - rotation;
        }
        Vector2 arrow_end = Vector2Add(position, vector);
        DrawLineEx(position, arrow_end, 2.0f, color);
        DrawPoly(arrow_end, 3, 4.5f, rotation, color);
}

void draw_arrows(Bodies bodies) {
    for (size_t i = 0; i < bodies.count; i++) {
        draw_vector_arrow(bodies.data[i].velocity, bodies.data[i].position,
                          COLOR_VELOCITY_ARROW);
        draw_vector_arrow(bodies.data[i].acceleration, bodies.data[i].position,
                          COLOR_ACCELERATION_ARROW);
    }
}

#define mod(a, b) (((a) % (b) + (b)) % (b))

void draw_trails(Bodies bodies, UI ui) {
    for (size_t i = 0; i < bodies.count; i++) {
        int trail_it = bodies.data[i].trail.iterator;
        int count = bodies.data[i].trail.count;

        for (int j = 1; j < count; j++) {
            DrawLineV(bodies.data[i].trail.points[mod(trail_it + j, count)],
                      bodies.data[i].trail.points[mod(trail_it + 1 + j, count)],
                      ui.body_colors[i % 10]);
        }
    }
}

static void translate_camera_on_m2(Camera2D *camera) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f/camera->zoom);
    camera->target = Vector2Add(camera->target, delta);
}

static void zoom_camera_on_mouse_wheel(Camera2D *camera, float wheel) {
    Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouse_world_pos;
    const float zoom_increment = 0.125f;
    camera->zoom += wheel * zoom_increment;
    if (camera->zoom < zoom_increment)
        camera->zoom = zoom_increment;
}

static Body create_body_with_popup(Vector2 position) {
    Body new_body = {
        .mass = 100.0f,
        .radius = 0.0f, // radius is calculated based on mass
        .position = position,
        .velocity = (Vector2){0},
        .acceleration = (Vector2){0},
        .trail.points = (Vector2*)malloc(MAX_TRAIL * sizeof(Vector2)),
        .trail.count = 1,
        .trail.iterator = 0,
    };
    if (!new_body.trail.points)
        MEM_ERR;
    new_body.trail.points[0] = new_body.position;
    new_body.radius = cbrtf(new_body.mass);
    return new_body;
}

void update_ui(UI *ui, Camera2D *camera, Bodies *bodies) {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        translate_camera_on_m2(camera);
    }
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        zoom_camera_on_mouse_wheel(camera, wheel);
    }

    Vector2 mouse = GetMousePosition();

    // Update play/pause button
    ui->play_pause.box.x = (float)GetScreenWidth()/2.0f - HUD_SECONDARY_BUTTON_SIZE/2.0f;
    ui->play_pause.box.y = HUD_BUTTON_MARGIN;
    if (CheckCollisionPointRec(mouse, ui->play_pause.box)) { // hovering
        ui->play_pause.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // clicked
            ui->is_paused = !ui->is_paused;
            ui->play_pause.icon_index = (ui->play_pause.icon_index + 1) % 2;
        }
        return;
    }
    ui->play_pause.color = ui->is_paused ? COLOR_HUD_BUTTON_PRESSED
                                         : COLOR_HUD_BUTTON_INITIAL;

    // Update toggle arrows button
    ui->toggle_arrows.box.x = HUD_BUTTON_MARGIN;
    ui->toggle_arrows.box.y = (float)GetScreenHeight()/2.0f
                              - HUD_BUTTON_MARGIN/2.0f - HUD_PRIMARY_BUTTON_SIZE;
    if (CheckCollisionPointRec(mouse, ui->toggle_arrows.box)) { // hovering
        ui->toggle_arrows.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // clicked
            ui->arrows_on = !ui->arrows_on;
            ui->toggle_arrows.icon_index = (ui->toggle_arrows.icon_index + 1) % 2 + 2;
        }
        return;
    }
    ui->toggle_arrows.color = ui->arrows_on ? COLOR_HUD_BUTTON_PRESSED
                                            : COLOR_HUD_BUTTON_INITIAL;

    // Update toggle trails button
    ui->toggle_trails.box.x = HUD_BUTTON_MARGIN;
    ui->toggle_trails.box.y = (float)GetScreenHeight()/2.0f + HUD_BUTTON_MARGIN/2.0f;
    if (CheckCollisionPointRec(mouse, ui->toggle_trails.box)) { // hovering
        ui->toggle_trails.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // clicked
            ui->trails_on = !ui->trails_on;
            ui->toggle_trails.icon_index = (ui->toggle_trails.icon_index + 1) % 2 + 4;
        }
        return;
    }
    ui->toggle_trails.color = ui->trails_on ? COLOR_HUD_BUTTON_PRESSED
                                            : COLOR_HUD_BUTTON_INITIAL;

    // Mouse is not on any button
    Vector2 mouse_in_world = GetScreenToWorld2D(mouse, *camera);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { // clicked
        bool body_will_collide = false;
        Body new_body = create_body_with_popup(mouse_in_world);
        for (size_t i = 0; i < bodies->count; i++) {
            if (CheckCollisionCircles(mouse_in_world, new_body.radius,
                                      bodies->data[i].position, bodies->data[i].radius)) {
                body_will_collide = true;
                break;
            }
        }
        if (!body_will_collide) {
            insert_body(bodies, new_body);
        }
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
    draw_button(ui.play_pause, ui);
    draw_button(ui.toggle_arrows, ui);
    draw_button(ui.toggle_trails, ui);
}
