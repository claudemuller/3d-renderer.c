#include "SDL.h"
#include "array.h"
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
void free_resources(void);

float fov_factor = 640;

// Stores the 2D projected points to be drawn
triangle_t *triangles_to_render = NULL;

vec3_t camera_pos = { 0, 0, -5 };

bool running = false;
int prev_frame_time = 0;

int main(void)
{
    running = init_win();

    if (!setup()) {
        cleanup();
        return EXIT_FAILURE;
    }

    while (running) {
        process_input();
        update();
        render();
    }

    cleanup();
    free_resources();

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

    // load_cube_mesh_data();
    load_obj("assets/f22.obj");

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

    // Initialise triangles to render
    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.00;
    mesh.rotation.z += 0.00;

    size_t num_faces = array_length(mesh.faces);
    for (size_t i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        // Triangle face vertices
        vec3_t face_vertices[NUM_TRIANGLE_VERTICES] = {
            mesh.vertices[mesh_face.a - 1],
            mesh.vertices[mesh_face.b - 1],
            mesh.vertices[mesh_face.c - 1],
        };

        triangle_t projected_triangle;
        for (size_t j = 0; j < NUM_TRIANGLE_VERTICES; j++) {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // Translate vertex away from camera in z
            transformed_vertex.z += 5;

            // Project current point to a 2D vector to draw
            vec2_t projected_point = project(transformed_vertex);

            // Scale and translate projected point to centre of screen
            projected_point.x += win_width / 2.0;
            projected_point.y += win_height / 2.0;

            projected_triangle.points[j] = projected_point;
        }

        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void)
{
    draw_grid();

    int num_triangles = array_length(triangles_to_render);
    for (size_t i = 0; i < (size_t)num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

        draw_triangle(triangle.points, 0xFFFFFF00);
    }

    // Clear triangles to render
    array_free(triangles_to_render);

    render_colour_buf();
    clear_colour_buf(0xFF000000);

    draw_ui(renderer);

    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    array_free(mesh.faces);
    array_free(mesh.vertices);
    free(colour_buf);
}
