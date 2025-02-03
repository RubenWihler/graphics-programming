#include "vertex_buffer_layout.h"

#include <GL/glew.h>
#include <string.h>
#include <assert.h>

#include "../../utils/gl_utils/gl_utils.h"

static void push(vertex_buffer_layout_t *layout, const unsigned int type, 
                 const unsigned int count, const unsigned char normalized);

bool vertex_buffer_layout_init(vertex_buffer_layout_t *layout)
{
    memset(layout, 0, sizeof(*layout));

    //on crée un tableau dynamique pour les éléments
    layout->elements = dyn_array_create(1, sizeof(vertex_buffer_element_t), NULL);
    if(!layout->elements) return false;

    layout->stride = 0;

    return true;
}

void vertex_buffer_layout_destroy(vertex_buffer_layout_t *layout)
{
    dyn_array_destroy(layout->elements);
    memset(layout, 0, sizeof(*layout));
}

void vertex_buffer_layout_push_float(vertex_buffer_layout_t *layout, const unsigned int count)
{
    push(layout, GL_FLOAT, count, GL_FALSE);
}

void vertex_buffer_layout_push_uint(vertex_buffer_layout_t *layout, const unsigned int count)
{
    push(layout, GL_UNSIGNED_INT, count, GL_FALSE);
}

void vertex_buffer_layout_push_uchar(vertex_buffer_layout_t *layout, const unsigned int count)
{
    push(layout, GL_UNSIGNED_BYTE, count, GL_TRUE);
}

static void push(vertex_buffer_layout_t *layout, const unsigned int type, 
                 const unsigned int count, const unsigned char normalized)
{
    vertex_buffer_element_t element = {
        .type = type,
        .count = count,
        .normalized = normalized
    };

    layout->stride += count * gl_get_type_size(type);
    dyn_array_push_back(layout->elements, &element);
}
