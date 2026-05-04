#pragma once

#include "../../render/camera/cam_persp.h"
#include "../../inputs/input_manager.h"
#include <stdbool.h>

typedef struct {
    cam_persp_t cam;                 // La caméra elle-même
    input_manager_t *input_manager;  // Pour lire clavier/souris

    float yaw;                       // Rotation gauche/droite (en degrés)
    float pitch;                     // Rotation haut/bas (en degrés)
    
    float movement_speed;
    float mouse_sensitivity;
    float fov_deg;
    float near_z;
    float far_z;

    vec2 last_mouse_pos;             // Pour calculer le mouvement de la souris
    bool first_mouse;                // Pour éviter un "saut" au premier clic
} cam_persp_controller_t;

void cam_persp_controller_init(cam_persp_controller_t *ctrl, input_manager_t *input, float aspect_ratio);
void cam_persp_controller_update(cam_persp_controller_t *ctrl, float delta_time);
void cam_persp_controller_resize(cam_persp_controller_t *ctrl, float aspect_ratio);
