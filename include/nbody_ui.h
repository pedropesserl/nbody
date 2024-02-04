#ifndef NBODY_UI_H_
#define NBODY_UI_H_

#include "raylib.h"
#include "nbody_simulation.h"

#define ICON_COUNT 10

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
#define HUD_INPUT_BOX_HEIGHT 160.0f
#define INPUT_FIELD_MARGIN 5.0f
#define MAX_LABEL_CHARACTERS 15
#define MAX_INPUT_CHARACTERS 12
#define MAX_FIELDS 4

typedef enum Icon_Index {
    ICON_IDX_PAUSE = 0,
    ICON_IDX_PLAY = 1,
    ICON_IDX_ARROWS_OFF = 2,
    ICON_IDX_ARROWS_ON = 3,
    ICON_IDX_TRAILS_OFF = 4,
    ICON_IDX_TRAILS_ON = 5,
    ICON_IDX_CONFIRM = 6,
    ICON_IDX_CANCEL = 7,
    ICON_IDX_FAST_FWD = 8,
    ICON_IDX_REWIND = 9,
} Icon_Index;

typedef enum Button_ID {
    BUTTON_PLAY_PAUSE = 0,
    BUTTON_TOGGLE_ARROWS = 1,
    BUTTON_TOGGLE_TRAILS = 2,
    BUTTON_CONFIRM = 3,
    BUTTON_CANCEL = 4,
    BUTTON_FAST_FWD = 5,
    BUTTON_REWIND = 6,
} Button_ID;

typedef enum Button_Type {
    BUTTON_PRIMARY = 0,
    BUTTON_SECONDARY = 1,
} Button_Type;

typedef struct Button {
    Button_ID id;
    Button_Type type;
    Rectangle box;
    Icon_Index icon_index;
    float roundness;
    bool is_pressed;
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
    bool is_on;
    bool is_invoked;
} Input_Box;

typedef struct UI {
    bool is_paused;
    bool arrows_on;
    bool trails_on;
    int fast_fwd_factor;
    bool rewind_on;
    Color body_colors[10];
    Button play_pause;
    Button toggle_arrows;
    Button toggle_trails;
    Button fast_fwd;
    Button rewind;
    Input_Box body_input;
    Vector2 position_to_create_body;
    bool created_body_with_input;
    Texture2D icons[ICON_COUNT];
    Font font;
} UI;

// Resizes an image to fit inside of a rectangle of size rectangle_size with its
// sides scaled by scale.
void resize_image_to_rectangle(Image *image, Vector2 rectangle_size, float scale);

// Returns a new button with the indicated id and type, at position (0,0).
Button new_button(Button_ID id, Button_Type type);

// Returns a new string input with the indicated label of length lable_len,
// at position pos and with size size.
Str_Input new_string_input(const char *label, size_t lable_len, Vector2 pos, Vector2 size);

// Returns a new input box at the specified position.
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

// Updates a toggle button and returns a bool that indicates whether the mouse
// is on that button.
bool update_toggle_button(Button *button, Button_ID id, Vector2 mouse, UI *ui);

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
