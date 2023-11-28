#ifndef CLIPPING_H_
#define CLIPPING_H_

#include "texture.h"
#include "triangle.h"
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
#define MAX_TRIANGLES_PER_POLY 10

typedef struct {
	vec3_t vertices[MAX_VERTICES_PER_POLY];
	tex2_t texcoords[MAX_VERTICES_PER_POLY];
	int num_vertices;
} polygon_t;

void init_frustum_planes(const float fovx, const float fovy, const float znear, const float zfar);
polygon_t poly_from_triangle(
    const vec3_t v0, const vec3_t v1, const vec3_t v2,
    const tex2_t t0, const tex2_t t1, const tex2_t t2
);
int triangles_from_poly(const polygon_t *polygon, triangle_t *triangles);
void clip_polygon_against_plane(polygon_t *polygon, const int plane);
void clip_polygon(polygon_t *polygon);

#endif // CLIPPING_H_
