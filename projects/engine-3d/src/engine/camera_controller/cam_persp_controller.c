#include "cam_persp_controller.h"

#include "../../core/vendor/cglm/vec3.h"
#include <GLFW/glfw3.h>

void cam_persp_controller_init(cam_persp_controller_t *ctrl, input_manager_t *input, float aspect_ratio, const cam_persp_controller_config_t config)
{
    ctrl->input_manager = input;
    
    // Valeurs par défaut
    ctrl->yaw = -90.0f; // Regarde vers l'avant (Z négatif)
    ctrl->pitch = 0.0f;
    ctrl->first_mouse = true;

    // lire depuis config
    ctrl->movement_speed = config.movement_speed; //5.0
    ctrl->mouse_sensitivity = config.mouse_sensitivity; //0.1
    ctrl->fov_deg = config.fov_deg; //45.0f;
    ctrl->near_z = config.near_z; //0.1f;
    ctrl->far_z  = config.far_z; //100.0f;

    // Position initiale de la caméra
    glm_vec3_copy((vec3){0.0f, 0.0f, 5.0f}, ctrl->cam.position);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, ctrl->cam.up);

    cam_persp_controller_resize(ctrl, aspect_ratio);
}

void cam_persp_controller_update(cam_persp_controller_t *ctrl, float delta_time)
{
    // --- 1. GESTION DE LA SOURIS (LOOK) ---
    vec2 mouse_xy = {0,0};
    input_manager_get_mouse_pos(ctrl->input_manager, &mouse_xy[0]);
    float mouse_x = mouse_xy[0], mouse_y = mouse_xy[1];

    if (ctrl->first_mouse) 
    {
        ctrl->last_mouse_pos[0] = mouse_x;
        ctrl->last_mouse_pos[1] = mouse_y;
        ctrl->first_mouse = false;
    }

    float x_offset = mouse_x - ctrl->last_mouse_pos[0];
    float y_offset = ctrl->last_mouse_pos[1] - mouse_y; 
    ctrl->last_mouse_pos[0] = mouse_x;
    ctrl->last_mouse_pos[1] = mouse_y;

    x_offset *= ctrl->mouse_sensitivity;
    y_offset *= ctrl->mouse_sensitivity;

    ctrl->yaw   += x_offset;
    ctrl->pitch += y_offset;

    if (ctrl->pitch > 89.0f)  ctrl->pitch = 89.0f;
    if (ctrl->pitch < -89.0f) ctrl->pitch = -89.0f;

    vec3 front;
    front[0] = cosf(glm_rad(ctrl->yaw)) * cosf(glm_rad(ctrl->pitch));
    front[1] = sinf(glm_rad(ctrl->pitch));
    front[2] = sinf(glm_rad(ctrl->yaw)) * cosf(glm_rad(ctrl->pitch));
    glm_vec3_normalize(front);

    // --- 2. GESTION DU CLAVIER (MOVE) ---
    float velocity = ctrl->movement_speed * delta_time;
    vec3 move_dir = {0.0f, 0.0f, 0.0f};

    if (input_manager_is_key_pressed(ctrl->input_manager, GLFW_KEY_LEFT_SHIFT)) {
        velocity *= 2;
    }

    if (input_manager_is_key_pressed(ctrl->input_manager, GLFW_KEY_W)) 
    {
        glm_vec3_scale(front, velocity, move_dir);
        glm_vec3_add(ctrl->cam.position, move_dir, ctrl->cam.position);
    }
    if (input_manager_is_key_pressed(ctrl->input_manager, GLFW_KEY_S)) 
    {
        glm_vec3_scale(front, velocity, move_dir);
        glm_vec3_sub(ctrl->cam.position, move_dir, ctrl->cam.position);
    }

    vec3 right;
    glm_vec3_crossn(front, ctrl->cam.up, right);

    if (input_manager_is_key_pressed(ctrl->input_manager, GLFW_KEY_A)) 
    {
        glm_vec3_scale(right, velocity, move_dir);
        glm_vec3_sub(ctrl->cam.position, move_dir, ctrl->cam.position);
    }
    if (input_manager_is_key_pressed(ctrl->input_manager, GLFW_KEY_D)) 
    {
        glm_vec3_scale(right, velocity, move_dir);
        glm_vec3_add(ctrl->cam.position, move_dir, ctrl->cam.position);
    }

    // --- 3. MISE A JOUR DES MATRICES ---
    // Calcul final de la target UNE SEULE FOIS ici, après tous les mouvements
    glm_vec3_add(ctrl->cam.position, front, ctrl->cam.target);
    cam_persp_update_view(&ctrl->cam);
}

void cam_persp_controller_resize(cam_persp_controller_t *ctrl, float aspect_ratio)
{
    cam_persp_set_projection(
        &ctrl->cam, 
        glm_rad(ctrl->fov_deg), 
        aspect_ratio, 
        ctrl->near_z, 
        ctrl->far_z
    );
}
