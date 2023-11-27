#include "matrix.h"
#include "vector.h"
#include <math.h>
#include <stddef.h>

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

mat4_t mat4_make_perspective(const float fov, const float aspect, const float znear, const float zfar)
{
    /*
     *  | (h/w)*1/tan(fov/2)            0          0                0 |
     *  |                  0 1/tan(fov/2)          0                0 |
     *  |                  0            0 zf/(zf-zn) (-zf*zn)/(zf-zn) |
     *  |                  0            0          1                0 |
     */
    mat4_t m = { { { 0 } } };
    m.m[0][0] = aspect * (1 / tan(fov / 2)); // Aspect ration
    m.m[1][1] = 1 / tan(fov / 2); // FOV calculation
    m.m[2][2] = zfar / (zfar - znear); // z normalisation
    m.m[2][3] = (-zfar * znear) / (zfar - znear); // z offset
    m.m[3][2] = 1.0; // Store z value for later use

    return m;
}

vec4_t mat4_mul_vec4(const mat4_t m, const vec4_t v)
{
    vec4_t res = {
        .x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
        .y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
        .z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
        .w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w,
    };

    return res;
}

mat4_t mat4_mul_mat4(const mat4_t m1, const mat4_t m2)
{
    mat4_t m;
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            m.m[i][j] = m1.m[i][0] * m2.m[0][j]
                + m1.m[i][1] * m2.m[1][j]
                + m1.m[i][2] * m2.m[2][j]
                + m1.m[i][3] * m2.m[3][j];
        }
    }

    return m;
}

vec4_t mat4_mul_vec4_project(const mat4_t mat4_proj, const vec4_t v)
{
    vec4_t result = mat4_mul_vec4(mat4_proj, v);

    // Perform perspective divide with original z value stored in w
    if (result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return result;
}

mat4_t mat4_look_at(const vec3_t eye, const vec3_t target, const vec3_t up)
{
    vec3_t z = vec3_sub(target, eye); // forward/z vector
    vec3_normalise(&z);
    vec3_t x = vec3_cross(up, z); // right/x vector (cross product of up and z is perpendicular vector x)
    vec3_normalise(&x);
    vec3_t y = vec3_cross(z, x); // up/y vector (cross product of z and x is perpendicular vector z)

    return (mat4_t) { { { x.x, x.y, x.z, -vec3_dot(x, eye) },
                        { y.x, y.y, y.z, -vec3_dot(y, eye) },
                        { z.x, z.y, z.z, -vec3_dot(z, eye) },
                        { 0, 0, 0, 1 } } };
}
