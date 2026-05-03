#include "cam_ortho.h"

#include "../../vendor/cglm/cglm.h"
#include "../../vendor/cglm/vec3.h"
#include "../../vendor/cglm/mat4.h"
#include "../../vendor/cglm/cam.h"

#include <string.h>

static void recalculate_view_proj(cam_ortho_t *cam);

bool cam_ortho_init(cam_ortho_t *cam, float left, float right, float bottom, float top)
{
    memset(cam, 0, sizeof(*cam));
    
    //position = 0 0 0
    //rotation = 0
    cam->rotation = 0.0f;
    memcpy(cam->position, &GLM_VEC3_ZERO, sizeof(vec3));

    //initialisation de la matrice de projection
    mat4 identity = GLM_MAT4_IDENTITY_INIT;
    memcpy(cam->proj, identity, sizeof(mat4));
    memcpy(cam->view, identity, sizeof(mat4));

    cam_ortho_set_projection(cam, left, right, bottom, top);
    recalculate_view_proj(cam);

    return true;
}

void cam_ortho_destroy(cam_ortho_t *cam)
{
    (void)cam;
}

void cam_ortho_set_projection(cam_ortho_t *cam, float left, float right, float bottom, float top)
{
    cam->bounds[0] = left;
    cam->bounds[1] = right;
    cam->bounds[2] = bottom;
    cam->bounds[3] = top;

    glm_ortho(left, right, bottom, top, -1.0f, 1.0f, cam->proj);
    glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
}

void cam_ortho_set_position(cam_ortho_t *cam, vec3 new_position)
{
    memcpy(cam->position, new_position, sizeof(vec3));
    recalculate_view_proj(cam);
}

void cam_ortho_set_rotation(cam_ortho_t *cam, float new_rotation)
{
    cam->rotation = new_rotation;
    recalculate_view_proj(cam);
}

static void recalculate_view_proj(cam_ortho_t *cam)
{
    mat4 transform = GLM_MAT4_IDENTITY_INIT;
    glm_translate(transform, cam->position);
    glm_rotate(transform, cam->rotation, (vec3){0.0f, 0.0f, 1.0f});

    glm_mat4_inv(transform, cam->view);
    glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
}
