#pragma once

#include "../../core/ecs/ecs.h"
#include "../../core/inputs/input_manager.h"

void s_camera_controller_update(ecs_registry_t* registry, input_manager_t* input, float delta_time);
