#include "texture.h"
#include "upng.h"
#include <stdio.h>

uint32_t *mesh_texture = NULL;
int texture_width = 64;
int texture_height = 64;
upng_t *png_texture = NULL;

void load_png_texture_data(const char *filename)
{
    png_texture = upng_new_from_file(filename);
    if (png_texture) {
        upng_decode(png_texture);
        if (upng_get_error(png_texture) != UPNG_EOK) {
            fprintf(stderr, "error loading .png\n");
        }

        mesh_texture = (uint32_t *)upng_get_buffer(png_texture);
        texture_width = upng_get_width(png_texture);
        texture_height = upng_get_height(png_texture);
    }
}
