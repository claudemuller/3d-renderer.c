#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include <stdbool.h>

#define NUM_CUBE_VERTICES 8
#define NUM_CUBE_FACES (6 * 2)

extern face_t cube_faces[NUM_CUBE_FACES];
extern vec3_t cube_vertices[NUM_CUBE_VERTICES];

typedef struct {
  vec3_t *vertices;
  face_t *faces;
  vec3_t rotation;
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
bool load_obj(const char *filename);

#endif // MESH_H
