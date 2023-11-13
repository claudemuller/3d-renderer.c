#include "matrix.h"

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

vec4_t mat4_mul_vec4(const mat4_t m, const vec4_t v)
{
    return (vec4_t) {
        .x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
        .y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
        .z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
        .w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w,
    };
}
