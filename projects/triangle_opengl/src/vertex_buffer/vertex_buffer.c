#include <GL/glew.h>
#include <string.h>
#include <assert.h>

#include "vertex_buffer.h"
#include "../log/log.h"

bool vertex_buffer_init(vertex_buffer_t *buffer, const void* data, const unsigned int size)
{
    assert(buffer);
    memset(buffer, 0, sizeof(*buffer));
    
    ASSERT_GL_BEGIN();

    glGenBuffers(1, &buffer->renderer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->renderer_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    if (ASSERT_GL_ERROR_OCCURED("error while creating vertex buffer"))
    {
        vertex_buffer_destroy(buffer);
        return false;
    }

    return true;
}

void vertex_buffer_destroy(vertex_buffer_t *buffer)
{
    ASSERT_GL_CALL(glDeleteBuffers(1, &buffer->renderer_id));
    buffer->renderer_id = 0;
}

void vertex_buffer_bind(vertex_buffer_t *buffer)
{
    ASSERT_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer->renderer_id));
}

void vertex_buffer_unbind(vertex_buffer_t *buffer)
{   
    assert(buffer);

    unsigned int bound_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (int*)&bound_buffer);

    if(bound_buffer == buffer->renderer_id)
        ASSERT_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
