#pragma once

#include "../../core/vendor/cglm/cglm.h"

typedef enum {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT = 1
} light_type_t;

typedef struct {
    light_type_t type;
    vec3 color;
    float intensity;
    
    // Valeurs d'atténuation (utiles uniquement pour les Point Lights)
    float constant;
    float linear;
    float quadratic;
} light_component_t;

// Helper pour créer un Soleil (pas d'atténuation, distance infinie)
static inline void light_component_init_directional(light_component_t* light, vec3 color, float intensity) {
    light->type = LIGHT_DIRECTIONAL;
    glm_vec3_copy(color, light->color);
    light->intensity = intensity;
    light->constant = 1.0f;
    light->linear = 0.0f;
    light->quadratic = 0.0f;
}

// Helper pour créer une Torche/Ampoule
// Pour une portée d'environ 50 unités, utilise : lin = 0.09f, quad = 0.032f
static inline void light_component_init_point(light_component_t* light, vec3 color, float intensity, float lin, float quad) {
    light->type = LIGHT_POINT;
    glm_vec3_copy(color, light->color);
    light->intensity = intensity;
    light->constant = 1.0f; // Toujours 1.0 par convention
    light->linear = lin;
    light->quadratic = quad;
}
