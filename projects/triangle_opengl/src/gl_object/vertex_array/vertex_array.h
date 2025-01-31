#pragma once

#include "../vertex_buffer/vertex_buffer.h"
#include "../vertex_buffer_layout/vertex_buffer_layout.h"

typedef struct _vertex_array_t{
    
} vertex_array_t;

int vertex_array_init();
void vertex_array_destroy(vertex_array_t *va);

void vertex_array_add_buffer(vertex_array_t* va, const vertex_buffer_t *vb, const vertex_buffer_layout_t* layout);

