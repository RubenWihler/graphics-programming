#pragma once
#include "../../core/ecs/ecs.h"
#include "../../core/render/renderer/renderer.h"
#include "../../core/render/shader/shader.h"
#include "../../engine/components/components.h"

void s_atmosphere_render(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader, camera_component_t* cam, entity_t cam_entity_id, vec3 sun_dir);
