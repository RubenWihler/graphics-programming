#pragma once

#include <stdbool.h>
#include "../shader/shader.h"

typedef struct _texture_t{
    unsigned int renderer_id;
    char *filepath;
    unsigned char *local_buffer;

    int width;
    int height;
    int bpp;//bytes par pixels
} texture_t;

typedef struct {
    unsigned int id;
    int width;
    int height;
} cubemap_t;


bool texture_init(texture_t *tex, const char *filepath);
void texture_destroy(texture_t *tex);

void texture_bind(texture_t *tex, unsigned int* slot_ptr);
void texture_unbind();

bool texture_load_cubemap_from_hdr(cubemap_t* out_cubemap, const char* filepath, shader_t* conversion_shader);
bool texture_create_irradiance_map(cubemap_t* out_irradiance, cubemap_t* env_map, shader_t* irrad_shader);
bool texture_create_prefilter_map(cubemap_t* out_prefilter, cubemap_t* env_map, shader_t* prefilter_shader);
bool texture_create_brdf_lut(texture_t* out_texture, shader_t* brdf_shader);
