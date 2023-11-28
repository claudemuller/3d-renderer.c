#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"
#include <stdbool.h>

typedef struct {
  vec3_t *vertices;
  face_t *faces;
  upng_t *texture;
  vec3_t rotation;
  vec3_t scale;
  vec3_t translation;
} mesh_t;

bool load_mesh_obj_data(mesh_t *mesh, const char *filename);
bool load_mesh_png_data(mesh_t *mesh, const char *filename);
void load_mesh(
  const char *obj_filename,
  const char *png_filename,
  const vec3_t scale,
  const vec3_t translation,
  const vec3_t rotation
);
int get_num_meshes(void);
mesh_t *get_mesh(const int idx);
void free_meshes(void);

#endif // MESH_H
