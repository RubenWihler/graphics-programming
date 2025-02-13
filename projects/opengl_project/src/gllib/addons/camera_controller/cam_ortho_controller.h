#pragma once 

#include "../../render/camera/cam_ortho.h"
#include "../../inputs/input_manager.h"

#include <stdbool.h>

#define CAM_ORTHO_CONTROLLER_DEFAULT_CONFIG \
{ \
    .camera_speed = 400.0f, \
    .rotate_speed = 1.0f, \
    .zoom_speed = 0.1f, \
    .zoom_min = 0.1f, \
    .zoom_max = 10.0f, \
    .keys = { \
        .up = GLFW_KEY_W, \
        .left = GLFW_KEY_A, \
        .down = GLFW_KEY_S, \
        .right = GLFW_KEY_D, \
        .rotate_right = GLFW_KEY_E, \
        .rotate_left = GLFW_KEY_Q \
    } \
}

typedef struct {
    float camera_speed;
    float rotate_speed;
    float zoom_speed;

    float zoom_min;
    float zoom_max;

    struct {
        int up;
        int left;
        int down;
        int right;
        int rotate_right;
        int rotate_left;
    } keys;
} cam_ortho_controller_config_t;

typedef struct {
    cam_ortho_t *cam;
    cam_ortho_controller_config_t config;
    
    input_manager_t *input_manager;
    float zoom;
} cam_ortho_controller_t;

bool cam_ortho_controller_init(cam_ortho_controller_t *controller, cam_ortho_t *cam, 
                               input_manager_t *input_manager, cam_ortho_controller_config_t config);

void cam_ortho_controller_destroy(cam_ortho_controller_t *controller);

void cam_ortho_controller_update(cam_ortho_controller_t *controller, float delta_time);
void cam_ortho_controller_zoom(cam_ortho_controller_t *controller, float delta_zoom);
void cam_ortho_controller_set_zoom(cam_ortho_controller_t *controller, float zoom);

void cam_ortho_controller_resize_viewport(cam_ortho_controller_t *controller, int width, int height);
