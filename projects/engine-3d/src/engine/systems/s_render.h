#pragma once

#include "../../core/ecs/ecs.h"
#include "../../core/render/renderer/renderer.h"
#include "../../core/render/shader/shader.h"

// La fonction principale du système
void s_render_update(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader, 
                     cubemap_t* irradiance_map, cubemap_t* prefilter_map, texture_t* brdf_lut);
