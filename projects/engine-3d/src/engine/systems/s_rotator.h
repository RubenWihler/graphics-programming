#pragma once

#include "../../core/ecs/ecs.h"

// La fonction principale du système
void s_rotator_update(ecs_registry_t* registry, const float rot_speed, const float delta_time);
