#include "display.h"
#include "triangle.h"
#include <stddef.h>

void draw_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t colour)
{
    // Sort vertices by y-coord asc (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2) {
        // Draw the top half of the triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, colour);
        return;
    }

    if (y0 == y1) {
        // Draw the bottom half of the triangle
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, colour);
        return;
    }

    // Calculate triangle midpoint
    int my = y1;
    int mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

    // Draw the top half of the triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, colour);

    // Draw the bottom half of the triangle
    fill_flat_top_triangle(x1, y1, mx, my, x2, y2, colour);
}

void fill_flat_bottom_triangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    uint32_t colour
)
{
    /*
     *                  x0,y0
     *                   /\
     *           slope1 /  \ slope2
     *                 /    \
     *                /______\
     *             x1,y1    x2,y2
     */
    // Inverse slope because we need to calculate the y increment
    const float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    const float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    // The start and end of the horizontal scanline
    float x_start = x0;
    float x_end = x0;

    for (size_t y = y0; y <= (size_t)y2; y++) {
        // Draw the scanline
        draw_line(x_start, y, x_end, y, colour);

        // Moves to the left of the triangle
        x_start += inv_slope_1;
        // Moves to the right of the triangle
        x_end += inv_slope_2;
    }
}

void fill_flat_top_triangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    uint32_t colour
)
{
    /*
     *           x0,y0      x1,y1
     *              ----------
     *               \      /
     *         slope1 \    / slope2
     *                 \  /
     *                  \/
     *                x2,y2
     */
    // Inverse slope because we need to calculate the y increment
    const float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    const float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    // The start and end of the horizontal scanline
    float x_start = x2;
    float x_end = x2;

    for (size_t y = y2; y >= (size_t)y0; y--) {
        // Draw the scanline
        draw_line(x_start, y, x_end, y, colour);

        // Moves to the left of the triangle
        x_start -= inv_slope_1;
        // Moves to the right of the triangle
        x_end -= inv_slope_2;
    }
}

void int_swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
