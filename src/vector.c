#include "vector.h"
#include <math.h>

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
