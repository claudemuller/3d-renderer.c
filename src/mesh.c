#include "array.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 },
};

vec3_t cube_vertices[NUM_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y = 1, .z = -1 }, // 2
    { .x = 1, .y = 1, .z = -1 }, // 3
    { .x = 1, .y = -1, .z = -1 }, // 4
    { .x = 1, .y = 1, .z = 1 }, // 5
    { .x = 1, .y = -1, .z = 1 }, // 6
    { .x = -1, .y = 1, .z = 1 }, // 7
    { .x = -1, .y = -1, .z = 1 }, // 8
};

face_t cube_faces[NUM_CUBE_FACES] = {
    // Cube front face of two triangle faces
    { .a = 1, .b = 2, .c = 3, .colour = 0xFFFF0000 },
    { .a = 1, .b = 3, .c = 4, .colour = 0xFFFF0000 },
    // Cube right face of two triangle faces
    { .a = 4, .b = 3, .c = 5, .colour = 0xFF00FF00 },
    { .a = 4, .b = 5, .c = 6, .colour = 0xFF00FF00 },
    // Cube back face of two triangle faces
    { .a = 6, .b = 5, .c = 7, .colour = 0xFF0000FF },
    { .a = 6, .b = 7, .c = 8, .colour = 0xFF0000FF },
    // Cube left face of two triangle faces
    { .a = 8, .b = 7, .c = 2, .colour = 0xFFFFFF00 },
    { .a = 8, .b = 2, .c = 1, .colour = 0xFFFFFF00 },
    // Cube top face of two triangle faces
    { .a = 2, .b = 7, .c = 5, .colour = 0xFFFF00FF },
    { .a = 2, .b = 5, .c = 3, .colour = 0xFFFF00FF },
    // Cube bottom face of two triangle faces
    { .a = 6, .b = 8, .c = 1, .colour = 0xFF00FFFF },
    { .a = 6, .b = 1, .c = 4, .colour = 0xFF00FFFF },
};

void load_cube_mesh_data(void)
{
    for (size_t i = 0; i < NUM_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for (size_t i = 0; i < NUM_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

bool load_obj(const char *filename)
{
    // Open file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "error opening .obj file\n");
        return false;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d ",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            );
            face_t face = {
                .a = vertex_indices[0],
                .b = vertex_indices[1],
                .c = vertex_indices[2],
            };
            array_push(mesh.faces, face);
        }
    }

    fclose(fp);
    free(line);

    return true;
}
