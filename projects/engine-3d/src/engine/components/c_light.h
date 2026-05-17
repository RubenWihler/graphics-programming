#pragma once

#include "../../core/vendor/cglm/cglm.h"

typedef struct {
    vec3 color;
    float intensity;
} light_component_t;

static inline void light_component_init(light_component_t* light, vec3 color, float intensity) {
    glm_vec3_copy(color, light->color);
    light->intensity = intensity;
}
