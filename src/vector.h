#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4_t;

float vec2_length(const vec2_t v);
vec2_t vec2_add(const vec2_t v1, const vec2_t v2);
vec2_t vec2_sub(const vec2_t v1, const vec2_t v2);
vec2_t vec2_mul(const vec2_t v, const float f);
vec2_t vec2_div(const vec2_t v, const float f);
float vec2_dot(const vec2_t v1, const vec2_t v2);
void vec2_normalise(vec2_t *v);

float vec3_length(const vec3_t v);
vec3_t vec3_add(const vec3_t v1, const vec3_t v2);
vec3_t vec3_sub(const vec3_t v1, const vec3_t v2);
vec3_t vec3_mul(const vec3_t v, const float f);
vec3_t vec3_div(const vec3_t v, const float f);
vec3_t vec3_cross(const vec3_t v1, const vec3_t v2);
float vec3_dot(const vec3_t v1, const vec3_t v2);
void vec3_normalise(vec3_t *v);
vec3_t vec3_rotate_x(const vec3_t v, const float angle);
vec3_t vec3_rotate_y(const vec3_t v, const float angle);
vec3_t vec3_rotate_z(const vec3_t v, const float angle);

vec4_t vec4_from_vec3(const vec3_t v);
vec2_t vec2_from_vec4(const vec4_t v);
vec3_t vec3_from_vec4(const vec4_t v);

#endif // VECTOR_H
