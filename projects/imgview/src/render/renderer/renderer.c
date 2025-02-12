#include "renderer.h"
#include "../../log/log.h"

#include <GL/glew.h>

void renderer_draw(__attribute__((unused)) const renderer_t *renderer, const vertex_array_t *vao, 
                   const index_buffer_t *ibo, const shader_t *shader)
{
    shader_bind(shader);
    vertex_array_bind(vao);
    index_buffer_bind(ibo);

    ASSERT_GL_CALL(glDrawElements(GL_TRIANGLES, ibo->count, GL_UNSIGNED_INT, NULL));
}

