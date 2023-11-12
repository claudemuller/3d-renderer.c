#include "SDL.h"
#include "display.h"
#include "mesh.h"
#include "triangle.h"
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

float fov_factor = 640;

// Stores the 2D projected points to be drawn
triangle_t triangles_to_render[NUM_MESH_FACES];

vec3_t camera_pos = { 0, 0, -5 };
vec3_t cube_rotation = { 0, 0, 0 };

bool running = false;
int prev_frame_time = 0;

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

// Receives a 3D vector and returns a 2D point.
vec2_t project(const vec3_t point)
{
    vec2_t projected_point = {
        // p'x = px / pz
        .x = (fov_factor * point.x) / point.z,
        // p'y = py / pz
        .y = (fov_factor * point.y) / point.z
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
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - prev_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    prev_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    for (int i = 0; i < NUM_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];

        // Triangle face vertices
        vec3_t face_vertices[NUM_TRIANGLE_VERTICES] = {
            mesh_vertices[mesh_face.a - 1],
            mesh_vertices[mesh_face.b - 1],
            mesh_vertices[mesh_face.c - 1],
        };

        triangle_t projected_triangle;
        for (int j = 0; j < NUM_TRIANGLE_VERTICES; j++) {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            // Translate vertex away from camera in z
            transformed_vertex.z -= camera_pos.z;

            // Project current point to a 2D vector to draw
            vec2_t projected_point = project(transformed_vertex);

            // Scale and translate projected point to centre of screen
            projected_point.x += (float)win_width / 2;
            projected_point.y += (float)win_height / 2;

            projected_triangle.points[j] = projected_point;
        }

        triangles_to_render[i] = projected_triangle;
    }
}

void render(void)
{
    draw_grid();

    for (int i = 0; i < NUM_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];

        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

        draw_triangle(triangle.points, 0xFFFFFF00);
    }

    render_colour_buf();
    clear_colour_buf(0xFF000000);

    SDL_RenderPresent(renderer);
}
