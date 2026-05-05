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

void renderer_draw_model(const renderer_t *renderer, const model_t *model, const shader_t *shader)
{
    shader_bind(shader);

    // Calcul de ViewProj (peut etre changer pour le faire 1 seule fois par frame)
    mat4 view_proj;
    if (renderer->data.view_mat && renderer->data.proj_mat) {
        glm_mat4_mul(*(renderer->data.proj_mat), *(renderer->data.view_mat), view_proj);
        shader_set_uniform(shader, "u_view_proj", view_proj);
    }

    // On boucle sur chaque sous-partie (submesh) de l'arbre
    for (size_t i = 0; i < model->submesh_count; i++) {
        submesh_t *submesh = &model->submeshes[i];

        vertex_array_bind(&submesh->vao);
        index_buffer_bind(&submesh->ibo);

        // --- ENVOI DU MATERIAU DE CE SUBMESH ---
        if (submesh->material_index >= 0 && submesh->material_index < (int)model->material_count) {
            material_t *mat = &model->materials[submesh->material_index];

            shader_set_uniform_vec3(shader, "u_material.ambient", mat->ambient);
            shader_set_uniform_vec3(shader, "u_material.diffuse", mat->diffuse);
            shader_set_uniform_vec3(shader, "u_material.specular", mat->specular);
            shader_set_uniform(shader, "u_material.shininess", mat->shininess);

            if (mat->diffuse_map) {
                uint32_t slot = 0;
                texture_bind(mat->diffuse_map, &slot);
                shader_set_uniform(shader, "u_material.diffuse_map", 0);
                shader_set_uniform(shader, "u_material.has_diffuse_map", 1);
            } else {
                shader_set_uniform(shader, "u_material.has_diffuse_map", 0);
            }
        }

        // On dessine le submesh
        ASSERT_GL_CALL(glDrawElements(GL_TRIANGLES, submesh->ibo.count, GL_UNSIGNED_INT, NULL));
    }
}

void renderer_draw(const renderer_t *renderer, const vertex_array_t *vao, 
                   const index_buffer_t *ibo, const shader_t *shader, 
                   const material_t *material)
{
    shader_bind(shader);
    vertex_array_bind(vao);
    index_buffer_bind(ibo);

    //calculer la matrice de vue projection
    mat4 view_proj;
    if (renderer->data.view_mat && renderer->data.proj_mat) {
        glm_mat4_mul(*(renderer->data.proj_mat), *(renderer->data.view_mat), view_proj);
        shader_set_uniform(shader, "u_view_proj", view_proj);
    }

    // --- ENVOI DU MATERIAU ---
    if (material) {
        // Envoi des couleurs et de la brillance
        shader_set_uniform_vec3(shader, "u_material.ambient", (float*)material->ambient);
        shader_set_uniform_vec3(shader, "u_material.diffuse", (float*)material->diffuse);
        shader_set_uniform_vec3(shader, "u_material.specular", (float*)material->specular);
        shader_set_uniform_1f(shader, "u_material.shininess", material->shininess);

        // Gestion de la texture optionnelle
        if (material->diffuse_map) {
            uint32_t slot = 0;
            texture_bind(material->diffuse_map, &slot);
            shader_set_uniform_1i(shader, "u_material.diffuse_map", 0);
            shader_set_uniform_1i(shader, "u_material.has_diffuse_map", 1);
        } else {
            shader_set_uniform_1i(shader, "u_material.has_diffuse_map", 0);
        }
    }

    ASSERT_GL_CALL(glDrawElements(GL_TRIANGLES, ibo->count, GL_UNSIGNED_INT, NULL));
}

