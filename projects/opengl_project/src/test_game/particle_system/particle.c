#include "particle.h"

#include <stdio.h>
#include <string.h>

#define ROTATION_SPEED 0.1f

static void clean(particle_t *particle);
static void lerp_color(vec4 from, vec4 to, float t, vec4 *result);

bool particle_init(particle_t *particle)
{
    clean(particle);
    return true;
}

void particle_destroy(__attribute__((unused))particle_t *particle){}

void particle_spawn(particle_t *particle, const particle_spawn_props_t props)
{
    memcpy(particle->pos, props.pos, sizeof(particle->pos));
    memcpy(particle->vel, props.vel, sizeof(particle->vel));

    memcpy(particle->color_start, props.color_start, sizeof(particle->color_start));
    memcpy(particle->color_end, props.color_end, sizeof(particle->color_end));

    particle->rotation = props.rotation;
    particle->size_begin = props.size_begin;
    particle->size_end = props.size_end;

    particle->lifetime = props.lifetime;
    particle->lifetime_remaining = props.lifetime;

    particle->active = true;
}

void particle_despawn(particle_t *particle)
{
    clean(particle);
}

bool particle_update(particle_t *particle, const float delta_time)
{
    particle->lifetime_remaining -= delta_time;

    if(particle->lifetime_remaining <= 0.0f)
    {
        particle_despawn(particle);
        return false;
    }

    particle->pos[0] += particle->vel[0] * delta_time;
    particle->pos[1] += particle->vel[1] * delta_time;

    particle->rotation += ROTATION_SPEED * delta_time;
    
    return true;
}

void particle_set_batch_vertex(particle_t *p, float *vertex)
{
    //chaque vertex est un coin d'un carre (2 triangles)
    //on en a donc 4 par particule
    //structure d'un vertex:
    //[ posx, posy, rotation, scale, colorr, colorg, colorb, colora ]

    const int stride = 8;
    
    float life = p->lifetime_remaining / p->lifetime;
    float size = glm_lerp(p->size_end, p->size_begin, life);
    vec4 color = {0};
    lerp_color(p->color_end,p->color_start, life, &color);
    
    const float pos[4][2] = {
        {p->pos[0] - (size/2), p->pos[1] - (size/2)},//bottom left
        {p->pos[0] + (size/2), p->pos[1] - (size/2)},//bottom right
        {p->pos[0] + (size/2), p->pos[1] + (size/2)},//top left
        {p->pos[0] - (size/2), p->pos[1] + (size/2)},//top right
    };

    for(size_t i = 0; i < 4; i++)
    {
        vertex[(i * stride) + 0] = pos[i][0];
        vertex[(i * stride) + 1] = pos[i][1];
        vertex[(i * stride) + 2] = p->rotation;
        vertex[(i * stride) + 3] = size;
        vertex[(i * stride) + 4] = color[0];
        vertex[(i * stride) + 5] = color[1];
        vertex[(i * stride) + 6] = color[2];
        vertex[(i * stride) + 7] = color[3];
    }
}

static void clean(particle_t *particle)
{
    memset(particle, 0, sizeof(*particle));
    particle->active = false;
}

static void lerp_color(vec4 from, vec4 to, float t, vec4 *result)
{
    for(int i = 0; i < 4; i++)
    {
        (*result)[i] = glm_lerp(from[i], to[i], t);
    }
}

