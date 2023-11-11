#include "SDL.h"
#include "display.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool init_win(void);
bool setup(void);
vec2_t project(const vec3_t point);
void process_input(void);
void update(void);
void render(void);

#define NUM_POINTS (9 * 9 * 9)

vec3_t cube_points[NUM_POINTS];
vec2_t projeced_points[NUM_POINTS];
float fov_factor = 128;

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

    int point_counter = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                vec3_t point = { x, y, z };
                cube_points[point_counter++] = point;
            }
        }
    }

    return true;
}

// Receives a 3D vector and returns a 2D point.
vec2_t project(const vec3_t point)
{
    vec2_t projected_point = {
        .x = (fov_factor * point.x),
        .y = (fov_factor * point.y)
    };
    return projected_point;
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
    for (int i = 0; i < NUM_POINTS; i++) {
        vec3_t point = cube_points[i];
        vec2_t projected_point = project(point);
        projeced_points[i] = projected_point;
    }
}

void render(void)
{
    draw_grid();

    for (int i = 0; i < NUM_POINTS; i++) {
        vec2_t p = projeced_points[i];
        draw_rect(
            p.x + (int)(win_width / 2),
            p.y + (int)(win_height / 2),
            4,
            4,
            0xFFFF00FF
        );
    }

    render_colour_buf();
    clear_colour_buf(0xFF000000);

    SDL_RenderPresent(renderer);
}
