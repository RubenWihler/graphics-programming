#pragma once

#include <stdbool.h>
#include "../../utils/dynamic_array/dyn_array.h"

typedef struct _vertex_buffer_element_t{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;
} vertex_buffer_element_t;

typedef struct _vertex_buffer_layout_t{
    dyn_array_t *elements;
    unsigned int stride;
} vertex_buffer_layout_t;

bool vertex_buffer_layout_init(vertex_buffer_layout_t *layout);
void vertex_buffer_layout_destroy(vertex_buffer_layout_t *layout);

void vertex_buffer_layout_push_float(vertex_buffer_layout_t *layout, const unsigned int count);
void vertex_buffer_layout_push_uint(vertex_buffer_layout_t *layout, const unsigned int count);
void vertex_buffer_layout_push_uchar(vertex_buffer_layout_t *layout, const unsigned int count);
