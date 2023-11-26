#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "texture.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	int a;
	int b;
	int c;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t colour;
} face_t;

#define NUM_TRIANGLE_VERTICES 3

typedef struct {
	vec4_t points[NUM_TRIANGLE_VERTICES];
	tex2_t texcoords[NUM_TRIANGLE_VERTICES];
	float avg_depth;
	uint32_t colour;
} triangle_t;

void draw_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, const uint32_t colour);
vec3_t barycentric_weights(const vec2_t a, const vec2_t b, vec2_t c, vec2_t p);
void draw_texel(
    const int x, const int y,
    const uint32_t *texture,
    const vec4_t point_a, const vec4_t point_b, const vec4_t point_c,
    const float u0, const float v0, const float u1, const float v1, const float u2, const float v2
);
void draw_textured_triangle(
  int x0, int y0, float z0, float w0, float u0, float v0,
  int x1, int y1, float z1, float w1, float u1, float v1,
  int x2, int y2, float z2, float w2, float u2, float v2,
  const uint32_t* texture
);
void fill_flat_bottom_triangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    uint32_t colour
);
void fill_flat_top_triangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    uint32_t colour
);

#endif // TRIANGLE_H_
