#include "vector.h"
#include <math.h>

vec3_t vec3_rotate_x(const vec3_t v, const float angle)
{
    vec3_t rotated_vec = {
        // Lock the x axis as we're rotating around it
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vec;
}

vec3_t vec3_rotate_y(const vec3_t v, const float angle)
{
    vec3_t rotated_vec = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        // Lock the y axis as we're rotating around it
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vec;
}

vec3_t vec3_rotate_z(const vec3_t v, const float angle)
{
    vec3_t rotated_vec = {
        .x = v.x * sin(angle) + v.y * cos(angle),
        .y = v.x * cos(angle) - v.y * sin(angle),
        // Lock the z axis as we're rotating around it
        .z = v.z
    };
    return rotated_vec;
}
