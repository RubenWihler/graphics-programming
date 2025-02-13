#include "renderer.h"

#include "../../log/log.h"

#include <string.h>
#include <GL/glew.h>

bool renderer_init(renderer_t *renderer, unsigned int id)
{
    renderer->id = id;
    renderer->data.view_proj_matrix = NULL;

    return true;
}

void renderer_destroy(renderer_t *renderer)
{
    (void)renderer;
}

void renderer_begin_scene(renderer_t *renderer, cam_ortho_t *cam)
{
    renderer->data.view_proj_matrix = &cam->view_projection_matrix;
}

void renderer_end_scene(__attribute__((unused))const renderer_t *renderer)
{
}

void renderer_draw(const renderer_t *renderer, const vertex_array_t *vao, 
                   const index_buffer_t *ibo, const shader_t *shader)
{
    shader_bind(shader);
    vertex_array_bind(vao);
    index_buffer_bind(ibo);

    shader_set_uniform_mat4(shader, "u_view_proj", *renderer->data.view_proj_matrix);
    ASSERT_GL_CALL(glDrawElements(GL_TRIANGLES, ibo->count, GL_UNSIGNED_INT, NULL));
}

