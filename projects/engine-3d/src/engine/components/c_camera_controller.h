#pragma once

#include "../../core/vendor/cglm/cglm.h"
#include <stdbool.h>

typedef struct {
    float movement_speed;
    float mouse_sensitivity;
    float yaw;
    float pitch;
    vec2 last_mouse_pos;
    bool first_mouse;
} camera_controller_component_t;

static inline void camera_controller_init(camera_controller_component_t* ctrl, float speed, float sens) {
    ctrl->movement_speed = speed;
    ctrl->mouse_sensitivity = sens;
    ctrl->yaw = -90.0f; // Regarde vers les Z negatifs
    ctrl->pitch = 0.0f;
    ctrl->first_mouse = true;
}
