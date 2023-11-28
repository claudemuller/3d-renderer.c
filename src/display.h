#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "SDL.h"
#include "SDL_ttf.h"
#include "triangle.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
};

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE,
};

int get_win_width(void);
int get_win_height(void);

void set_render_method(const int rm);
void set_cull_method(const int cm);

bool is_cull_backface(void);
bool should_render_filled_triangles(void);
bool should_render_texture_triangles(void);
bool should_render_wireframe_triangles(void);
bool should_render_vertices(void);

bool init_win(const bool debug);
void clear_colour_buf(const uint32_t colour);
void clear_zbuf(void);
float get_zbuf_at(const int x, const int y);
void update_zbuf_at(const int x, const int y, float value);
void render_display(void);
void render_colour_buf(void);

void draw_pixel(const int x, const int y, const uint32_t colour);
void draw_line(const int x0, const int y0, const int x1, const int y1, const uint32_t colour);
void draw_rect(const int x, const int y, const int w, const int h, const uint32_t colour);
void draw_grid(void);
void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, const int x, const int y, SDL_Color colour);
void render_ui(SDL_Renderer *renderer);

void cleanup(void);

#endif // DISPLAY_H_
