#include "s_camera_controller.h"
#include "../components/components.h"
#include "../components/c_transform.h"
#include "../components/c_camera.h"
#include "../components/c_camera_controller.h"
#include <GLFW/glfw3.h>

void s_camera_controller_update(ecs_registry_t* registry, input_manager_t* input, float delta_time) {
    // L'entite doit posseder ces 3 composants pour etre controlee !
    signature_t required_sig = (1 << COMP_TRANSFORM) | (1 << COMP_CAMERA) | (1 << COMP_CAMERA_CONTROLLER);

    for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) {
        if ((registry->signatures[entity] & required_sig) == required_sig) {
            
            transform_t* t = (transform_t*)ecs_get_component(registry, entity, COMP_TRANSFORM);
            camera_component_t* cam = (camera_component_t*)ecs_get_component(registry, entity, COMP_CAMERA);
            camera_controller_component_t* ctrl = (camera_controller_component_t*)ecs_get_component(registry, entity, COMP_CAMERA_CONTROLLER);

            if (!cam->is_active) continue;

            // --- 1. SOURIS (ROTATION) ---
            vec2 mouse_pos = {0};
            input_manager_get_mouse_pos(input, &mouse_pos[0]);
            
            if (ctrl->first_mouse) {
                ctrl->last_mouse_pos[0] = mouse_pos[0];
                ctrl->last_mouse_pos[1] = mouse_pos[1];
                ctrl->first_mouse = false;
            }

            float x_offset = (mouse_pos[0] - ctrl->last_mouse_pos[0]) * ctrl->mouse_sensitivity;
            float y_offset = (ctrl->last_mouse_pos[1] - mouse_pos[1]) * ctrl->mouse_sensitivity; // Inverse
            
            ctrl->last_mouse_pos[0] = mouse_pos[0];
            ctrl->last_mouse_pos[1] = mouse_pos[1];

            ctrl->yaw += x_offset;
            ctrl->pitch += y_offset;
            
            if (ctrl->pitch > 89.0f) ctrl->pitch = 89.0f;
            if (ctrl->pitch < -89.0f) ctrl->pitch = -89.0f;

            // Calcul du vecteur 'Front'
            vec3 front;
            front[0] = cosf(glm_rad(ctrl->yaw)) * cosf(glm_rad(ctrl->pitch));
            front[1] = sinf(glm_rad(ctrl->pitch));
            front[2] = sinf(glm_rad(ctrl->yaw)) * cosf(glm_rad(ctrl->pitch));
            glm_vec3_normalize(front);

            // --- 2. CLAVIER (MOUVEMENT) ---
            float velocity = ctrl->movement_speed * delta_time;
            if (input_manager_is_key_pressed(input, GLFW_KEY_LEFT_SHIFT)) velocity *= 2.0f;

            vec3 right, up = {0.0f, 1.0f, 0.0f}, move_dir = {0};
            glm_vec3_crossn(front, up, right);

            if (input_manager_is_key_pressed(input, GLFW_KEY_W)) {
                glm_vec3_scale(front, velocity, move_dir);
                glm_vec3_add(t->position, move_dir, t->position);
            }
            if (input_manager_is_key_pressed(input, GLFW_KEY_S)) {
                glm_vec3_scale(front, velocity, move_dir);
                glm_vec3_sub(t->position, move_dir, t->position);
            }
            if (input_manager_is_key_pressed(input, GLFW_KEY_A)) {
                glm_vec3_scale(right, velocity, move_dir);
                glm_vec3_sub(t->position, move_dir, t->position);
            }
            if (input_manager_is_key_pressed(input, GLFW_KEY_D)) {
                glm_vec3_scale(right, velocity, move_dir);
                glm_vec3_add(t->position, move_dir, t->position);
            }

            // --- 3. RECALCUL DE LA MATRICE DE VUE ---
            vec3 target;
            glm_vec3_add(t->position, front, target);
            glm_lookat(t->position, target, up, cam->view_matrix);
        }
    }
}
