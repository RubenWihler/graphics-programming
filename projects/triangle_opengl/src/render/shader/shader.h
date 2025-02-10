#pragma once

#include "../../utils/hashmap/hashmap.h"
#include <stdbool.h>

typedef struct _shader_t{
    unsigned int renderer_id;
    char* dirpath;
    hashmap_t* uniforms;//caching des locations des uniforms
} shader_t;

bool shader_init(shader_t *shader, const char* dirpath);
void shader_destroy(shader_t *shader);

void shader_bind(const shader_t *shader);
void shader_unbind(const shader_t *shader);

void shader_set_uniform_1f(const shader_t *shader, const char* name, float v0);
void shader_set_uniform_4f(const shader_t *shader, const char* name, 
                           float v0, float v1, float v2, float v3);

