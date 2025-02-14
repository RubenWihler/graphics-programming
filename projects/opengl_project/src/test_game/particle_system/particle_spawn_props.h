#pragma once

#include "../../gllib/vendor/cglm/vec2.h"
#include "../../gllib/vendor/cglm/vec4.h"

typedef struct {
    vec2 pos;
    vec2 vel;

    vec4 color_start;
    vec4 color_end;

    float size_begin;
    float size_end;

    float rotation;
    float lifetime;
} particle_spawn_props_t;
