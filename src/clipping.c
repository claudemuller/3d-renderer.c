#include "clipping.h"
#include "texture.h"
#include "vector.h"

#include <math.h>
#include <stddef.h>

#define NUM_FRUSTUM_PLANES 6

plane_t frustum_planes[NUM_FRUSTUM_PLANES];

#define LERP(a, b, t) _Generic((a), int: int_lerp, float: float_lerp)(a, b, t)

static float float_lerp(const float a, const float b, const float t)
{
    return a + t * (b - a);
}

static int int_lerp(const int a, const int b, const int t)
{
    return a + t * (b - a);
}

void init_frustum_planes(const float fovx, const float fovy, const float znear, const float zfar)
{
    const float cos_half_fovx = cos(fovx / 2);
    const float sin_half_fovx = sin(fovx / 2);
    const float cos_half_fovy = cos(fovy / 2);
    const float sin_half_fovy = sin(fovy / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[LEFT_FRUSTUM_PLANE].normal = (vec3_t) { cos_half_fovx, 0, sin_half_fovx };

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal = (vec3_t) { -cos_half_fovx, 0, sin_half_fovx };

    frustum_planes[TOP_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[TOP_FRUSTUM_PLANE].normal = (vec3_t) { 0, -cos_half_fovy, sin_half_fovy };

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal = (vec3_t) { 0, cos_half_fovy, sin_half_fovy };

    frustum_planes[NEAR_FRUSTUM_PLANE].point = (vec3_t) { 0, 0, znear };
    frustum_planes[NEAR_FRUSTUM_PLANE].normal = (vec3_t) { 0, 0, 1 };

    frustum_planes[FAR_FRUSTUM_PLANE].point = (vec3_t) { 0, 0, zfar };
    frustum_planes[FAR_FRUSTUM_PLANE].normal = (vec3_t) { 0, 0, -1 };
}

polygon_t poly_from_triangle(
    const vec3_t v0, const vec3_t v1, const vec3_t v2,
    const tex2_t t0, const tex2_t t1, const tex2_t t2
)
{
    return (polygon_t) {
        .vertices = { v0, v1, v2 },
        .texcoords = { t0, t1, t2 },
        .num_vertices = 3,
    };
}

int triangles_from_poly(const polygon_t *polygon, triangle_t *triangles)
{
    const int idx0 = 0;
    int idx1 = 0;
    int idx2 = 0;
    for (int i = 0; i < polygon->num_vertices - 2; i++) {
        idx1 = i + 1;
        idx2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(polygon->vertices[idx0]);
        triangles[i].points[1] = vec4_from_vec3(polygon->vertices[idx1]);
        triangles[i].points[2] = vec4_from_vec3(polygon->vertices[idx2]);

        triangles[i].texcoords[0] = polygon->texcoords[idx0];
        triangles[i].texcoords[1] = polygon->texcoords[idx1];
        triangles[i].texcoords[2] = polygon->texcoords[idx2];
    }
    return polygon->num_vertices - 2;
}

void clip_polygon_against_plane(polygon_t *polygon, const int plane)
{
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertices[MAX_VERTICES_PER_POLY] = { 0 };
    tex2_t inside_texcoords[MAX_VERTICES_PER_POLY] = { 0 };
    size_t num_inside_vertices = 0;

    vec3_t *cur_vertex = &polygon->vertices[0];
    tex2_t *cur_texcoord = &polygon->texcoords[0];

    vec3_t *prev_vertex = &polygon->vertices[polygon->num_vertices - 1];
    tex2_t *prev_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

    float cur_dot = 0;
    float prev_dot = vec3_dot(vec3_sub(*prev_vertex, plane_point), plane_normal);

    while (cur_vertex != &polygon->vertices[polygon->num_vertices]) {
        cur_dot = vec3_dot(vec3_sub(*cur_vertex, plane_point), plane_normal);

        // Check if inside or outside of frustum (side of the plane in question)
        if (cur_dot * prev_dot < 0) {
            // Find the interpolation factor t where t is a value between the beginning (Q1) and end (Q2) of a
            // line segment and is between 0 and 1
            const float t = prev_dot / (prev_dot - cur_dot);

            // Calculate the intersection point I where I = Q1 + t(Q2 - Q1)
            vec3_t intersection_point = {
                .x = LERP(prev_vertex->x, cur_vertex->x, t),
                .y = LERP(prev_vertex->y, cur_vertex->y, t),
                .z = LERP(prev_vertex->z, cur_vertex->z, t),
            };

            // Use lerp (linear interpolation) formula to get interpolated UV texture coords
            tex2_t interpolated_texcoord = {
                .u = LERP(prev_texcoord->u, cur_texcoord->u, t),
                .v = LERP(prev_texcoord->v, cur_texcoord->v, t),
            };

            // Add intersection point to inside vertices
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            inside_texcoords[num_inside_vertices] = tex2_clone(&interpolated_texcoord);
            num_inside_vertices++;
        }

        // Current vertex is inside the frustum (side of the plane in question)
        if (cur_dot > 0) {
            inside_vertices[num_inside_vertices] = vec3_clone(cur_vertex);
            inside_texcoords[num_inside_vertices] = tex2_clone(cur_texcoord);
            num_inside_vertices++;
        }

        prev_dot = cur_dot;
        prev_vertex = cur_vertex;
        prev_texcoord = cur_texcoord;
        cur_vertex++;
        cur_texcoord++;
    }

    for (size_t i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
    }
    polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t *polygon)
{
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}
