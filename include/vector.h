#ifndef VECTOR_H_
#define VECTOR_H_

#include "raylib.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline Vector2 neg_vec2(Vector2 v) {
    return (Vector2){-v.x, -v.y};
}

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

static inline float mag_vec2_sq(Vector2 v) {
    return v.x * v.x + v.y * v.y;
}

static inline float mag_vec2(Vector2 v) {
    return sqrt(mag_vec2_sq(v));
}

static inline Vector2 normalize_vec2(Vector2 v) {
    return div_vec2_scalar(v, mag_vec2(v));
}

#endif // VECTOR_H_
