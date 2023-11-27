#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector.h"

typedef struct {
	vec3_t position;
	vec3_t direction;
} camera_t;

extern camera_t camera;

#endif // CAMERA_H_
