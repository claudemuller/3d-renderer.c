#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	vec3_t direction;	
} light_t;

extern light_t light;
extern bool lighting;

uint32_t light_apply_intensity(const uint32_t col, const float factor);

#endif // LIGHT_H
