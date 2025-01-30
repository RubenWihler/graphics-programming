#include <GL/glew.h>
#include <string.h>

#include "index_buffer.h"
#include "../log/log.h"

bool index_buffer_init(index_buffer_t *buffer, const unsigned int* data, const unsigned int count)
{
    assert(buffer);
    memset(buffer, 0, sizeof(*buffer));

    buffer->count = count;

    ASSERT_GL_BEGIN();

    glGenBuffers(1, &buffer->renderer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->renderer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);

    if (ASSERT_GL_ERROR_OCCURED("error while creating index buffer"))
    {
        index_buffer_destroy(buffer);
        return false;
    }

    return true;
}

void index_buffer_destroy(index_buffer_t *buffer)
{
    ASSERT_GL_CALL(glDeleteBuffers(1, &buffer->renderer_id));
    buffer->renderer_id = 0;
}

void index_buffer_bind(index_buffer_t *buffer)
{
    ASSERT_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->renderer_id));
}

void index_buffer_unbind(index_buffer_t *buffer)
{
    assert(buffer);

    unsigned int bound_buffer;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (int*)&bound_buffer);

    if(bound_buffer == buffer->renderer_id)
        ASSERT_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
