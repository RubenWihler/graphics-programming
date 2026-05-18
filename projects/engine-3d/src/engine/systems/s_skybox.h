#pragma once

#include "../../core/render/texture/texture.h"
#include "../../core/render/shader/shader.h"
#include "../../engine/components/c_camera.h"

// On initialise les sommets du cube une seule fois
void s_skybox_init(void);

// On l'appelle à chaque frame
void s_skybox_render(cubemap_t* skybox, camera_component_t* camera, shader_t* skybox_shader);
