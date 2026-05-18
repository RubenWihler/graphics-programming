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

void renderer_begin_scene(renderer_t *renderer, const mat4* view_mat, const mat4* proj_mat)
{
    renderer->data.proj_mat = (mat4*)proj_mat;
    renderer->data.view_mat = (mat4*)view_mat;
}

void renderer_end_scene(__attribute__((unused))const renderer_t *renderer)
{
}

void renderer_draw_model(const renderer_t *renderer, const model_t *model, const shader_t *shader, const material_t* material_override)
{
    // On boucle sur chaque sous-partie (submesh) de l'arbre
    for (size_t i = 0; i < model->submesh_count; i++) {
        submesh_t *submesh = &model->submeshes[i];
        const material_t *mat = material_override;

        if (!mat && submesh->material_index >= 0 && submesh->material_index < (int)model->material_count) {
            mat = &model->materials[submesh->material_index];
        }

        // On dessine le submesh
        renderer_draw(
            renderer,
            &submesh->vao,
            &submesh->ibo,
            shader,
            mat
        );
    }
}

void renderer_draw_mesh(const renderer_t *renderer, const submesh_t* mesh, const shader_t *shader, const material_t *material)
{
    renderer_draw(renderer, &mesh->vao, &mesh->ibo, shader, material);
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
        // Envoi des variables PBR
        shader_set_uniform_vec3(shader, "u_albedo", material->albedo);
        shader_set_uniform_1f(shader, "u_metallic", material->metallic);
        shader_set_uniform_1f(shader, "u_roughness", material->roughness);
        shader_set_uniform_1f(shader, "u_ao", material->ao);

        // 1. Albedo (Slot 0)
        if (material->albedo_map) {
            uint32_t slot = 0; texture_bind(material->albedo_map, &slot);
            shader_set_uniform_1i(shader, "u_material.albedo_map", 0);
            shader_set_uniform_1i(shader, "u_material.has_albedo_map", 1);
        } else shader_set_uniform_1i(shader, "u_material.has_albedo_map", 0);

        // 2. Normal (Slot 1)
        if (material->normal_map) {
            uint32_t slot = 1; texture_bind(material->normal_map, &slot);
            shader_set_uniform_1i(shader, "u_material.normal_map", 1);
            shader_set_uniform_1i(shader, "u_material.has_normal_map", 1);
        } else shader_set_uniform_1i(shader, "u_material.has_normal_map", 0);

        // 3. Metallic (Slot 2)
        if (material->metallic_map) {
            uint32_t slot = 2; texture_bind(material->metallic_map, &slot);
            shader_set_uniform_1i(shader, "u_material.metallic_map", 2);
            shader_set_uniform_1i(shader, "u_material.has_metallic_map", 1);
        } else shader_set_uniform_1i(shader, "u_material.has_metallic_map", 0);

        // 4. Roughness (Slot 3)
        if (material->roughness_map) {
            uint32_t slot = 3; texture_bind(material->roughness_map, &slot);
            shader_set_uniform_1i(shader, "u_material.roughness_map", 3);
            shader_set_uniform_1i(shader, "u_material.has_roughness_map", 1);
        } else shader_set_uniform_1i(shader, "u_material.has_roughness_map", 0);

        // 5. AO (Slot 4)
        if (material->ao_map) {
            uint32_t slot = 4; texture_bind(material->ao_map, &slot);
            shader_set_uniform_1i(shader, "u_material.ao_map", 4);
            shader_set_uniform_1i(shader, "u_material.has_ao_map", 1);
        } else shader_set_uniform_1i(shader, "u_material.has_ao_map", 0);

        // 6. Emission (Slot 5)
        if (material->emission_map) {
            uint32_t slot = 5; texture_bind(material->emission_map, &slot);
            shader_set_uniform_1i(shader, "u_material.emission_map", 5);
            shader_set_uniform_1i(shader, "u_material.has_emission_map", 1);
        } else {
            shader_set_uniform_1i(shader, "u_material.has_emission_map", 0);
        }

        //les slots suivants: 6,7,8 sont utiliser dans s_render.c pour l'IBL:
        //   6 : l'irradience map
        //   7 : prefilter map
        //   8 : brdf lut

        // Gestion de la texture diffuse si on utilise phong
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

