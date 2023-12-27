#ifndef NBODY_UI_H_
#define NBODY_UI_H_

#include "raylib.h"
#include "nbody_simulation.h"

#define ICON_COUNT 6

#define COLOR_BACKGROUND         BLACK
#define COLOR_VELOCITY_ARROW     RAYWHITE
#define COLOR_ACCELERATION_ARROW GRAY
#define COLOR_HUD_BUTTON_INITIAL COLOR_BACKGROUND
#define COLOR_HUD_BUTTON_PRESSED RAYWHITE
#define COLOR_HUD_BUTTON_HOVERED GRAY
#define COLOR_HUD_BUTTON_BORDER  RAYWHITE
#define HUD_PRIMARY_BUTTON_SIZE   40.0f
#define HUD_SECONDARY_BUTTON_SIZE 30.0f
#define HUD_BUTTON_MARGIN 20.0f

typedef struct Button {
    Rectangle box;
    int icon_index;
    float roundness;
    bool is_pressed;
    bool is_hovered;
    Color color;
    bool has_border;
    Color border_color;
    float border_thickness;
} Button;

typedef struct UI {
    bool is_paused;
    bool arrows_on;
    bool trails_on;
    int fast_forward_factor;
    Color body_colors[10];
    Button play_pause;
    Button toggle_arrows;
    Button toggle_trails;
    Texture2D icons[ICON_COUNT];
} UI;

// Resizes an image to fit inside of a rectangle of size rectangle_size with its
// sides scaled by scale.
void resize_image_to_rectangle(Image *image, Vector2 rectangle_size, float scale);

// Sets up and returns an UI structure with all of its initial values, and
// loads the necessary textures in memory, resizing them apropriately.
UI setup_ui(void);

// Unloads the textures loaded in setup_ui().
void unload_ui(UI *ui);

// For each body, draws a circle of some color, if the body is on screen.
void draw_bodies(Body *bodies, int n_bodies, UI ui);

// Draws arrows that represent the velocity and acceleration vectors of each body,
// in white and grey respectively.
void draw_arrows(Body *bodies, int n_bodies);

// Draws the trail of the bodies, up to a maximum of points (MAX_TRAIL).
void draw_trails(Body *bodies, int n_bodies, UI ui);

// Updates the UI: translate camera on mouse right, zoom camera on mouse wheel,
// and update button colors according to their state.
void update_ui(UI *ui, Camera2D *camera);

// Draws a button according to its values.
void draw_button(Button b, UI ui);

// Draws the buttons in the UI.
void draw_ui(UI ui);

#endif // NBODY_UI_H_
