#ifndef TEXTURE_H_
#define TEXTURE_H_

typedef struct {
    float u;
    float v;
} tex2_t;

tex2_t tex2_clone(tex2_t *tex);

#endif // TEXTURE_H_
