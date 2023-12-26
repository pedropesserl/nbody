#ifndef NBODY_UI_H_
#define NBODY_UI_H_

#include "raylib.h"
#include "nbody_simulation.h"

typedef struct Button {
    Rectangle box;
    int icon_index;
    int bg_color_index;
    float padding;
    float roundness;
    bool has_border;
    Color border_color;
    float border_thickness;
} Button;

// Resizes an image to fit inside of a rectangle of size rectangle_size with its
// sides scaled by scale.
void resize_image_to_rectangle(Image *image, Vector2 rectangle_size, float scale);

typedef struct UI {
    bool arrows_on;
    bool trails_on;
    Color button_bgs[2];
    Button toggle_arrows;
    Button toggle_trails;
    Texture2D icons[4];
} UI;

UI setup_ui(void);

void unload_ui(UI *ui);

extern Color body_colors[10];

// For each body, draws a circle of some color, if the body is on screen.
void draw_bodies(Body *bodies, int n_bodies);

// Draws arrows that represent the velocity and acceleration vectors of each body,
// in white and grey respectively.
void draw_arrows(Body *bodies, int n_bodies);

// Draws the trail of the bodies, up to a maximum of points (MAX_TRAIL).
void draw_trails(Body *bodies, int n_bodies);

void translate_camera_on_m2(Camera2D *camera);

void zoom_camera_on_mouse_wheel(Camera2D *camera, float wheel);

void update_ui(UI *ui, Camera2D *camera);

void draw_button(Button b, UI ui);

void draw_ui(UI ui);

#endif // NBODY_UI_H_
