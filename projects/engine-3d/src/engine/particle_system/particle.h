#pragma once

#include "particle_spawn_props.h"

#include "../../core/vendor/cglm/vec2.h"
#include "../../core/vendor/cglm/vec4.h"

//quelques constantes pour le batching des particules
//chaque vertex est un coin d'un carre
//chaque carre est compose de 2 triangles
//8 float par vertex
#define PARTICLE_STRIDE 7
//2 triangle par particule
#define INDICE_PER_PARTICLE 2 * 3
//4 vertex par particule (les 4 coins d'un carre)
#define VERTEX_PER_PARTICLE 4
//indices pour les 2 triangles
#define PARTICLE_INDICES_TEMPLATE {0, 1, 2, \
                                   2, 3, 0}

typedef struct _particle_t particle_t;

struct _particle_t {
    vec2 pos;
    vec2 vel;

    vec4 color_start;
    vec4 color_end;

    float rotation;
    float rotation_speed;

    float size_begin;
    float size_end;

    float lifetime;
    float lifetime_remaining;

    bool active;
};

bool particle_init(particle_t *particle);
void particle_destroy(particle_t *particle);

void particle_spawn(particle_t *particle, const particle_spawn_props_t props);
void particle_despawn(particle_t *particle);

bool particle_update(particle_t *particle, const float delta_time);
void particle_set_batch_vertex(particle_t *particle, float *vertex);

