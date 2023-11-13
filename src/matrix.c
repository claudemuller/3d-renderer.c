#include "matrix.h"
#include <math.h>

mat4_t mat4_identity(void)
{
    /*
     *  |  1  0  0  0  |
     *  |  0  1  0  0  |
     *  |  0  0  1  0  |
     *  |  0  0  0  1  |
     */
    return (mat4_t) {
        .m = {
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 },
        }
    };
}

mat4_t mat4_make_scale(const float sx, const float sy, const float sz)
{
    /*
     *  | sx  0  0  0  |
     *  |  0 sy  0  0  |
     *  |  0  0 sz  0  |
     *  |  0  0  0  1  |
     */
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    return m;
}

mat4_t mat4_make_translation(const float tx, const float ty, const float tz)
{
    /*
     *  |  1  0  0 tx  |
     *  |  0  1  0 ty  |
     *  |  0  0  1 tz  |
     *  |  0  0  0  1  |
     */
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;

    return m;
}

mat4_t mat4_make_rotation_x(const float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    /*
     *  |  1  0  0  0  |
     *  |  0  c  s  0  |
     *  |  0 -s  c  0  |
     *  |  0  0  0  1  |
     */
    mat4_t m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = s;
    m.m[2][1] = -s;
    m.m[2][2] = c;

    return m;
}

mat4_t mat4_make_rotation_y(const float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    /*
     * The sin and -sin are inverted for left-handed/clocwise rotation
     *  |  c  0 -s  0  |
     *  |  0  1  0  0  |
     *  |  s  0  c  0  |
     *  |  0  0  0  1  |
     */
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = -s;
    m.m[2][0] = s;
    m.m[2][2] = c;

    return m;
}

mat4_t mat4_make_rotation_z(const float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    /*
     *  |  c  s  0  0  |
     *  | -s  c  0  0  |
     *  |  0  0  1  0  |
     *  |  0  0  0  1  |
     */
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = s;
    m.m[1][0] = -s;
    m.m[1][1] = c;

    return m;
}

vec4_t mat4_mul_vec4(const mat4_t m, const vec4_t v)
{
    return (vec4_t) {
        .x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
        .y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
        .z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
        .w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w,
    };
}
