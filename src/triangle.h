#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include <stdint.h>

typedef struct {
	int a;
	int b;
	int c;
	uint32_t colour;
} face_t;

#define NUM_TRIANGLE_VERTICES 3

typedef struct {
	vec2_t points[NUM_TRIANGLE_VERTICES];
	uint32_t colour;
	float avg_depth;
} triangle_t;

void draw_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t colour);
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
void int_swap(int *a, int *b);

#endif // TRIANGLE_H
