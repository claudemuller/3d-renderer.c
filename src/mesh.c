#include "array.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_mesh(
    const char *obj_filename,
    const char *png_filename,
    const vec3_t scale,
    const vec3_t translation,
    const vec3_t rotation
)
{
    if (!load_mesh_obj_data(&meshes[mesh_count], obj_filename)) {
        // tODO: handle error
    }

    if (!load_mesh_png_data(&meshes[mesh_count], png_filename)) {
        // tODO: handle error
    }

    meshes[mesh_count].scale = scale;
    meshes[mesh_count].translation = translation;
    meshes[mesh_count].rotation = rotation;

    mesh_count++;
}

bool load_mesh_png_data(mesh_t *mesh, const char *filename)
{
    upng_t *png_image = upng_new_from_file(filename);
    if (!png_image) {
        fprintf(stderr, "error loading .png\n");
        return false;
    }

    upng_decode(png_image);
    if (upng_get_error(png_image) != UPNG_EOK) {
        fprintf(stderr, "error decoding .png\n");
        return false;
    }

    mesh->texture = png_image;

    return true;
}

bool load_mesh_obj_data(mesh_t *mesh, const char *filename)
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
    tex2_t *texcoords = NULL;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh->vertices, vertex);
        }

        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
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
                .a = vertex_indices[0] - 1,
                .b = vertex_indices[1] - 1,
                .c = vertex_indices[2] - 1,
                .a_uv = texcoords[texture_indices[0] - 1],
                .b_uv = texcoords[texture_indices[1] - 1],
                .c_uv = texcoords[texture_indices[2] - 1],
                .colour = 0xFFFFFFFF,
            };
            array_push(mesh->faces, face);
        }
    }

    array_free(texcoords);
    fclose(fp);
    free(line);

    return true;
}

mesh_t *get_mesh(const int idx)
{
    return &meshes[idx];
}

int get_num_meshes(void)
{
    return mesh_count;
}

void free_meshes(void)
{
    for (int i = 0; i < mesh_count; i++) {
        upng_free(meshes[i].texture);
        array_free(meshes[i].faces);
        array_free(meshes[i].vertices);
    }
}
