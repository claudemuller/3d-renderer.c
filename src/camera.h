#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector.h"

typedef struct {
	vec3_t pos;
	vec3_t direction;
	vec3_t forward_vel;
	float yaw; // angle in radians
} camera_t;

extern camera_t camera;

#endif // CAMERA_H_
