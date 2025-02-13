#pragma once

#include <stdbool.h>

typedef struct _texture_t{
    unsigned int renderer_id;
    char *filepath;
    unsigned char *local_buffer;
    
    int width;
    int height;
    int bpp;//bytes par pixels
} texture_t;


bool texture_init(texture_t *tex, const char *filepath);
void texture_destroy(texture_t *tex);

void texture_bind(texture_t *tex, unsigned int* slot_ptr);
void texture_unbind();
