#pragma once

#include "../vertex_buffer/vertex_buffer.h"
#include "../vertex_buffer_layout/vertex_buffer_layout.h"

typedef struct _vertex_array_t{
    unsigned int renderer_id;
} vertex_array_t;

bool vertex_array_init(vertex_array_t *va);
void vertex_array_destroy(vertex_array_t *va);

void vertex_array_bind(const vertex_array_t *va);
void vertex_array_unbind(const vertex_array_t *va);

void vertex_array_add_buffer(vertex_array_t* va, const vertex_buffer_t *vb, const vertex_buffer_layout_t* layout);

