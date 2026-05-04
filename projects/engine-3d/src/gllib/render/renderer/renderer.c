#include "renderer.h"

#include "../../vendor/cglm/mat4.h"
#include "../../vendor/glad/glad.h"
#include "../../log/log.h"
#include <string.h>


bool renderer_init(renderer_t *renderer, unsigned int id)
{
    renderer->id = id;
    renderer->data.proj_mat = NULL;
    renderer->data.view_mat = NULL;

    return true;
}

void renderer_destroy(renderer_t *renderer)
{
    renderer->data.proj_mat = NULL;
    renderer->data.view_mat = NULL;
}

void renderer_begin_scene_ortho(renderer_t *renderer, const cam_ortho_t *cam)
{
    renderer->data.proj_mat = (mat4*)&cam->proj;
    renderer->data.view_mat = (mat4*)&cam->view;
}

void renderer_begin_scene_persp(renderer_t *renderer, const cam_persp_t *cam)
{
    renderer->data.proj_mat = (mat4*)&cam->proj;
    renderer->data.view_mat = (mat4*)&cam->view;
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

    mat4 view_proj;
    if (renderer->data.view_mat && renderer->data.proj_mat) {
        glm_mat4_mul(*(renderer->data.proj_mat), *(renderer->data.view_mat), view_proj);
        shader_set_uniform(shader, "u_view_proj", view_proj);
    }

    ASSERT_GL_CALL(glDrawElements(GL_TRIANGLES, ibo->count, GL_UNSIGNED_INT, NULL));
}

