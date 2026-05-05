#include "cam_ortho_controller.h"
#include "../../core/log/log.h"

#include "../../core/vendor/cglm/cglm.h"

#include <string.h>

static void update_camera_projection(cam_ortho_controller_t *controller);

bool cam_ortho_controller_init(cam_ortho_controller_t *controller, input_manager_t *input_manager, GLFWwindow *window, 
                               cam_ortho_controller_config_t config, const float aspect_ratio)
{
    controller->config = config;
    controller->window = window;
    controller->input_manager = input_manager;

    memset(controller->position, 0, sizeof(vec3));
    controller->rotation = 0.0f;
    controller->zoom_level = 20.0f;
    controller->aspect_ratio = aspect_ratio;

    if(!cam_ortho_init(&controller->cam, 
                       -aspect_ratio * controller->zoom_level, 
                       aspect_ratio * controller->zoom_level, 
                       -controller->zoom_level, 
                       controller->zoom_level))
    {
        LOG_ERROR_FATAL("Failed to initialize orthographic camera");
        return false;
    }

    return true;
}

void cam_ortho_controller_destroy(cam_ortho_controller_t *controller)
{
    cam_ortho_destroy(&controller->cam);
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
        glm_vec3_scale(movepos, controller->config.translate_speed * delta_time, movepos);
        
        //modifie la position de la camera
        glm_vec3_add(controller->cam.position, movepos, movepos);
        cam_ortho_set_position(&controller->cam, movepos);
    }
    
    //-------------------- Rotation --------------------//
    if(controller->config.enable_rotation)
    {
        int rotation_delta = 0;

        if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.rotate_right))
            rotation_delta -= 1;

        if(input_manager_is_key_pressed(controller->input_manager, controller->config.keys.rotate_left))
            rotation_delta += 1;

        //si on doit rotationner la camera
        if(rotation_delta != 0)
        {
            float rotation = controller->cam.rotation + (rotation_delta * controller->config.rotate_speed * delta_time);
            cam_ortho_set_rotation(&controller->cam, rotation);
        }
    }
}

void cam_ortho_controller_zoom(cam_ortho_controller_t *controller, float delta_zoom)
{
    controller->zoom_level += delta_zoom;
    controller->zoom_level = glm_clamp(controller->zoom_level, controller->config.zoom_min, controller->config.zoom_max);
    
    controller->config.translate_speed = controller->zoom_level;
    update_camera_projection(controller);
}

void cam_ortho_controller_resize(cam_ortho_controller_t *controller, int width, int height)
{
    controller->aspect_ratio = (float)width / (float)height;
    update_camera_projection(controller);
}

void cam_ortho_controller_screen_to_world(const cam_ortho_controller_t *controller, vec2 screen_pos, vec2 world_pos)
{
    int winw, winh;
    glfwGetWindowSize(controller->window, &winw, &winh);

    float boundsw = controller->cam.bounds[1] - controller->cam.bounds[0];
    float boundsh = controller->cam.bounds[3] - controller->cam.bounds[2];
    
    world_pos[0] = (screen_pos[0] / (float)winw) * boundsw - boundsw * 0.5f;
    world_pos[1] = boundsh * 0.5f - (screen_pos[1] / (float)winh) * boundsh;

    //position
    glm_vec2_add(world_pos, (vec2){controller->cam.position[0], controller->cam.position[1]}, world_pos);

    //rotation
    if(controller->config.enable_rotation)
    {
        float x = world_pos[0] - controller->cam.position[0];
        float y = world_pos[1] - controller->cam.position[1];
        float angle = controller->cam.rotation;

        world_pos[0] = x * cos(angle) - y * sin(angle) + controller->cam.position[0];
        world_pos[1] = x * sin(angle) + y * cos(angle) + controller->cam.position[1];   
    }
}

static void update_camera_projection(cam_ortho_controller_t *controller)
{
    cam_ortho_set_projection(&controller->cam, -controller->aspect_ratio * controller->zoom_level, 
                             controller->aspect_ratio * controller->zoom_level, 
                             -controller->zoom_level, controller->zoom_level);
}
