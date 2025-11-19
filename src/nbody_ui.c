#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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
            LoadImage("./resources/img/icon_pause.png"),      \
            LoadImage("./resources/img/icon_play.png"),       \
            LoadImage("./resources/img/icon_arrows_off.png"), \
            LoadImage("./resources/img/icon_arrows_on.png"),  \
            LoadImage("./resources/img/icon_trails_off.png"), \
            LoadImage("./resources/img/icon_trails_on.png"),  \
            LoadImage("./resources/img/icon_confirm.png"),    \
            LoadImage("./resources/img/icon_cancel.png"),     \
            LoadImage("./resources/img/icon_fast_fwd.png"),   \
            LoadImage("./resources/img/icon_rewind.png"),     \
        }

Button new_button(Button_ID id, Button_Type type) {
    Button b = (Button){
        .id = id,
        .type = type,
        .box = (Rectangle){
            .x = 0.0f,
            .y = 0.0f,
            .width = 0,
            .height = 0,
        },
        .icon_index = 0,
        .roundness = 0.3f,
        .is_pressed = false,
        .color = COLOR_HUD_BUTTON_INITIAL,
        .has_border = true,
        .border_color = COLOR_HUD_BUTTON_BORDER,
        .border_thickness = 2.0f,
    };
    switch (type) {
        case BUTTON_PRIMARY:
            b.box.width = HUD_PRIMARY_BUTTON_SIZE;
            b.box.height = HUD_PRIMARY_BUTTON_SIZE;
            break;
        case BUTTON_SECONDARY:
            b.box.width = HUD_SECONDARY_BUTTON_SIZE;
            b.box.height = HUD_SECONDARY_BUTTON_SIZE;
            break;
    }
    switch (id) {
        case BUTTON_PLAY_PAUSE:    b.icon_index = ICON_IDX_PAUSE;      break;
        case BUTTON_TOGGLE_ARROWS: b.icon_index = ICON_IDX_ARROWS_OFF; break;
        case BUTTON_TOGGLE_TRAILS: b.icon_index = ICON_IDX_TRAILS_OFF; break;
        case BUTTON_CONFIRM:       b.icon_index = ICON_IDX_CONFIRM;    break;
        case BUTTON_CANCEL:        b.icon_index = ICON_IDX_CANCEL;     break;
        case BUTTON_FAST_FWD:      b.icon_index = ICON_IDX_FAST_FWD;   break;
        case BUTTON_REWIND:        b.icon_index = ICON_IDX_REWIND;     break;
    }
    return b;
}

Str_Input new_string_input(const char *label, size_t lable_len, Vector2 pos, Vector2 size) {
    Str_Input si = (Str_Input){
        .label = {0},
        .input = {0},
        .input_box = (Rectangle) {
            .x = pos.x,
            .y = pos.y,
            .width = size.x,
            .height = size.y,
        },
        .roundness = 0.3f,
        .is_selected = false,
        .is_hovered = false,
        .color = COLOR_INPUT_FIELD_INITIAL,
        .input_value = 0.0f,
        .char_count = 0,
        .has_period = false,
    };
    if (lable_len > MAX_LABEL_CHARACTERS) {
        lable_len = MAX_LABEL_CHARACTERS;
    }
    memcpy(si.label, label, lable_len);
    return si;
}

Input_Box new_input_box(Vector2 position) {
    float hud_space = 1.5f * INPUT_FIELD_MARGIN + HUD_SECONDARY_BUTTON_SIZE;
    float fields_space = HUD_INPUT_BOX_HEIGHT - hud_space - INPUT_FIELD_MARGIN / 2.0f;
    Vector2 field_boxes_positions[MAX_FIELDS] = {0};
    for (int i = 0; i < MAX_FIELDS; i++) {
        field_boxes_positions[i] = (Vector2){
            .x = position.x + HUD_INPUT_BOX_WIDTH / 2.0f + INPUT_FIELD_MARGIN / 2.0f,
            .y = position.y + hud_space + fields_space / MAX_FIELDS * i + INPUT_FIELD_MARGIN/2.0f
        };
    }

   float field_box_width = HUD_INPUT_BOX_WIDTH / 2.0f - 1.5f * INPUT_FIELD_MARGIN;
   float field_box_height = fields_space / MAX_FIELDS - INPUT_FIELD_MARGIN;

   Input_Box ib = (Input_Box){
        .box = (Rectangle){
            .x = position.x,
            .y = position.y,
            .width = HUD_INPUT_BOX_WIDTH,
            .height = HUD_INPUT_BOX_HEIGHT},
        .roundness = 0.1f,
        .color = COLOR_INPUT_BOX,
        .has_border = true,
        .border_color = COLOR_HUD_BUTTON_BORDER,
        .border_thickness = 1.5f,
        .fields[0] = new_string_input("Mass:", 5,
            (Vector2){ field_boxes_positions[0].x, field_boxes_positions[0].y },
            (Vector2){ field_box_width, field_box_height }),
        .fields[1] = new_string_input("Density:", 8,
            (Vector2){ field_boxes_positions[1].x, field_boxes_positions[1].y },
            (Vector2){ field_box_width, field_box_height }),
        .fields[2] = new_string_input("Velocity (x):", 13,
            (Vector2){ field_boxes_positions[2].x, field_boxes_positions[2].y },
            (Vector2){ field_box_width, field_box_height }),
        .fields[3] = new_string_input("Velocity (y):", 13,
            (Vector2){ field_boxes_positions[3].x, field_boxes_positions[3].y },
            (Vector2){ field_box_width, field_box_height }),
        .confirm = new_button(BUTTON_CONFIRM, BUTTON_SECONDARY),
        .cancel = new_button(BUTTON_CANCEL, BUTTON_SECONDARY),
        .is_on = true,
        .is_invoked = false,
    };

    ib.confirm.box.x = position.x + HUD_INPUT_BOX_WIDTH - 2*HUD_SECONDARY_BUTTON_SIZE - 2*INPUT_FIELD_MARGIN;
    ib.confirm.box.y = position.y + INPUT_FIELD_MARGIN;
    ib.cancel.box.x = position.x + HUD_INPUT_BOX_WIDTH - HUD_SECONDARY_BUTTON_SIZE - INPUT_FIELD_MARGIN;
    ib.cancel.box.y = position.y + INPUT_FIELD_MARGIN;

    return ib;
}

// this function makes sure the input box is visible in the screen wherever it's called
static Input_Box create_input_box_with_mouse(Vector2 mouse) {
    if (mouse.x < GetScreenWidth()/2) {
        if (mouse.y < GetScreenHeight()/2) {
            return new_input_box(mouse);
        }
        return new_input_box((Vector2){mouse.x,
                                       mouse.y - HUD_INPUT_BOX_HEIGHT});
    }
    if (mouse.y < GetScreenHeight()/2) {
        return new_input_box((Vector2){mouse.x - HUD_INPUT_BOX_WIDTH,
                                       mouse.y});
    }
    return new_input_box((Vector2){mouse.x - HUD_INPUT_BOX_WIDTH,
                                   mouse.y - HUD_INPUT_BOX_HEIGHT});
}

UI setup_ui(void) {
    UI ui = {
        .is_paused = false,
        .arrows_on = false,
        .trails_on = false,
        .fast_fwd_factor = 1,
        .rewind_on = false,
        .body_colors = {
            RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE, PINK, GOLD, LIME, DARKBLUE
        },
        .play_pause = new_button(BUTTON_PLAY_PAUSE, BUTTON_SECONDARY),
        .toggle_arrows = new_button(BUTTON_TOGGLE_ARROWS, BUTTON_PRIMARY),
        .toggle_trails = new_button(BUTTON_TOGGLE_TRAILS, BUTTON_PRIMARY),
        .fast_fwd = new_button(BUTTON_FAST_FWD, BUTTON_SECONDARY),
        .rewind = new_button(BUTTON_REWIND, BUTTON_SECONDARY),
        .body_input = new_input_box(Vector2Zero()),
        .position_to_create_body = Vector2Zero(),
        .created_body_with_input = false,
        .icons = {{0}}, // initialized after we load and resize the images
        .font = LoadFontEx("./resources/fonts/EurostileBold.ttf", 28, NULL, 0),
    };
    ui.body_input.is_on = false;

    Image icons_img[ICON_COUNT] = LOAD_ICONS;
    Vector2 play_pause_size = (Vector2) { HUD_SECONDARY_BUTTON_SIZE,
                                          HUD_SECONDARY_BUTTON_SIZE };
    Vector2 confirm_size = play_pause_size;
    Vector2 cancel_size = play_pause_size;
    Vector2 fast_fwd_size = play_pause_size;
    Vector2 rewind_size = play_pause_size;
    Vector2 toggle_arrows_size = (Vector2){ HUD_PRIMARY_BUTTON_SIZE,
                                            HUD_PRIMARY_BUTTON_SIZE };
    Vector2 toggle_trails_size = toggle_arrows_size;
    resize_image_to_rectangle(&(icons_img[ICON_IDX_PAUSE]),      play_pause_size, 0.6f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_PLAY]),       play_pause_size, 0.6f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_ARROWS_OFF]), toggle_arrows_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_ARROWS_ON]),  toggle_arrows_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_TRAILS_OFF]), toggle_trails_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_TRAILS_ON]),  toggle_trails_size, 0.8f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_CONFIRM]),    confirm_size, 0.75f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_CANCEL]),     cancel_size, 0.6f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_FAST_FWD]),   fast_fwd_size, 0.75f);
    resize_image_to_rectangle(&(icons_img[ICON_IDX_REWIND]),     rewind_size, 0.75f);
    for (int i = 0; i < ICON_COUNT; i++) {
        ui.icons[i] = LoadTextureFromImage(icons_img[i]);
        UnloadImage(icons_img[i]);
    }

    return ui;
}

void unload_ui(UI *ui) {
    for (int i = 0; i < ICON_COUNT; i++) {
        UnloadTexture(ui->icons[i]);
    }
    UnloadFont(ui->font);
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
        Vector2 arrow_head = Vector2Add(position, vector);
        DrawLineEx(position, arrow_head, 2.0f, color);
        DrawPoly(arrow_head, 3, 4.5f, rotation, color);
}

void draw_arrows(Bodies bodies) {
    for (size_t i = 0; i < bodies.count; i++) {
        draw_vector_arrow(bodies.data[i].velocity, bodies.data[i].position,
                          COLOR_VELOCITY_ARROW);
        draw_vector_arrow(bodies.data[i].acceleration, bodies.data[i].position,
                          COLOR_ACCELERATION_ARROW);
    }
}

void draw_trails(Bodies bodies, UI ui) {
    for (size_t i = 0; i < bodies.count; i++) {
        int begin = bodies.data[i].trail.begin, end = bodies.data[i].trail.end;
        for (int j = begin; j != end; j = (j + 1) % MAX_TRAIL) {
            DrawLineV(bodies.data[i].trail.positions[j],
                      bodies.data[i].trail.positions[(j + 1) % MAX_TRAIL],
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
    float scale = 0.2f * wheel;
    camera->zoom = fmax(expf(logf(camera->zoom) + scale), 0.125f);
}

static void update_input_box(Input_Box *ib, Vector2 mouse_in_world, UI *ui) {
    if (!ib->is_on) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ib->is_invoked = true;
        }
        if (ib->is_invoked && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            *ib = create_input_box_with_mouse(mouse_in_world);
            ui->position_to_create_body = mouse_in_world;
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        ib->is_on = false;
        return;
    }
    if (CheckCollisionPointRec(mouse_in_world, ib->cancel.box)) { // hovering
        ib->cancel.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { // clicked
            ib->is_on = false;
        }
        return;
    }
    ib->cancel.color = COLOR_HUD_BUTTON_INITIAL;

    if (IsKeyPressed(KEY_ENTER)) {
        ui->created_body_with_input = false;
        ib->is_on = false;
        for (int i = 0; i < MAX_FIELDS; i++) {
            ib->fields[i].input_value = strtof(ib->fields[i].input, NULL);
        }
        return;
    }
    if (CheckCollisionPointRec(mouse_in_world, ib->confirm.box)) { // hovering
        ib->confirm.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { // clicked
            ui->created_body_with_input = false;
            ib->is_on = false;
            // parse input
            for (int i = 0; i < MAX_FIELDS; i++) {
                ib->fields[i].input_value = strtof(ib->fields[i].input, NULL);
            }
        }
        return;
    }
    ib->confirm.color = COLOR_HUD_BUTTON_INITIAL;

    if (IsKeyPressed(KEY_TAB)) {
        int selected = -1;
        for (int i = 0; i < MAX_FIELDS; i++) {
            if (ib->fields[i].is_selected) {
                selected = i;
                break;
            }
        }
        if (selected == -1) { // no field was selected
            ib->fields[0].is_selected = true;
            return;
        }
        ib->fields[selected].is_selected = false;
        ib->fields[(selected + 1) % MAX_FIELDS].is_selected = true;
    }
    for (int i = 0; i < MAX_FIELDS; i++) {
        Str_Input *field = &(ib->fields[i]);
        if (CheckCollisionPointRec(mouse_in_world, field->input_box)) { // hovering
            field->color = COLOR_INPUT_FIELD_HOVERED;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { // clicked
                field->is_selected = true;
                for (int j = 0; j < i; j++) {
                    ib->fields[j].is_selected = false;
                }
                for (int j = i+1; j < MAX_FIELDS; j++) {
                    ib->fields[j].is_selected = false;
                }
            }
        } else if (!field->is_selected) { // not hovering and not selected
            field->color = COLOR_INPUT_FIELD_INITIAL;
        }

        if (field->is_selected) { // entering text
            char c = (char)GetCharPressed();
            // Check if more characters have been pressed on the same frame
            while (c > 0) {
                if (field->char_count >= MAX_INPUT_CHARACTERS)
                    break;
                if ((c == '-' && field->char_count == 0)
                    || (!field->has_period && (isdigit(c) || c == '.'))
                    || (field->has_period && isdigit(c))) {

                    field->input[field->char_count] = c;
                    field->input[field->char_count + 1] = '\0';
                    field->char_count += 1;
                }
                field->has_period |= c == '.';
                c = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                field->char_count -= field->char_count > 0;
                if (field->input[field->char_count] == '.') {
                    field->has_period = false;
                }
                field->input[field->char_count] = '\0';
            }
        }
    }
}

static Body *create_body_with_input_box(Input_Box ib, Vector2 position) {
    float mass = ib.fields[0].input_value;
    float density = ib.fields[1].input_value;
    if (mass <= 0.0f || density <= 0.0f) { // mass and density must be greater than 0
        return NULL;
    }
    float velocity_x = ib.fields[2].input_value;
    float velocity_y = ib.fields[3].input_value;
    Body *new_body = (Body*)malloc(sizeof(Body));
    *new_body = (Body){
        .mass = mass,
        .density = density,
        .radius = 0.0f, // radius is calculated based on mass and density
        .position = position,
        .velocity = (Vector2){velocity_x, velocity_y},
        .acceleration = Vector2Zero(),
        .trail = {0},
    };
    new_body->trail = new_trail();
    new_body->trail.positions[0] = new_body->position;
    new_body->trail.velocities[0] = new_body->velocity;
    new_body->trail.accelerations[0] = new_body->acceleration;
    new_body->trail.begin = 0;
    new_body->trail.end = 0;
    new_body->radius = cbrtf(new_body->mass / new_body->density);
    return new_body;
}

bool update_toggle_button(Button *button, Button_ID id, Vector2 mouse, UI *ui) {
    bool mouse_is_on_button = false;
    bool *toggle_value;
    Icon_Index icon_active;
    Icon_Index icon_inactive;
    switch (id) {
        case BUTTON_PLAY_PAUSE:
            toggle_value = &(ui->is_paused);
            icon_active = ICON_IDX_PLAY;
            icon_inactive = ICON_IDX_PAUSE;
            break;
        case BUTTON_TOGGLE_ARROWS:
            toggle_value = &(ui->arrows_on);
            icon_active = ICON_IDX_ARROWS_ON;
            icon_inactive = ICON_IDX_ARROWS_OFF;
            break;
        case BUTTON_TOGGLE_TRAILS:
            toggle_value = &(ui->trails_on);
            icon_active = ICON_IDX_TRAILS_ON;
            icon_inactive = ICON_IDX_TRAILS_OFF;
            break;
        default:
            return false;
    }
    if (CheckCollisionPointRec(mouse, button->box)) { // hovering
        mouse_is_on_button = true;
        button->color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            button->is_pressed = true;
        }
        if (button->is_pressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            *toggle_value = !(*toggle_value);
        }
    } else {
        button->color = *toggle_value ? COLOR_HUD_BUTTON_PRESSED : COLOR_HUD_BUTTON_INITIAL;
        button->is_pressed = false;
    }
    button->icon_index = *toggle_value ? icon_active : icon_inactive;
    return mouse_is_on_button;
}

void update_ui(UI *ui, Camera2D *camera, Bodies *bodies) {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        translate_camera_on_m2(camera);
    }
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        zoom_camera_on_mouse_wheel(camera, wheel);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        ui->is_paused = !ui->is_paused;
    }
    if (IsKeyPressed(KEY_A)) {
        ui->arrows_on = !ui->arrows_on;
    }
    if (IsKeyPressed(KEY_T)) {
        ui->trails_on = !ui->trails_on;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        ui->fast_fwd_factor += ui->fast_fwd_factor < 5;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        ui->fast_fwd_factor -= ui->fast_fwd_factor > 0;
    }

    Vector2 mouse = GetMousePosition();
    bool mouse_is_on_button = false;

    // Update play/pause button
    ui->play_pause.box.x = (float)GetScreenWidth()/2.0f - HUD_SECONDARY_BUTTON_SIZE/2.0f;
    ui->play_pause.box.y = HUD_BUTTON_MARGIN;
    mouse_is_on_button |= update_toggle_button(&(ui->play_pause), BUTTON_PLAY_PAUSE, mouse, ui);

    // Update toggle arrows button
    ui->toggle_arrows.box.x = HUD_BUTTON_MARGIN;
    ui->toggle_arrows.box.y = (float)GetScreenHeight()/2.0f
                              - HUD_BUTTON_MARGIN/2.0f - HUD_PRIMARY_BUTTON_SIZE;
    mouse_is_on_button |= update_toggle_button(&(ui->toggle_arrows), BUTTON_TOGGLE_ARROWS, mouse, ui);

    // Update toggle trails button
    ui->toggle_trails.box.x = HUD_BUTTON_MARGIN;
    ui->toggle_trails.box.y = (float)GetScreenHeight()/2.0f + HUD_BUTTON_MARGIN/2.0f;
    mouse_is_on_button |= update_toggle_button(&(ui->toggle_trails), BUTTON_TOGGLE_TRAILS, mouse, ui);

    // Update fast forward button
    ui->fast_fwd.box.x = (float)GetScreenWidth()/2.0f + HUD_SECONDARY_BUTTON_SIZE/2.0f + HUD_BUTTON_MARGIN;
    ui->fast_fwd.box.y = HUD_BUTTON_MARGIN;
    if (CheckCollisionPointRec(mouse, ui->fast_fwd.box)) { // hovering
        mouse_is_on_button = true;
        ui->fast_fwd.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ui->fast_fwd.is_pressed = true;
        }
        if (ui->fast_fwd.is_pressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ui->fast_fwd_factor += ui->fast_fwd_factor < 5;
        }
    } else {
        ui->fast_fwd.color = COLOR_HUD_BUTTON_INITIAL;
        ui->fast_fwd.is_pressed = false;
    }

    // Update rewind button
    ui->rewind.box.x = (float)GetScreenWidth()/2.0f - 1.5*HUD_SECONDARY_BUTTON_SIZE - HUD_BUTTON_MARGIN;
    ui->rewind.box.y = HUD_BUTTON_MARGIN;
    if (CheckCollisionPointRec(mouse, ui->rewind.box)) { // hovering
        mouse_is_on_button = true;
        ui->rewind.color = COLOR_HUD_BUTTON_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ui->rewind.is_pressed = true;
        }
        if (ui->rewind.is_pressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ui->fast_fwd_factor -= ui->fast_fwd_factor > 0;
        }
    } else {
        ui->rewind.color = COLOR_HUD_BUTTON_INITIAL;
        ui->rewind.is_pressed = false;
    }

    if (mouse_is_on_button) {
        ui->body_input.is_invoked = false;
        return;
    }
    // Mouse is not on any button
    update_input_box(&(ui->body_input), GetScreenToWorld2D(mouse, *camera), ui);

    if (ui->created_body_with_input) {
        return;
    }

    bool body_will_collide = false;
    Body *new_body = create_body_with_input_box(ui->body_input, ui->position_to_create_body);
    ui->created_body_with_input = true;
    if (new_body == NULL) { // bad input of fields in input box
        return;
    }
    for (size_t i = 0; i < bodies->count; i++) {
        if (CheckCollisionCircles(ui->position_to_create_body, new_body->radius,
                                  bodies->data[i].position, bodies->data[i].radius)) {
            body_will_collide = true;
            break;
        }
    }
    if (!body_will_collide) {
        insert_body(bodies, *new_body);
    }
}

void draw_button(Button b, UI ui) {
    const int segments = 20;

    DrawRectangleRounded(b.box, b.roundness, segments, b.color);
    if (b.has_border) {
        DrawRectangleRoundedLinesEx(b.box, b.roundness, segments,
                                    b.border_thickness, b.border_color);
    }
   
    // TODO: find a way to perfectly scale the image given the button roundness
    Vector2 texture_pos = (Vector2){
        .x = b.box.x + (b.box.width - ui.icons[b.icon_index].width) / 2.0f,
        .y = b.box.y + (b.box.height - ui.icons[b.icon_index].height) / 2.0f,
    };
    DrawTextureV(ui.icons[b.icon_index], texture_pos, WHITE);
}

void draw_input_box(Input_Box ib, UI ui) {
    const int segments = 20;
    
    DrawRectangleRounded(ib.box, ib.roundness, segments, ib.color);
    if (ib.has_border) {
        DrawRectangleRoundedLinesEx(ib.box, ib.roundness, segments,
                                    ib.border_thickness, ib.border_color);
    }

    draw_button(ib.confirm, ui);
    draw_button(ib.cancel, ui);

    float font_size = ib.fields[0].input_box.height * 0.8;
    
    for (int i = 0; i < MAX_FIELDS; i++) {
        Vector2 label_size = MeasureTextEx(ui.font, ib.fields[i].label, font_size, 1);
        // TODO: center label correctly instead of just adding 3
        Vector2 label_position = (Vector2){ ib.box.x + INPUT_FIELD_MARGIN,
            ib.fields[i].input_box.y + ib.fields[i].input_box.height/2.0f - label_size.y/2.0f };
        DrawTextEx(ui.font, ib.fields[i].label, label_position, font_size, 1, WHITE);
        DrawRectangleRounded(ib.fields[i].input_box, ib.fields[i].roundness, segments,
                             ib.fields[i].color);
        if (ib.fields[i].is_selected) {
            DrawRectangleRoundedLinesEx(ib.fields[i].input_box, ib.fields[i].roundness,
                                        segments, 2.0f, ib.border_color);
        }
        Vector2 input_position = (Vector2){ ib.fields[i].input_box.x + 2,
            ib.fields[i].input_box.y + ib.fields[i].input_box.height/2.0f - label_size.y/2.0f };
        DrawTextEx(ui.font, ib.fields[i].input, input_position, font_size, 0, WHITE);
    }
}

void draw_ui(UI ui) {
    /* DrawFPS(10, 10); */
    draw_button(ui.play_pause, ui);
    draw_button(ui.toggle_arrows, ui);
    draw_button(ui.toggle_trails, ui);
    draw_button(ui.fast_fwd, ui);
    if (ui.fast_fwd_factor == 0) {
        Vector2 text_size = MeasureTextEx(ui.font, "REWIND", 28, 0.5);
        DrawTextEx(ui.font, "REWIND", (Vector2){GetScreenWidth() - text_size.x - 10, 10}, 28, 0.5, WHITE);
    } else if (ui.fast_fwd_factor > 1) {
        char ff[3] = {0};
        snprintf(ff, 3, "x%d", ui.fast_fwd_factor);
        Vector2 text_size = MeasureTextEx(ui.font, ff, 28, 0.5);
        DrawTextEx(ui.font, ff, (Vector2){GetScreenWidth() - text_size.x - 10, 10}, 28, 0.5, WHITE);
    }
    draw_button(ui.rewind, ui);
}
