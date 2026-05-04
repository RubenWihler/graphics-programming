#include "cam_persp.h"

#include "../../vendor/cglm/mat4.h"
#include "../../vendor/cglm/cam.h"
#include "../../vendor/cglm/cglm.h"

#include <string.h>
#include <stdbool.h>

bool cam_persp_init(cam_persp_t *cam, float fov, float aspect, float near_z, float far_z)
{
    memset(cam, 1, sizeof(*cam));

    //initialisation des matrices
    mat4 identity = GLM_MAT4_IDENTITY_INIT;
    memcpy(cam->proj, identity, sizeof(mat4));
    memcpy(cam->view, identity, sizeof(mat4));

    memcpy(cam->position, &GLM_VEC3_ZERO, sizeof(vec3));
    memcpy(cam->target, &GLM_VEC3_ZERO, sizeof(vec3));
    memcpy(cam->up, &GLM_VEC3_ZERO, sizeof(vec3));

    cam_persp_set_projection(cam, fov, aspect, near_z, far_z);

    return true;
}

void cam_persp_set_projection(cam_persp_t *cam, float fov, float aspect, float near_z, float far_z) 
{
    cam->fov = fov;
    cam->aspect = aspect;
    cam->near_z = near_z;
    cam->far_z = far_z;
    glm_perspective(fov, aspect, near_z, far_z, cam->proj);
    cam_persp_update_view(cam);
}

void cam_persp_set_position(cam_persp_t *cam, vec3 new_position)
{
    memcpy(cam->position, new_position, sizeof(vec3));
    cam_persp_update_view(cam);
}

void cam_persp_set_aspect_ratio(cam_persp_t *cam, float aspect)
{
    cam->aspect = aspect;
    glm_perspective(cam->fov, cam->aspect, cam->near_z, cam->far_z, cam->proj);
    cam_persp_update_view(cam);
}

void cam_persp_update_view(cam_persp_t *cam)
{
    glm_lookat(cam->position, cam->target, cam->up, cam->view);
    glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
}

