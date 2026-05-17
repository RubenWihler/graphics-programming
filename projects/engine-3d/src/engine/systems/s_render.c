#include "s_render.h"
#include "../../engine/components/components.h"
#include "../../engine/components/c_transform.h"
#include "../../engine/components/c_mesh.h"
#include "../../engine/components/c_light.h"

#define MAX_LIGHTS 8 // Doit correspondre à la valeur dans frag.glsl

static void compute_lights(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader)
{
    vec3 light_positions[MAX_LIGHTS]; 
    vec3 light_colors[MAX_LIGHTS];
    int light_count = 0;

    signature_t light_sig = (1 << COMP_TRANSFORM) | (1 << COMP_LIGHT);

    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
        if ((registry->signatures[e] & light_sig) == light_sig) {
            transform_t* l_t = (transform_t*)ecs_get_component(registry, e, COMP_TRANSFORM);
            light_component_t* l_c = (light_component_t*)ecs_get_component(registry, e, COMP_LIGHT);

            // On ajoute cette lumière à nos tableaux
            glm_vec3_copy(l_t->position, light_positions[light_count]);
            glm_vec3_scale(l_c->color, l_c->intensity, light_colors[light_count]);
            
            light_count++;
            
            // Sécurité : on arrête de chercher si on a atteint la limite du Shader
            if (light_count >= MAX_LIGHTS) {
                break; 
            }
        }
    }

    // On envoie le compteur au Shader (très important pour la boucle 'for' du GLSL !)
    shader_set_uniform_1i(shader, "u_lightCount", light_count);

    // On envoie chaque lumière via une boucle et un sprintf
    char uniform_name[64];
    for (int i = 0; i < light_count; i++) {
        // Envoi de la position
        snprintf(uniform_name, sizeof(uniform_name), "u_lightPos[%d]", i);
        shader_set_uniform_vec3(shader, uniform_name, light_positions[i]);

        // Envoi de la couleur
        snprintf(uniform_name, sizeof(uniform_name), "u_lightColor[%d]", i);
        shader_set_uniform_vec3(shader, uniform_name, light_colors[i]);
    }
}

static void render_meshs(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader)
{
    signature_t required_signature = (1 << COMP_TRANSFORM) | (1 << COMP_MESH);

    for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) {
        if ((registry->signatures[entity] & required_signature) == required_signature) {

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

void s_render_update(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader) 
{
    compute_lights(registry, renderer, shader);
    render_meshs(registry, renderer, shader);
}
