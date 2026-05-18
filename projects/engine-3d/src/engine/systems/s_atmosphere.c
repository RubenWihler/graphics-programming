#include "s_atmosphere.h"
#include "../../core/vendor/glad/glad.h"

// 1. Modifie la signature de la fonction pour passer l'ID de l'entité Caméra
void s_atmosphere_render(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader, camera_component_t* cam, entity_t cam_entity_id, vec3 sun_dir) {
    if (!cam) return;

    signature_t sig = (1 << COMP_TRANSFORM) | (1 << COMP_MESH) | (1 << COMP_ATMOSPHERE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glDepthMask(GL_FALSE); 
    glEnable(GL_CULL_FACE); 

    shader_bind(shader);
    
    shader_set_uniform_mat4(shader, "u_view", cam->view_matrix);
    shader_set_uniform_mat4(shader, "u_projection", cam->projection_matrix);

    // 2. RÉCUPÈRE LA VRAIE POSITION DE LA CAMÉRA ICI !
    vec3 cam_pos = {0.0f, 0.0f, 0.0f};
    transform_t* cam_transform = ecs_get_component(registry, cam_entity_id, COMP_TRANSFORM);
    if (cam_transform) {
        glm_vec3_copy(cam_transform->position, cam_pos);
    }
    shader_set_uniform_vec3(shader, "u_cameraPos", cam_pos);
    
    vec3 normalized_sun;
    glm_vec3_copy(sun_dir, normalized_sun);
    glm_vec3_normalize(normalized_sun);
    shader_set_uniform_vec3(shader, "u_sunDir", normalized_sun);

    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
        if ((registry->signatures[e] & sig) == sig) {
            transform_t* t = ecs_get_component(registry, e, COMP_TRANSFORM);
            mesh_component_t* m = ecs_get_component(registry, e, COMP_MESH);
            atmosphere_component_t* a = ecs_get_component(registry, e, COMP_ATMOSPHERE);

            float distance = glm_vec3_distance(cam_pos, t->position);
            if (distance > a->atmosphere_radius) {
                glCullFace(GL_BACK);
            } else {
                glCullFace(GL_FRONT);
            }

            shader_set_uniform_vec3(shader, "u_planetCenter", t->position);
            shader_set_uniform_1f(shader, "u_planetRadius", a->planet_radius);
            shader_set_uniform_1f(shader, "u_atmosphereRadius", a->atmosphere_radius);
            shader_set_uniform_1f(shader, "u_sunIntensity", a->sun_intensity);

            mat4 model_matrix;
            transform_get_matrix(t, model_matrix);
            shader_set_uniform_mat4(shader, "u_model", model_matrix);

            if (m->model) {
                for (uint32_t i = 0; i < m->model->submesh_count; i++) {
                    vertex_array_bind(&m->model->submeshes[i].vao);
                    glDrawElements(GL_TRIANGLES, m->model->submeshes[i].ibo.count, GL_UNSIGNED_INT, 0);
                    vertex_array_unbind(&m->model->submeshes[i].vao);
                }
            }
        }
    }

    glCullFace(GL_BACK); 
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}


