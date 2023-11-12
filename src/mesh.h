#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define NUM_MESH_VERTICES 8
extern vec3_t mesh_vertices[NUM_MESH_VERTICES];

#define NUM_MESH_FACES (6 * 2)
extern face_t mesh_faces[NUM_MESH_FACES];

#endif // MESH_H
