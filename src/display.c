#include "display.h"

int win_width = 800;
int win_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t *colour_buf = NULL;
SDL_Texture *colour_buf_tex = NULL;

bool init_win(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "error initialising SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    win_width = display_mode.w;
    win_height = display_mode.h;

    window = SDL_CreateWindow(
        "3d Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_width,
        win_height,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "error creating window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "error creating renderer: %s\n", SDL_GetError());
        return false;
    }

    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void clear_colour_buf(uint32_t colour)
{
    for (int y = 0; y < win_height; y++) {
        for (int x = 0; x < win_width; x++) {
            colour_buf[(win_width * y) + x] = colour;
        }
    }
}

void render_colour_buf(void)
{
    SDL_UpdateTexture(
        colour_buf_tex,
        NULL,
        colour_buf,
        win_width * sizeof(uint32_t)
    );
    SDL_RenderCopy(renderer, colour_buf_tex, NULL, NULL);
}

void draw_pixel(const int x, const int y, const uint32_t colour)
{
    if (x >= 0 && x < win_width && y >= 0 && y < win_height) {
        colour_buf[(win_width * y) + x] = colour;
    }
}

void draw_rect(const int x, const int y, const int w, const int h, const uint32_t colour)
{
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            draw_pixel(j, i, colour);
        }
    }
}

void draw_grid(void)
{
    for (int y = 0; y < win_height; y += 10) {
        for (int x = 0; x < win_width; x += 10) {
            colour_buf[(win_width * y) + x] = 0xFF333333;
        }
    }
}

void cleanup(void)
{
    free(colour_buf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
