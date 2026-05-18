#include "s_clouds.h"
#include "../../core/vendor/glad/glad.h"

void s_clouds_render(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader, camera_component_t* cam, vec3 sun_dir) {
    if (!cam) return;

    signature_t sig = (1 << COMP_TRANSFORM) | (1 << COMP_MESH) | (1 << COMP_CLOUDS);

    // Blending Classique pour les textures avec Alpha !
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glDepthMask(GL_FALSE); // On n'écrit pas dans le Z-buffer pour ne pas bloquer l'atmosphère

    shader_bind(shader);
    shader_set_uniform_mat4(shader, "u_view", cam->view_matrix);
    shader_set_uniform_mat4(shader, "u_projection", cam->projection_matrix);
    
    vec3 normalized_sun;
    glm_vec3_copy(sun_dir, normalized_sun);
    glm_vec3_normalize(normalized_sun);
    shader_set_uniform_vec3(shader, "u_sunDir", normalized_sun);

    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
        if ((registry->signatures[e] & sig) == sig) {
            transform_t* t = ecs_get_component(registry, e, COMP_TRANSFORM);
            mesh_component_t* m = ecs_get_component(registry, e, COMP_MESH);
            clouds_component_t* c = ecs_get_component(registry, e, COMP_CLOUDS);

            mat4 model_matrix;
            transform_get_matrix(t, model_matrix);
            shader_set_uniform_mat4(shader, "u_model", model_matrix);

            if (c->cloud_map) {
                uint32_t slot = 0;
                texture_bind(c->cloud_map, &slot);
                shader_set_uniform_1i(shader, "u_cloudMap", 0);
            }

            if (m->model) {
                for (uint32_t i = 0; i < m->model->submesh_count; i++) {
                    vertex_array_bind(&m->model->submeshes[i].vao);
                    glDrawElements(GL_TRIANGLES, m->model->submeshes[i].ibo.count, GL_UNSIGNED_INT, 0);
                    vertex_array_unbind(&m->model->submeshes[i].vao);
                }
            }
        }
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
