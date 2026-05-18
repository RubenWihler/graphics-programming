#include "s_render.h"
#include "../../engine/components/components.h"
#include "../../engine/components/c_transform.h"
#include "../../engine/components/c_mesh.h"
#include "../../engine/components/c_light.h"

#include "../../core/vendor/glad/glad.h"

#define MAX_LIGHTS 8 // Doit correspondre à la valeur dans frag.glsl

#define MAX_LIGHTS 8

static void compute_lights(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader)
{
    int light_count = 0;
    signature_t light_sig = (1 << COMP_TRANSFORM) | (1 << COMP_LIGHT);
    char uniform_name[64];
    
    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
        if ((registry->signatures[e] & light_sig) == light_sig) {
            transform_t* l_t = (transform_t*)ecs_get_component(registry, e, COMP_TRANSFORM);
            light_component_t* l_c = (light_component_t*)ecs_get_component(registry, e, COMP_LIGHT);

            vec3 actual_color;
            glm_vec3_scale(l_c->color, l_c->intensity, actual_color);

            // On envoie directement dans le tableau GLSL en générant le nom dynamiquement !
            snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].type", light_count);
            shader_set_uniform_1i(shader, uniform_name, l_c->type);

            snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].position", light_count);
            shader_set_uniform_vec3(shader, uniform_name, l_t->position);

            snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].color", light_count);
            shader_set_uniform_vec3(shader, uniform_name, actual_color);

            snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].constant", light_count);
            shader_set_uniform_1f(shader, uniform_name, l_c->constant);

            snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].linear", light_count);
            shader_set_uniform_1f(shader, uniform_name, l_c->linear);

            snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].quadratic", light_count);
            shader_set_uniform_1f(shader, uniform_name, l_c->quadratic);

            light_count++;
            if (light_count >= MAX_LIGHTS) break;
        }
    }

    // Très important : dire au shader combien de lumières tourner dans sa boucle !
    shader_set_uniform_1i(shader, "u_lightCount", light_count);
}

static void render_meshs(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader)
{
    signature_t required_signature = (1 << COMP_TRANSFORM) | (1 << COMP_MESH);

    for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) {
        if ((registry->signatures[entity] & required_signature) == required_signature) {
            if ((registry->signatures[entity] & (1 << COMP_ATMOSPHERE)) != 0 || 
                (registry->signatures[entity] & (1 << COMP_CLOUDS)) != 0) {
                continue; 
            }

            transform_t* t = (transform_t*)ecs_get_component(registry, entity, COMP_TRANSFORM);
            mesh_component_t* m = (mesh_component_t*)ecs_get_component(registry, entity, COMP_MESH);

            if (t && m && m->model) {
                mat4 model_matrix;
                transform_get_matrix(t, model_matrix);
                shader_set_uniform_mat4(shader, "u_model", model_matrix);


                for (size_t i = 0; i < m->model->submesh_count; i++) {
                    material_t* mat_to_use;
                    if (m->use_material_override) {
                        mat_to_use = &m->material_override;
                    } else {
                        int mat_idx = m->model->submeshes[i].material_index;
                        mat_to_use = (mat_idx >= 0) ? &m->model->materials[mat_idx] : NULL;
                    }

                    renderer_draw_mesh(renderer, &m->model->submeshes[i], shader, mat_to_use);
                }
            }
        }
    }
}

void s_render_update(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader, 
                     cubemap_t* irradiance_map, cubemap_t* prefilter_map, texture_t* brdf_lut) 
{
    compute_lights(registry, renderer, shader);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map->id); 
    shader_set_uniform_1i(shader, "u_irradianceMap", 6);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map->id); 
    shader_set_uniform_1i(shader, "u_prefilterMap", 7);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, brdf_lut->renderer_id); 
    shader_set_uniform_1i(shader, "u_brdfLUT", 8);

    render_meshs(registry, renderer, shader);
}
