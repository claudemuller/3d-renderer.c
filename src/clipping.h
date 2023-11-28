#ifndef CLIPPING_H_
#define CLIPPING_H_

#include "vector.h"

enum {
	LEFT_FRUSTUM_PLANE,
	RIGHT_FRUSTUM_PLANE,
	TOP_FRUSTUM_PLANE,
	BOTTOM_FRUSTUM_PLANE,
	NEAR_FRUSTUM_PLANE,
	FAR_FRUSTUM_PLANE,
};

typedef struct {
	vec3_t point;
	vec3_t normal;
} plane_t;

#define MAX_VERTICES_PER_POLY 10

typedef struct {
	vec3_t vertices[MAX_VERTICES_PER_POLY];
	int num_vertices;
} polygon_t;

void init_frustum_planes(const float fov, const float znear, const float zfar);
polygon_t create_poly_from_triangle(const vec3_t v0, const vec3_t v1, const vec3_t v2);
void clip_polygon_against_plane(polygon_t *polygon, const int plane);
void clip_polygon(polygon_t *polygon);

#endif // CLIPPING_H_
