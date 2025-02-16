#pragma once 

#include "../../render/camera/cam_ortho.h"
#include "../../inputs/input_manager.h"

#include <GLFW/glfw3.h>
#include <stdbool.h>

#define CAM_ORTHO_CONTROLLER_DEFAULT_CONFIG { \
    .translate_speed = 10.0f, \
    .rotate_speed = 1.0f, \
    .enable_rotation = false, \
    .zoom_min = 0.1f, \
    .zoom_max = 100.0f, \
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
    float translate_speed;
    float rotate_speed;

    bool enable_rotation;
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
    cam_ortho_t cam;
    cam_ortho_controller_config_t config;

    input_manager_t *input_manager;
    GLFWwindow *window;

    float aspect_ratio;
    float zoom_level;

    vec3 position;
    float rotation;
} cam_ortho_controller_t;

bool cam_ortho_controller_init(cam_ortho_controller_t *controller, input_manager_t *input_manager, GLFWwindow *window, 
                               cam_ortho_controller_config_t config, const float aspect_ratio);

void cam_ortho_controller_destroy(cam_ortho_controller_t *controller);

void cam_ortho_controller_update(cam_ortho_controller_t *controller, float delta_time);
void cam_ortho_controller_zoom(cam_ortho_controller_t *controller, float delta_zoom);

void cam_ortho_controller_resize(cam_ortho_controller_t *controller, int width, int height);

void cam_ortho_controller_screen_to_world(const cam_ortho_controller_t *controller, vec2 screen_pos, vec2 world_pos);
