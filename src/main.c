#include "SDL.h"
#include <stdbool.h>
#include <stdio.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

bool init_win(void);
void setup(void);
void process_input(void);
void update(void);
void render(void);
void cleanup(void);

SDL_Window *window;
SDL_Renderer *renderer;
bool running = false;

int main(int argc, char *argv[])
{
    running = init_win();

    setup();

    while (running) {
        process_input();
        update();
        render();
    }

    cleanup();

    return 0;
}

bool init_win(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "error initialising SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "3d Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "error creating window: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "error creating renderer: %s", SDL_GetError());
        return false;
    }

    return true;
}

void setup(void)
{
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
    SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect = {
        .x = 10,
        .y = 10,
        .w = 100,
        .h = 100
    };
    SDL_SetRenderDrawColor(renderer, 33, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}

void cleanup(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
