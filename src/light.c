#include "light.h"
#include <math.h>

bool lighting = false;

light_t light = {
    .direction = { 0, 0, 1 }
};

uint32_t light_apply_intensity(const uint32_t col, float factor)
{
    if (factor < 0) {
        factor = 0;
    }
    if (factor > 1) {
        factor = 1;
    }

    uint32_t a = (col & 0xFF000000);
    uint32_t r = (col & 0x00FF0000) * factor;
    uint32_t g = (col & 0x0000FF00) * factor;
    uint32_t b = (col & 0x000000FF) * factor;
    uint32_t adjusted_col = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return adjusted_col;
}
