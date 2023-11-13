#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
typedef struct {
	float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(const float sx, const float sy, const float sz);
mat4_t mat4_make_translation(const float tx, const float ty, const float tz);
mat4_t mat4_make_rotation(const float tx, const float ty, const float tz);
vec4_t mat4_mul_vec4(const mat4_t m, const vec4_t v);

#endif // MATRIX_H
