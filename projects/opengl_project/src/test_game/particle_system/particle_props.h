#pragma once

#include "../../gllib/vendor/cglm/vec2.h"
#include "../../gllib/vendor/cglm/vec4.h"

typedef struct {
    vec2 vel;
    vec2 vel_variation;

    vec4 color_start;
    vec4 color_end;
    vec4 color_variation;

    float rotation;
    float rotation_variation;

    float rotation_speed;
    float rotation_speed_variation;

    float size_start;
    float size_end;
    float size_variation;
    
    float life_time;
    float life_time_variation;

    bool vel_circular;
} particle_props_t;

