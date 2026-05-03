#include "renderer.h"

#include "../../log/log.h"

#include <string.h>
#include "../../vendor/glad/glad.h"

bool renderer_init(renderer_t *renderer, unsigned int id)
{
    renderer->id = id;
    renderer->data.view_proj_matrix = NULL;

    return true;
}

void renderer_destroy(renderer_t *renderer)
{
    renderer->data.view_proj_matrix = NULL;
}

void renderer_begin_scene_ortho(renderer_t *renderer, const cam_ortho_t *cam)
{
    renderer->data.view_proj_matrix = (mat4*)&cam->view_proj;
}

void renderer_begin_scene_persp(renderer_t *renderer, const cam_persp_t *cam)
{
    renderer->data.view_proj_matrix = (mat4*)&cam->view_proj;
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

    shader_set_uniform(shader, "u_view_proj", *renderer->data.view_proj_matrix);
    ASSERT_GL_CALL(glDrawElements(GL_TRIANGLES, ibo->count, GL_UNSIGNED_INT, NULL));
}

