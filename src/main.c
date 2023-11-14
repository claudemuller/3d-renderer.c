#include "SDL.h"
#include "array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool setup(void);
void process_input(void);
vec2_t project(const vec3_t point);
void update(void);
void render(void);
void free_resources(void);

float fov_factor = 640;
float zoom = 5.0;

// Stores the 2D projected points to be drawn
triangle_t *triangles_to_render = NULL;

vec3_t camera_pos = { 0, 0, 0 };

bool running = false;
int prev_frame_time = 0;

int main(int argc, char *argv[])
{
    bool debug = false;
    if (argc > 1) {
        debug = strncmp(argv[1], "true", 4) == 0;
    }

    running = init_win(debug);

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
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

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

    load_cube_mesh_data();
    // load_obj("./assets/f22.obj");

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
        switch (ev.key.keysym.sym) {
        case SDLK_ESCAPE: {
            running = false;
        } break;

        case SDLK_1: {
            render_method = RENDER_WIRE;
        } break;

        case SDLK_2: {
            render_method = RENDER_WIRE_VERTEX;
        } break;

        case SDLK_3: {
            render_method = RENDER_FILL_TRIANGLE;
        } break;

        case SDLK_4: {
            render_method = RENDER_FILL_TRIANGLE_WIRE;
        } break;

        case SDLK_c: {
            cull_method = CULL_BACKFACE;
        } break;

        case SDLK_d: {
            cull_method = CULL_NONE;
        } break;
        }
    } break;

    case SDL_MOUSEWHEEL: {
        if (ev.wheel.y < 0) {
            zoom += 0.5;
        } else if (ev.wheel.y > 0) {
            zoom += -0.5;
        }
    } break;
    }
}

// Receives a 3D vector and returns a 2D point.
vec2_t project(const vec3_t point)
{
    return (vec2_t) {
        // p'x = px / pz
        .x = (fov_factor * point.x) / point.z,
        // p'y = py / pz
        .y = (fov_factor * point.y) / point.z
    };
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

    // Change mesh rotation/scale/translation with matrix
    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;
    // mesh.translation.x += 0.01;
    mesh.translation.z = zoom;

    // Create rotation/scale/translation matrix to scale mesh
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(
        mesh.translation.x, mesh.translation.y, mesh.translation.z
    );
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    size_t num_faces = array_length(mesh.faces);
    for (size_t i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        // Triangle face vertices
        vec3_t face_vertices[NUM_TRIANGLE_VERTICES] = {
            mesh.vertices[mesh_face.a - 1],
            mesh.vertices[mesh_face.b - 1],
            mesh.vertices[mesh_face.c - 1],
        };

        vec4_t transformed_vertices[NUM_TRIANGLE_VERTICES];

        for (size_t j = 0; j < NUM_TRIANGLE_VERTICES; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a world matrix with scale/rotation/translation matrices
            // Scale -> rotate -> translate
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        // Check which faces need to be culled
        if (cull_method == CULL_BACKFACE) {
            vec3_t vec_a = vec3_from_vec4(transformed_vertices[0]); /*     A     */
            vec3_t vec_b = vec3_from_vec4(transformed_vertices[1]); /*   /   \   */
            vec3_t vec_c = vec3_from_vec4(transformed_vertices[2]); /*  C --- B  */
            vec3_t vec_ab = vec3_sub(vec_b, vec_a);
            vec3_normalise(&vec_ab);
            vec3_t vec_ac = vec3_sub(vec_c, vec_a);
            vec3_normalise(&vec_ac);

            // Compute face normal by getting the cross product to find the perpendicular
            vec3_t normal = vec3_cross(vec_ab, vec_ac);

            // Normalise the face normal vector i.e. turn it into a unit vector
            vec3_normalise(&normal);

            // Find vector between the triangle and the camera
            vec3_t camera_ray = vec3_sub(camera_pos, vec_a);

            // Calculate the dot product between the camera and triangle normal
            float dot_normal_cam = vec3_dot(normal, camera_ray);

            // Cull if face is facing away from camera
            if (dot_normal_cam < 0) {
                continue;
            }
        }

        vec2_t projected_points[NUM_TRIANGLE_VERTICES];
        for (size_t j = 0; j < NUM_TRIANGLE_VERTICES; j++) {
            // Project current point to a 2D vector to draw
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

            // Scale and translate projected point to centre of screen
            projected_points[j].x += win_width / 2.0;
            projected_points[j].y += win_height / 2.0;
        }

        // Average depth of face based off avg z of translated vertices
        float avg_depth_z = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z)
            / NUM_TRIANGLE_VERTICES;

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y },
            },
            .colour = mesh_face.colour,
            .avg_depth = avg_depth_z
        };

        array_push(triangles_to_render, projected_triangle);
    }

    // Sort faces based on depth with bubble sort
    size_t num_triangles = array_length(triangles_to_render);
    for (size_t i = 0; i < num_triangles; i++) {
        for (size_t j = 0; j < num_triangles; j++) {
            if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
                triangle_t temp = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = temp;
            }
        }
    }
}

void render(void)
{
    draw_grid();

    size_t num_triangles = array_length(triangles_to_render);
    for (size_t i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_fill_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.colour
            );
        }

        if (
            render_method == RENDER_WIRE
            || render_method == RENDER_FILL_TRIANGLE_WIRE
            || render_method == RENDER_WIRE_VERTEX
        ) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF666666
            );
        }

        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFF0000);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFF0000);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFF0000);
        }
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
