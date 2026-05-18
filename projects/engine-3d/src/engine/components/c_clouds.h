#pragma once
#include "../../core/render/texture/texture.h"

typedef struct {
    texture_t* cloud_map;
    float rotation_speed; // Pour faire tourner la météo plus tard !
} clouds_component_t;
