#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector.h"

typedef struct {
	vec3_t pos;
	vec3_t direction;
	vec3_t forward_vel;
	float yaw;   // angle in radians on y axis
	float pitch; // angle in radians on x axis
} camera_t;

vec3_t camera_get_pos(void);
vec3_t camera_get_forward_vel(void);
vec3_t camera_get_direction(void);
float camera_get_yaw(void);
float camera_get_pitch(void);
vec3_t get_camera_lookat_target(void);

void camera_set_forward_vel(const vec3_t vel);
void camera_set_pos(const vec3_t pos);
void camera_set_direction(const vec3_t dir);

void camera_rotate_yaw(const float angle);
void camera_rotate_pitch(const float angle);

#endif // CAMERA_H_
