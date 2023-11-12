#include "vector.h"
#include <math.h>

float vec2_length(const vec2_t v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(const vec2_t v1, const vec2_t v2)
{
    return (vec2_t) {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
    };
}

vec2_t vec2_sub(const vec2_t v1, const vec2_t v2)
{
    return (vec2_t) {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
    };
}

vec2_t vec2_mul(const vec2_t v, const float f)
{
    return (vec2_t) {
        .x = v.x * f,
        .y = v.y * f,
    };
}

vec2_t vec2_div(const vec2_t v, const float f)
{
    return (vec2_t) {
        .x = v.x / f,
        .y = v.y / f,
    };
}

float vec2_dot(const vec2_t v1, const vec2_t v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

void vec2_normalise(vec2_t *v)
{
    float len = vec2_length(*v);
    v->x /= len;
    v->y /= len;
}

float vec3_length(const vec3_t v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(const vec3_t v1, const vec3_t v2)
{
    return (vec3_t) {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z,
    };
}

vec3_t vec3_sub(const vec3_t v1, const vec3_t v2)
{
    return (vec3_t) {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
    };
}

vec3_t vec3_mul(const vec3_t v, const float f)
{
    return (vec3_t) {
        .x = v.x * f,
        .y = v.y * f,
        .z = v.z * f,
    };
}

vec3_t vec3_div(const vec3_t v, const float f)
{
    return (vec3_t) {
        .x = v.x / f,
        .y = v.y / f,
        .z = v.z / f,
    };
}

vec3_t vec3_cross(const vec3_t v1, const vec3_t v2)
{
    return (vec3_t) {
        .x = v1.y * v2.z - v1.z * v2.y,
        .y = v1.z * v2.x - v1.x * v2.z,
        .z = v1.x * v2.y - v1.y * v2.x,
    };
}

float vec3_dot(const vec3_t v1, const vec3_t v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void vec3_normalise(vec3_t *v)
{
    float len = vec3_length(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

vec3_t vec3_rotate_x(const vec3_t v, const float angle)
{
    return (vec3_t) {
        // Lock the x axis as we're rotating around it
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle),
    };
}

vec3_t vec3_rotate_y(const vec3_t v, const float angle)
{
    return (vec3_t) {
        .x = v.x * cos(angle) - v.z * sin(angle),
        // Lock the y axis as we're rotating around it
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle),
    };
}

vec3_t vec3_rotate_z(const vec3_t v, const float angle)
{
    return (vec3_t) {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        // Lock the z axis as we're rotating around it
        .z = v.z,
    };
}
