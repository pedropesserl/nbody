#ifndef NBODY_UI_H_
#define NBODY_UI_H_

#include "raylib.h"
#include "nbody_simulation.h"

#define ICON_COUNT 8

#define COLOR_BACKGROUND          BLACK
#define COLOR_VELOCITY_ARROW      WHITE
#define COLOR_ACCELERATION_ARROW  GRAY
#define COLOR_HUD_BUTTON_INITIAL  COLOR_BACKGROUND
#define COLOR_HUD_BUTTON_PRESSED  WHITE
#define COLOR_HUD_BUTTON_HOVERED  GRAY
#define COLOR_HUD_BUTTON_BORDER   WHITE
#define COLOR_INPUT_BOX           COLOR_BACKGROUND
#define COLOR_INPUT_FIELD_INITIAL DARKGRAY
#define COLOR_INPUT_FIELD_HOVERED GRAY

#define HUD_PRIMARY_BUTTON_SIZE   40.0f
#define HUD_SECONDARY_BUTTON_SIZE 30.0f
#define HUD_BUTTON_MARGIN 20.0f

#define HUD_INPUT_BOX_WIDTH 266.0f
#define HUD_INPUT_BOX_HEIGHT 135.0f
#define INPUT_FIELD_MARGIN 5.0f
#define MAX_LABEL_CHARACTERS 15
#define MAX_INPUT_CHARACTERS 10
#define MAX_FIELDS 3

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

typedef struct Str_Input {
    char label[MAX_LABEL_CHARACTERS + 1];
    char input[MAX_INPUT_CHARACTERS + 1];
    float roundness;
    Rectangle input_box;
    bool is_selected;
    bool is_hovered;
    Color color;
    float input_value;
    int char_count;
    bool has_period;
} Str_Input;

typedef struct Input_Box {
    Rectangle box;
    float roundness;
    Color color;
    bool has_border;
    Color border_color;
    float border_thickness;
    Str_Input fields[MAX_FIELDS];
    Button confirm;
    Button cancel;
    bool active;
} Input_Box;

typedef struct UI {
    bool is_paused;
    bool arrows_on;
    bool trails_on;
    int fast_forward_factor;
    Color body_colors[10];
    Button play_pause;
    Button toggle_arrows;
    Button toggle_trails;
    Input_Box body_input;
    Vector2 position_to_generate_body;
    bool generated_body_with_input;
    Texture2D icons[ICON_COUNT];
} UI;

// Resizes an image to fit inside of a rectangle of size rectangle_size with its
// sides scaled by scale.
void resize_image_to_rectangle(Image *image, Vector2 rectangle_size, float scale);

// Returns a new input box with the specified position.
Input_Box new_input_box(Vector2 position);

// Sets up and returns an UI structure with all of its initial values, and
// loads the necessary textures in memory, resizing them apropriately.
UI setup_ui(void);

// Unloads the textures loaded in setup_ui().
void unload_ui(UI *ui);

// For each body, draws a circle of some color.
void draw_bodies(Bodies bodies, UI ui);

// Draws arrows that represent the velocity and acceleration vectors of each body,
// in white and grey respectively.
void draw_arrows(Bodies bodies);

// Draws the trail of the bodies, up to a maximum of points (MAX_TRAIL).
void draw_trails(Bodies bodies, UI ui);

// Updates the UI: translate camera on mouse right, zoom camera on mouse wheel,
// update button colors according to their state and update input box.
void update_ui(UI *ui, Camera2D *camera, Bodies *bodies);

// Draws a button according to its values and using the icons in the UI.
void draw_button(Button b, UI ui);

// Draws an input box according to its values and using the icons in the UI.
void draw_input_box(Input_Box ib, UI ui);

// Draws the buttons in the UI and the input box, if it's active.
void draw_ui(UI ui);

#endif // NBODY_UI_H_
