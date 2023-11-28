#include "camera.h"
#include "matrix.h"
#include "vector.h"

static camera_t camera;

void init_camera(const vec3_t pos, const vec3_t direction)
{
    camera.pos = pos;
    camera.direction = direction;
    camera.forward_vel = (vec3_t) { 0 };
    camera.yaw = 0.0;
    camera.pitch = 0.0;
}

vec3_t camera_get_pos(void)
{
    return camera.pos;
}

vec3_t camera_get_forward_vel(void)
{
    return camera.forward_vel;
}

vec3_t camera_get_direction(void)
{
    return camera.direction;
}

float camera_get_yaw(void)
{
    return camera.yaw;
}

float camera_get_pitch(void)
{
    return camera.pitch;
}

vec3_t get_camera_lookat_target(void)
{
    vec3_t target = { 0, 0, 1 };

    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);

    mat4_t camera_rotation = mat4_identity();
    camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

    vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
    camera.direction = vec3_from_vec4(camera_direction);

    return vec3_add(camera.pos, camera.direction);
}

void camera_set_forward_vel(const vec3_t vel)
{
    camera.forward_vel = vel;
}

void camera_set_pos(const vec3_t pos)
{
    camera.pos = pos;
}

void camera_set_direction(const vec3_t dir)
{
    camera.direction = dir;
}

void camera_rotate_yaw(const float angle)
{
    camera.yaw += angle;
}

void camera_rotate_pitch(const float angle)
{
    camera.pitch += angle;
}
