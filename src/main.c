#include "SDL.h"
#include "display.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool init_win(void);
bool setup(void);
void process_input(void);
void update(void);
void render(void);

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

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_grid();
    draw_rect(100, 100, 100, 100, 0xFFFF00FF);

    render_colour_buf();
    clear_colour_buf(0xFF000000);

    SDL_RenderPresent(renderer);
}
