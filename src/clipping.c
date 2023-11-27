#include "clipping.h"
#include "vector.h"

#include <math.h>

#define NUM_FRUSTUM_PLANES 6

plane_t frustum_planes[NUM_FRUSTUM_PLANES];

void init_frustum_planes(const float fov, const float znear, const float zfar)
{
    const float cos_half_fov = cos(fov / 2);
    const float sin_half_fov = sin(fov / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[LEFT_FRUSTUM_PLANE].normal = (vec3_t) { cos_half_fov, 0, sin_half_fov };

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal = (vec3_t) { -cos_half_fov, 0, sin_half_fov };

    frustum_planes[TOP_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[TOP_FRUSTUM_PLANE].normal = (vec3_t) { 0, -cos_half_fov, sin_half_fov };

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = (vec3_t) { 0 };
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal = (vec3_t) { 0, cos_half_fov, sin_half_fov };

    frustum_planes[NEAR_FRUSTUM_PLANE].point = (vec3_t) { 0, 0, znear };
    frustum_planes[NEAR_FRUSTUM_PLANE].normal = (vec3_t) { 0, 0, 1 };

    frustum_planes[FAR_FRUSTUM_PLANE].point = (vec3_t) { 0, 0, zfar };
    frustum_planes[FAR_FRUSTUM_PLANE].normal = (vec3_t) { 0, 0, -1 };
}
