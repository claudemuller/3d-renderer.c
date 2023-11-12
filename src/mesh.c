#include "mesh.h"

vec3_t mesh_vertices[NUM_MESH_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y = 1, .z = -1 }, // 2
    { .x = 1, .y = 1, .z = -1 }, // 3
    { .x = 1, .y = -1, .z = -1 }, // 4
    { .x = 1, .y = 1, .z = 1 }, // 5
    { .x = 1, .y = -1, .z = 1 }, // 6
    { .x = -1, .y = 1, .z = 1 }, // 7
    { .x = -1, .y = -1, .z = 1 }, // 8
};

#define NUM_MESH_FACES (6 * 2)
face_t mesh_faces[NUM_MESH_FACES] = {
    // Cube front face of two triangle faces
    { .a = 1, .b = 2, .c = 3 },
    { .a = 1, .b = 3, .c = 4 },
    // Cube right face of two triangle faces
    { .a = 4, .b = 3, .c = 5 },
    { .a = 4, .b = 5, .c = 6 },
    // Cube back face of two triangle faces
    { .a = 6, .b = 5, .c = 7 },
    { .a = 6, .b = 7, .c = 8 },
    // Cube left face of two triangle faces
    { .a = 8, .b = 7, .c = 2 },
    { .a = 8, .b = 2, .c = 1 },
    // Cube top face of two triangle faces
    { .a = 2, .b = 7, .c = 5 },
    { .a = 2, .b = 5, .c = 3 },
    // Cube bottom face of two triangle faces
    { .a = 6, .b = 8, .c = 1 },
    { .a = 6, .b = 1, .c = 4 },
};
