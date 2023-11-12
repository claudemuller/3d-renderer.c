#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL.h"
#include "triangle.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

enum _cull_method {
    CULL_NONE,
    CULL_BACKFACE
};
extern enum _cull_method cull_method;

enum _render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
};
extern enum _render_method render_method;

extern int win_width;
extern int win_height;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern uint32_t *colour_buf;
extern SDL_Texture *colour_buf_tex;

bool init_win(const bool debug);
void clear_colour_buf(const uint32_t colour);
void render_colour_buf(void);
void draw_pixel(const int x, const int y, const uint32_t colour);
void draw_line(const int x0, const int y0, const int x1, const int y1, const uint32_t colour);
void draw_triangle(
  const int x0, const int y0,
  const int x1, const int y1,
  const int x2, const int y2,
  uint32_t colour
);
void draw_rect(const int x, const int y, const int w, const int h, const uint32_t colour);
void draw_grid(void);
void draw_ui(SDL_Renderer *renderer);

void cleanup(void);

#endif // DISPLAY_H
