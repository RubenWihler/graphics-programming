#pragma once

#include "../../vendor/cglm/cglm.h"

#include <stdbool.h>

typedef struct {
    vec3 position;
    float rotation;

    vec4 bounds;
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 view_projection_matrix;
} cam_ortho_t;

bool cam_ortho_init(cam_ortho_t *cam, float left, float right, float bottom, float top);
void cam_ortho_destroy(cam_ortho_t *cam);

void cam_ortho_set_projection(cam_ortho_t *cam, float left, float right, float bottom, float top);
void cam_ortho_set_position(cam_ortho_t *cam, vec3 new_position);
void cam_ortho_set_rotation(cam_ortho_t *cam, float new_rotation);

