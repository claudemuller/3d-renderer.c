#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <stdint.h>
#include "upng.h"

typedef struct {
    float u;
    float v;
} tex2_t;

extern int texture_width;
extern int texture_height;
extern uint32_t* mesh_texture;
extern upng_t *png_texture;

tex2_t tex2_clone(tex2_t *tex);
void load_png_texture_data(const char *filename);

#endif // TEXTURE_H_
