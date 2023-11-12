#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;
} face_t;

#define NUM_TRIANGLE_VERTICES 3

typedef struct {
	vec2_t points[NUM_TRIANGLE_VERTICES];
} triangle_t;

#endif // TRIANGLE_H
