#ifndef VECTOR_H_
#define VECTOR_H_

#include "raylib.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline Vector2 add_vec2(Vector2 a, Vector2 b) {
    return (Vector2){a.x + b.x, a.y + b.y};
}

static inline Vector2 sub_vec2(Vector2 a, Vector2 b) {
    return (Vector2){a.x - b.x, a.y - b.y};
}

static inline float dot_vec2(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline Vector2 add_vec2_scalar(Vector2 v, float s) {
    return (Vector2){v.x + s, v.y + s};
}

static inline Vector2 sub_vec2_scalar(Vector2 v, float s) {
    return (Vector2){v.x - s, v.y - s};
}

static inline Vector2 mul_vec2_scalar(Vector2 v, float s) {
    return (Vector2){v.x * s, v.y * s};
}

static inline Vector2 div_vec2_scalar(Vector2 v, float s) {
    return (Vector2){v.x / s, v.y / s};
}

static inline Vector2 normalize_vec2(Vector2 v) {
    float v_mag = sqrt(v.x * v.x + v.y * v.y);
    return div_vec2_scalar(v, v_mag);
}

#endif // VECTOR_H_
