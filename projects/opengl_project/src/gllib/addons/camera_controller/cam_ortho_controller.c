#include "cam_ortho_controller.h"

bool cam_ortho_controller_init(cam_ortho_controller_t *controller, cam_ortho_t *cam, 
                               input_manager_t *input_manager, cam_ortho_controller_config_t config)
{
    controller->cam = cam;
    controller->input_manager = input_manager;
    controller->config = config;
    controller->zoom = 1.0f;

    return true;
}

void cam_ortho_controller_destroy(cam_ortho_controller_t *controller)
{
    controller->cam = NULL;
    controller->input_manager = NULL;
}

void cam_ortho_controller_update(cam_ortho_controller_t *controller, float delta_time)
{
    //-------------------- Position --------------------//
    vec3 movepos = GLM_VEC3_ZERO_INIT;
    
    if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.up))
        glm_vec3_add(movepos, (vec3){0, 1, 0}, movepos);

    if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.left))
        glm_vec3_add(movepos, (vec3){-1, 0, 0}, movepos);

    if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.down))
        glm_vec3_add(movepos, (vec3){0, -1, 0}, movepos);

    if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.right))
        glm_vec3_add(movepos, (vec3){1, 0, 0}, movepos);

    //si on doit bouger la camera
    if(!glm_vec3_eqv_eps(movepos, GLM_VEC3_ZERO))
    {
        //normalise le vecteur et multiplie par vitesse * delta time
        glm_vec3_normalize(movepos);
        glm_vec3_scale(movepos, controller->config.camera_speed * delta_time, movepos);
        
        //modifie la position de la camera
        glm_vec3_add(controller->cam->position, movepos, movepos);
        cam_ortho_set_position(controller->cam, movepos);
    }
    
    //-------------------- Rotation --------------------//
    int rotation_delta = 0;

    if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.rotate_right))
        rotation_delta -= 1;

    if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.rotate_left))
        rotation_delta += 1;

    //si on doit rotationner la camera
    if(rotation_delta != 0)
    {
        float rotation = controller->cam->rotation + (rotation_delta * controller->config.rotate_speed * delta_time);
        cam_ortho_set_rotation(controller->cam, rotation);
    }
}

void cam_ortho_controller_zoom(cam_ortho_controller_t *controller, float delta_zoom)
{
    //pour pas que le zoom soit trop rapide
    //on multiplie par la vitesse de zoom et le delta time
    
    float zoom = controller->zoom + (delta_zoom * controller->config.zoom_speed);
    cam_ortho_controller_set_zoom(controller, zoom);
}

void cam_ortho_controller_set_zoom(cam_ortho_controller_t *controller, float zoom)
{
    controller->zoom = zoom;
    controller->zoom = glm_clamp(controller->zoom, controller->config.zoom_min, controller->config.zoom_max);
    cam_ortho_set_scale(controller->cam, (vec3){controller->zoom, controller->zoom, 1.0f});
}

void cam_ortho_controller_resize_viewport(cam_ortho_controller_t *controller, int width, int height)
{
    cam_ortho_set_viewport(controller->cam, 0.0f, width, 0.0f, height);
}
