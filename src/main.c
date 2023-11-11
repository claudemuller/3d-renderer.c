#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <bits/posix2_lim.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool init_win(void);
bool setup(void);
void process_input(void);
void update(void);
void clear_colour_buf(const uint32_t colour);
void render_colour_buf(void);
void render(void);
void cleanup(void);

int win_width = 800;
int win_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t *colour_buf = NULL;
SDL_Texture *colour_buf_tex = NULL;

bool running = false;

int main(void)
{
    running = init_win();

    if (!setup()) {
        return EXIT_FAILURE;
    }

    while (running) {
        process_input();
        update();
        render();
    }

    cleanup();

    return EXIT_SUCCESS;
}

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

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

bool setup(void)
{
    colour_buf = (uint32_t *)malloc(sizeof(uint32_t) * win_width * win_height);
    if (!colour_buf) {
        fprintf(stderr, "error allocating colour buffer\n");
        return false;
    }

    colour_buf_tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        win_width,
        win_height
    );
    if (!colour_buf_tex) {
        fprintf(stderr, "error creating colour buffer texture: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void process_input(void)
{
    SDL_Event ev;
    SDL_PollEvent(&ev);

    switch (ev.type) {
    case SDL_QUIT: {
        running = false;
    } break;

    case SDL_KEYDOWN: {
        if (ev.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    } break;
    }
}

void update(void)
{
}

void clear_colour_buf(const uint32_t colour)
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

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    render_colour_buf();
    clear_colour_buf(0xFFFFFF00);

    SDL_RenderPresent(renderer);
}

void cleanup(void)
{
    free(colour_buf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
