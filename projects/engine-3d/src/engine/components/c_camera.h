#pragma once

#include "../../core/vendor/cglm/cglm.h"
#include <stdbool.h>

typedef struct {
    float fov_deg;
    float near_z;
    float far_z;
    mat4 projection_matrix;
    mat4 view_matrix;
    bool is_active; // Pratique pour savoir quelle camera utiliser au rendu
} camera_component_t;

// Helper pour initialiser la matrice de projection facilement
static inline void camera_component_init(camera_component_t* cam, float fov_deg, float aspect_ratio, float near_z, float far_z) {
    cam->fov_deg = fov_deg;
    cam->near_z = near_z;
    cam->far_z = far_z;
    cam->is_active = true;
    
    glm_mat4_identity(cam->view_matrix);
    glm_perspective(glm_rad(fov_deg), aspect_ratio, near_z, far_z, cam->projection_matrix);
}

static inline void camera_component_resize(camera_component_t* cam, float aspect_ratio){
    glm_perspective(glm_rad(cam->fov_deg), aspect_ratio, cam->near_z, cam->far_z, cam->projection_matrix);
}
