#include <string.h>
#include <assert.h>

#include "../../vendor/glad/glad.h"
#include "vertex_buffer.h"
#include "../../log/log.h"

bool vertex_buffer_init(vertex_buffer_t *vb, const void* data, const unsigned int size, bool dynamic)
{
    assert(vb);
    memset(vb, 0, sizeof(*vb));
    
    ASSERT_GL_BEGIN();

    glGenBuffers(1, &vb->renderer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vb->renderer_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    if (ASSERT_GL_ERROR_OCCURED("error while creating vertex buffer"))
    {
        vertex_buffer_destroy(vb);
        return false;
    }

    return true;
}

void vertex_buffer_destroy(vertex_buffer_t *vb)
{
    ASSERT_GL_CALL(glDeleteBuffers(1, &vb->renderer_id));
    vb->renderer_id = 0;
}

void vertex_buffer_bind(const vertex_buffer_t *vb)
{
    ASSERT_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vb->renderer_id));
}

void vertex_buffer_unbind(const vertex_buffer_t *vb)
{   
    assert(vb);

    unsigned int bound_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (int*)&bound_buffer);

    if(bound_buffer == vb->renderer_id)
        ASSERT_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void vertex_buffer_set_data(vertex_buffer_t *vb, const void* data, const unsigned int size)
{
    vertex_buffer_bind(vb);
    ASSERT_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}
