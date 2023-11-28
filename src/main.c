#include "SDL.h"
#include "array.h"
#include "camera.h"
#include "clipping.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool setup(void);
void process_input(void);
vec2_t project(const vec3_t point);
void update(void);
void render(void);
void free_resources(void);

// Stores the 2D projected points to be drawn
#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool running = false;
int prev_frame_time = 0;
float delta_time = 0;

mat4_t proj_matrix = { 0 };
mat4_t view_matrix = { 0 };

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
    render_method = RENDER_FILL_TRIANGLE;
    cull_method = CULL_BACKFACE;
    lighting = true;

    colour_buf = (uint32_t *)malloc(sizeof(uint32_t) * win_width * win_height);
    if (!colour_buf) {
        fprintf(stderr, "error allocating colour buffer\n");
        return false;
    }

    z_buf = (float *)malloc(sizeof(float) * win_width * win_height);
    if (!z_buf) {
        fprintf(stderr, "error allocating z buffer\n");
        return false;
    }

    colour_buf_tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        win_width,
        win_height
    );
    if (!colour_buf_tex) {
        fprintf(stderr, "error creating colour buffer texture: %s\n", SDL_GetError());
        return false;
    }

    // Init perspective projection matrix
    const float aspectx = win_width / (float)win_height;
    const float aspecty = win_height / (float)win_width;
    const float fovy = M_PI / 3.0; // 60° (180/3)
    const float fovx = atan(tan(fovy / 2) * aspectx) * 2.0;
    const float znear = 0.1;
    const float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

    init_frustum_planes(fovx, fovy, znear, zfar);

    // load_cube_mesh_data();
    load_obj("./assets/cube.obj");

    load_png_texture_data("./assets/cube.png");

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
            lighting = false;
        } break;

        case SDLK_2: {
            render_method = RENDER_WIRE_VERTEX;
            lighting = false;
        } break;

        case SDLK_3: {
            render_method = RENDER_FILL_TRIANGLE;
        } break;

        case SDLK_4: {
            render_method = RENDER_FILL_TRIANGLE_WIRE;
        } break;

        case SDLK_5: {
            render_method = RENDER_FILL_TRIANGLE;
            lighting = true;
        } break;

        case SDLK_6: {
            render_method = RENDER_TEXTURED;
            lighting = true;
        } break;

        case SDLK_7: {
            render_method = RENDER_TEXTURED_WIRE;
            lighting = true;
        } break;

        case SDLK_c: {
            cull_method = CULL_BACKFACE;
        } break;

        case SDLK_x: {
            cull_method = CULL_NONE;
        } break;

        case SDLK_p: {
            rot = rot == 0 ? 0.005 : 0;
            is_paused = !is_paused;
        } break;

        case SDLK_w: {
            camera.forward_vel = vec3_mul(camera.direction, 5.0 * delta_time);
            camera.pos = vec3_add(camera.pos, camera.forward_vel);
        } break;

        case SDLK_s: {
            camera.forward_vel = vec3_mul(camera.direction, 5.0 * delta_time);
            camera.pos = vec3_sub(camera.pos, camera.forward_vel);
        } break;

        case SDLK_a: {
            camera.yaw += 1.0 * delta_time;
        } break;

        case SDLK_d: {
            camera.yaw -= 1.0 * delta_time;
        } break;

        case SDLK_UP: {
            camera.pos.y += 3.0 * delta_time;
        } break;

        case SDLK_DOWN: {
            camera.pos.y -= 3.0 * delta_time;
        } break;
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

    delta_time = (SDL_GetTicks() - prev_frame_time) / 1000.0;
    prev_frame_time = SDL_GetTicks();

    // Initialize triangles to render counter for current frame
    num_triangles_to_render = 0;

    // Change the mesh scale/rotation/translation with matrix
    // mesh.rotation.x += 0.6 * delta_time;
    // mesh.rotation.y += 0.6 * delta_time;
    // mesh.rotation.z += 0.6 * delta_time;
    mesh.translation.z = 5.0;

    // Create view matrix looking
    vec3_t target = { 0, 0, 1 };
    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));
    target = vec3_add(camera.pos, camera.direction);
    mat4_t view_matrix = mat4_look_at(camera.pos, target, (vec3_t) { 0, 1, 0 });

    // Create scale/rotation/translation matrices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    size_t num_faces = (size_t)array_length(mesh.faces);
    for (size_t i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        // Triangle face vertices
        vec3_t face_vertices[NUM_TRIANGLE_VERTICES] = {
            mesh.vertices[mesh_face.a],
            mesh.vertices[mesh_face.b],
            mesh.vertices[mesh_face.c],
        };

        vec4_t transformed_vertices[NUM_TRIANGLE_VERTICES];

        for (int j = 0; j < NUM_TRIANGLE_VERTICES; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a world matrix with scale/rotation/translation matrices
            mat4_t world_matrix = mat4_identity();

            // Scale -> rotate -> translate
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Transform scene to camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        // Check which faces need to be culled
        vec3_t vec_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
        vec3_t vec_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
        vec3_t vec_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

        // Do vec subtraction of B-A and C-A
        vec3_t vec_ab = vec3_sub(vec_b, vec_a);
        vec3_t vec_ac = vec3_sub(vec_c, vec_a);

        vec3_normalise(&vec_ab);
        vec3_normalise(&vec_ac);

        // Find the vector between vertex A in the triangle and the camera origin
        vec3_t normal = vec3_cross(vec_ab, vec_ac);

        // Normalise the face normal vector i.e. turn it into a unit vector
        vec3_normalise(&normal);

        // Find vector between the triangle and the camera
        vec3_t origin = { 0, 0, 0 };
        vec3_t camera_ray = vec3_sub(origin, vec_a);

        // Calculate the dot product between the camera and triangle normal
        float dot_normal_camera = vec3_dot(normal, camera_ray);

        if (cull_method == CULL_BACKFACE) {
            // Cull if face is facing away from camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Create polygon from original triangle to be clipped
        polygon_t polygon = poly_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2])
        );

        clip_polygon(&polygon);

        // Break clipped polygon back into triangles
        triangle_t triangles[MAX_TRIANGLES_PER_POLY] = { 0 };
        size_t num_triangles = triangles_from_poly(&polygon, triangles);

        for (size_t t = 0; t < num_triangles; t++) {
            triangle_t triangle = triangles[t];

            // Project into screen space
            vec4_t projected_points[NUM_TRIANGLE_VERTICES];

            // Loop all three vertices to perform projection
            for (int j = 0; j < NUM_TRIANGLE_VERTICES; j++) {
                // Project current point to a 2D vector to draw
                projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangle.points[j]);

                // Scale into the viewport
                projected_points[j].x *= win_width / 2.0;
                projected_points[j].y *= win_height / 2.0;

                // Invert y values to account for invert y growth between model and screen draw
                projected_points[j].y *= -1;

                // Translate projected point to centre of screen
                projected_points[j].x += win_width / 2.0;
                projected_points[j].y += win_height / 2.0;
            }

            if (lighting) {
                // Calculate light intensity based on face normal relation to light direction
                float light_intensity_factor = -vec3_dot(normal, light.direction);
                mesh_face.colour = light_apply_intensity(mesh_face.colour, light_intensity_factor);
            }

            triangle_t triangle_to_render = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
                },
                .texcoords = {
                    { mesh_face.a_uv.u, mesh_face.a_uv.v },
                    { mesh_face.b_uv.u, mesh_face.b_uv.v },
                    { mesh_face.c_uv.u, mesh_face.c_uv.v },
                },
                .colour = mesh_face.colour
            };

            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[num_triangles_to_render++] = triangle_to_render;
            }
        }
    }
}

void render(void)
{
    draw_grid();

    for (size_t i = 0; i < (size_t)num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_fill_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.colour
            );
        }

        if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE) {
            // TODO: way too many args - fix 🤮
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture
            );
        }

        if (
            render_method == RENDER_WIRE
            || render_method == RENDER_FILL_TRIANGLE_WIRE
            || render_method == RENDER_WIRE_VERTEX
            || render_method == RENDER_TEXTURED_WIRE
        ) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF666666
            );
        }

        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF0000FF);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF0000FF);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF0000FF);
        }
    }

    render_colour_buf();
    clear_colour_buf(0xFF000000);
    clear_z_buf();

    draw_ui(renderer);

    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    free(colour_buf);
    free(z_buf);
    upng_free(png_texture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}
