#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL.h"
#include <stdbool.h>
#include <stdint.h>

extern int win_width;
extern int win_height;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern uint32_t *colour_buf;
extern SDL_Texture *colour_buf_tex;

bool init_win(void);
void clear_colour_buf(uint32_t colour);
void render_colour_buf(void);
void draw_pixel(const int x, const int y, const uint32_t colour);
void draw_rect(const int x, const int y, const int w, const int h, const uint32_t colour);
void draw_grid(void);
void cleanup(void);

#endif // DISPLAY_H
