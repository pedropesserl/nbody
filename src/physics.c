#include <math.h>
#include "raylib.h"

float lerp(float a, float b, float t) {
    return (1 - t)*a + t*b;
}

float get_pixel_size(int monitor) {
    int monitor_width = GetMonitorWidth(monitor);
    int monitor_physical_width = GetMonitorPhysicalWidth(monitor);
    return ((float)monitor_physical_width / monitor_width) / 1000.0f;
}

float convert_acceleration(float mpss, int fps) {
    float pixels_in_a_meter = 1.0f / get_pixel_size(GetCurrentMonitor());
    float conversion_rate = pixels_in_a_meter / (fps * fps);
    return mpss * conversion_rate;
}
