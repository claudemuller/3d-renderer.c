#ifndef LIGHT_H_
#define LIGHT_H_

#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	vec3_t direction;	
} light_t;

void init_light(const vec3_t direction);
vec3_t get_light_direction(void);
uint32_t light_apply_intensity(const uint32_t col, const float factor);

#endif // LIGHT_H_
