#pragma once

typedef unsigned long size_t;

#include "particle_props.h"
#include "particle.h"

#include "../../core/utils/dynamic_array/dyn_array.h"

#include "../../core/render/renderer/renderer.h"
#include "../../core/render/camera/cam_ortho.h"
#include "../../core/render/vertex_array/vertex_array.h"
#include "../../core/render/vertex_buffer/vertex_buffer.h"
#include "../../core/render/index_buffer/index_buffer.h"
#include "../../core/render/shader/shader.h"

typedef struct {
    particle_props_t props;

    size_t capacity;
    size_t index;
    
    particle_t *particles;

    //render
    vertex_array_t va;
    vertex_buffer_t vb;
    vertex_buffer_layout_t layout;
    index_buffer_t ib;

    float *vertices;
    unsigned int *indices;
} particle_pool_t;

bool particle_pool_init(particle_pool_t *pool, const particle_props_t props, const size_t capacity);
void particle_pool_destroy(particle_pool_t *pool);

void particle_pool_update(particle_pool_t *pool, const float delta_time);
void particle_pool_render(particle_pool_t *pool, const shader_t *shader, const renderer_t *renderer);

void particle_pool_emit(particle_pool_t *pool, const vec2 pos);
