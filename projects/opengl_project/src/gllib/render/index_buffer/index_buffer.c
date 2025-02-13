#include <GL/glew.h>
#include <string.h>

#include "index_buffer.h"
#include "../../log/log.h"

bool index_buffer_init(index_buffer_t *ib, const unsigned int* data, const unsigned int count, bool dynamic)
{
    assert(ib);
    memset(ib, 0, sizeof(*ib));

    ib->count = count;

    ASSERT_GL_BEGIN();

    glGenBuffers(1, &ib->renderer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->renderer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    if (ASSERT_GL_ERROR_OCCURED("error while creating index buffer"))
    {
        index_buffer_destroy(ib);
        return false;
    }

    return true;
}

void index_buffer_destroy(index_buffer_t *ib)
{
    ASSERT_GL_CALL(glDeleteBuffers(1, &ib->renderer_id));
    ib->renderer_id = 0;
}

void index_buffer_bind(const index_buffer_t *ib)
{
    ASSERT_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->renderer_id));
}

void index_buffer_unbind(const index_buffer_t *ib)
{
    assert(ib);

    unsigned int bound_buffer;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (int*)&bound_buffer);

    if(bound_buffer == ib->renderer_id)
        ASSERT_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void index_buffer_set_data(index_buffer_t *ib, const unsigned int* data, const unsigned int count)
{
    index_buffer_bind(ib);
    ASSERT_GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(unsigned int), data));
}
