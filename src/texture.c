#include "texture.h"

tex2_t tex2_clone(tex2_t *t)
{
    return (tex2_t) {
        .u = t->u,
        .v = t->v,
    };
}
