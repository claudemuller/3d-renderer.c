#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <stdint.h>

typedef struct {
    float u;
    float v;
} tex2_t;

extern int texture_width;
extern int texture_height;
extern uint32_t* mesh_texture;
extern const uint8_t REDBRICK_TEXTURE[];

#endif // TEXTURE_H_
