#pragma once

#include "../../vendor/cglm/types.h"
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

void shader_set_uniform_1i(const shader_t *shader, const char* name, int v0);
void shader_set_uniform_1f(const shader_t *shader, const char* name, float v0);
void shader_set_uniform_vec3(const shader_t *shader, const char* name, vec3 v);
void shader_set_uniform_vec4(const shader_t *shader, const char* name, vec4 v);
void shader_set_uniform_mat4(const shader_t *shader, const char* name, mat4 v0);

#define shader_set_uniform(shader, name, v) _Generic((v), \
    int: shader_set_uniform_1i, \
    float: shader_set_uniform_1f, \
    double: shader_set_uniform_1f, \
    float*: shader_set_uniform_vec4, /* vec4 decay en float* */ \
    vec4*: shader_set_uniform_mat4   /* mat4 decay en vec4* */ \
)(shader, name, v)
