#include "SDL_events.h"
#include "display.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>

#define SWAP(a, b) _Generic((a), int *: int_swap, float *: float_swap)(a, b)

static void int_swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

static void float_swap(float *a, float *b)
{
    float t = *a;
    *a = *b;
    *b = t;
}

void draw_triangle(const int x0, const int y0, const int x1, const int y1, const int x2, const int y2, uint32_t colour)
{
    draw_line(x0, y0, x1, y1, colour);
    draw_line(x1, y1, x2, y2, colour);
    draw_line(x2, y2, x0, y0, colour);
}

void draw_fill_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    const uint32_t colour
)
{
    // Sort vertices by y-coord asc (y0 < y1 < y2)
    if (y0 > y1) {
        SWAP(&y0, &y1);
        SWAP(&x0, &x1);
        SWAP(&z0, &z1);
        SWAP(&w0, &w1);
    }
    if (y1 > y2) {
        SWAP(&y1, &y2);
        SWAP(&x1, &x2);
        SWAP(&z1, &z2);
        SWAP(&w1, &w2);
    }
    if (y0 > y1) {
        SWAP(&y0, &y1);
        SWAP(&x0, &x1);
        SWAP(&z0, &z1);
        SWAP(&w0, &w1);
    }

    const vec4_t point_a = { x0, y0, z0, w0 };
    const vec4_t point_b = { x1, y1, z1, w1 };
    const vec4_t point_c = { x2, y2, z2, w2 };

    // Render the top of the triangle i.e. the flat bottomed triangle
    // Inverse slope because we need to calculate the y increment
    float inv_slope1 = 0;
    if (y1 - y0 != 0) { // Guard against div by zero
        inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
    }

    float inv_slope2 = 0;
    if (y2 - y0 != 0) { // Guard against div by zero
        inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int xstart = x1 + (y - y1) * inv_slope1;
            int xend = x0 + (y - y0) * inv_slope2;

            if (xend < xstart) {
                SWAP(&xstart, &xend);
            }

            for (int x = xstart; x < xend; x++) {
                draw_triangle_pixel(x, y, colour, point_a, point_b, point_c);
            }
        }
    }

    // Render the bottom of the triangle i.e. the flat topped triangle
    // Inverse slope because we need to calculate the y increment
    inv_slope1 = 0;
    if (y2 - y1 != 0) { // Guard against div by zero
        inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
    }

    inv_slope2 = 0;
    if (y2 - y0 != 0) { // Guard against div by zero
        inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int xstart = x1 + (y - y1) * inv_slope1;
            int xend = x0 + (y - y0) * inv_slope2;

            if (xend < xstart) {
                SWAP(&xstart, &xend);
            }

            for (int x = xstart; x < xend; x++) {
                draw_triangle_pixel(x, y, colour, point_a, point_b, point_c);
            }
        }
    }
}

vec3_t barycentric_weights(const vec2_t a, const vec2_t b, vec2_t c, vec2_t p)
{
    /*
     *          B
     *         /|\
     *        / | \
     *       /  |  \
     *      /  (p)  \
     *     /  /   \  \
     *    / /       \ \
     *   //___________\\
     *  A               C
     */
    // Vectors between vertices
    const vec2_t ac = vec2_sub(c, a);
    const vec2_t ab = vec2_sub(b, a);
    const vec2_t ap = vec2_sub(p, a);
    const vec2_t pc = vec2_sub(c, p);
    const vec2_t pb = vec2_sub(b, p);

    // Calculate area of ABC (parallegram/triangle ABC) "2D cross product"
    const float area_para_abc = ac.x * ab.y - ac.y * ab.x; // || AC x AB ||

    // α which is area PBC / area ABC
    const float alpha = (pc.x * pb.y - pc.y * pb.x) / area_para_abc;

    // β which is area APC / area ABC
    const float beta = (ac.x * ap.y - ac.y * ap.x) / area_para_abc;

    // γ is what's left of 1.0 - α - β because barycentric coords add up to 1.0
    const float gamma = 1 - alpha - beta;

    return (vec3_t) {
        alpha, beta, gamma
    };
}

void draw_triangle_pixel(
    const int x, const int y,
    const uint32_t colour,
    const vec4_t point_a, const vec4_t point_b, const vec4_t point_c
)
{
    const vec2_t p = { x, y };
    const vec2_t a = vec2_from_vec4(point_a);
    const vec2_t b = vec2_from_vec4(point_b);
    const vec2_t c = vec2_from_vec4(point_c);
    const vec3_t weights = barycentric_weights(a, b, c, p);
    const float alpha = weights.x;
    const float beta = weights.y;
    const float gamma = weights.z;

    // Interpolate the value of 1/w for current pixel
    // TODO: pull out this reciprocal calc out of this func
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Adjust 1/w so that pixels closer to camera are smaller than those behind
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw pixel if depth value is less than what was already in z_buf
    if (interpolated_reciprocal_w < get_zbuf_at(x, y)) {
        draw_pixel(x, y, colour);

        // Update z_buf with the 1/w of current pixel
        update_zbuf_at(x, y, interpolated_reciprocal_w);
    }
}

void draw_triangle_texel(
    const int x, const int y,
    const upng_t *texture,
    const vec4_t point_a, const vec4_t point_b, const vec4_t point_c,
    const tex2_t a_uv, const tex2_t b_uv, const tex2_t c_uv
)
{
    const vec2_t p = { x, y };
    const vec2_t a = vec2_from_vec4(point_a);
    const vec2_t b = vec2_from_vec4(point_b);
    const vec2_t c = vec2_from_vec4(point_c);
    const vec3_t weights = barycentric_weights(a, b, c, p);
    const float alpha = weights.x;
    const float beta = weights.y;
    const float gamma = weights.z;

    // Interpolate U/w and V/w values using barycentric weights and a factor of 1/w
    float interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    float interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    // Interpolate the value of 1/w for current pixel
    // TODO: pull out this reciprocal calc out of this func
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Divide the values back by 1/w to "reverse" the reciprocal calulation
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    const int texture_width = upng_get_width(texture);
    const int texture_height = upng_get_height(texture);

    // Map UV coords to texture width and height
    const int tex_x = abs((int)(interpolated_u * texture_width) % texture_width); // Clamp value within tex width
    const int tex_y = abs((int)(interpolated_v * texture_width) % texture_height); // Clamp value within tex height

    // Adjust 1/w so that pixels closer to camera are smaller than those behind
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw pixel if depth value is less than what was already in z_buf
    if (interpolated_reciprocal_w < get_zbuf_at(x, y)) {
        const uint32_t *texture_buffer = (uint32_t *)upng_get_buffer(texture);

        draw_pixel(x, y, texture_buffer[(texture_width * tex_y) + tex_x]);

        // Update z_buf with the 1/w of current pixel
        update_zbuf_at(x, y, interpolated_reciprocal_w);
    }
}

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    const upng_t *texture
)
{
    // Sort vertices by y-coord asc (y0 < y1 < y2)
    if (y0 > y1) {
        SWAP(&y0, &y1);
        SWAP(&x0, &x1);
        SWAP(&z0, &z1);
        SWAP(&w0, &w1);
        SWAP(&u0, &u1);
        SWAP(&v0, &v1);
    }
    if (y1 > y2) {
        SWAP(&y1, &y2);
        SWAP(&x1, &x2);
        SWAP(&z1, &z2);
        SWAP(&w1, &w2);
        SWAP(&u1, &u2);
        SWAP(&v1, &v2);
    }
    if (y0 > y1) {
        SWAP(&y0, &y1);
        SWAP(&x0, &x1);
        SWAP(&z0, &z1);
        SWAP(&w0, &w1);
        SWAP(&u0, &u1);
        SWAP(&v0, &v1);
    }

    // Flip V component to account for inverted UV coords
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    const vec4_t point_a = { x0, y0, z0, w0 };
    const vec4_t point_b = { x1, y1, z1, w1 };
    const vec4_t point_c = { x2, y2, z2, w2 };
    const tex2_t a_uv = { u0, v0 };
    const tex2_t b_uv = { u1, v1 };
    const tex2_t c_uv = { u2, v2 };

    // Render the top of the triangle i.e. the flat bottomed triangle
    // Inverse slope because we need to calculate the y increment
    float inv_slope1 = 0;
    if (y1 - y0 != 0) { // Guard against div by zero
        inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
    }

    float inv_slope2 = 0;
    if (y2 - y0 != 0) { // Guard against div by zero
        inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int xstart = x1 + (y - y1) * inv_slope1;
            int xend = x0 + (y - y0) * inv_slope2;

            if (xend < xstart) {
                SWAP(&xstart, &xend);
            }

            for (int x = xstart; x < xend; x++) {
                draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    // Render the bottom of the triangle i.e. the flat topped triangle
    // Inverse slope because we need to calculate the y increment
    inv_slope1 = 0;
    if (y2 - y1 != 0) { // Guard against div by zero
        inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
    }

    inv_slope2 = 0;
    if (y2 - y0 != 0) { // Guard against div by zero
        inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int xstart = x1 + (y - y1) * inv_slope1;
            int xend = x0 + (y - y0) * inv_slope2;

            if (xend < xstart) {
                SWAP(&xstart, &xend);
            }

            for (int x = xstart; x < xend; x++) {
                draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
}

vec3_t get_triangle_normal(vec4_t vertices[NUM_TRIANGLE_VERTICES])
{
            // Check which faces need to be culled
    vec3_t vec_a = vec3_from_vec4(vertices[0]); /*   A   */
    vec3_t vec_b = vec3_from_vec4(vertices[1]); /*  / \  */
    vec3_t vec_c = vec3_from_vec4(vertices[2]); /* C---B */

            // Do vec subtraction of B-A and C-A
    vec3_t vec_ab = vec3_sub(vec_b, vec_a);
    vec3_t vec_ac = vec3_sub(vec_c, vec_a);

    vec3_normalise(&vec_ab);
    vec3_normalise(&vec_ac);

            // Find the vector between vertex A in the triangle and the camera origin
    vec3_t normal = vec3_cross(vec_ab, vec_ac);

            // Normalise the face normal vector i.e. turn it into a unit vector
    vec3_normalise(&normal);

    return normal;
}
