#pragma once

#include "../vertex_array/vertex_array.h"
#include "../index_buffer/index_buffer.h"
#include "../shader/shader.h"

typedef struct _renderer_t{
    unsigned int id;
} renderer_t;

void renderer_draw(const renderer_t *renderer, const vertex_array_t *vao, 
                   const index_buffer_t *ibo, const shader_t *shader);
