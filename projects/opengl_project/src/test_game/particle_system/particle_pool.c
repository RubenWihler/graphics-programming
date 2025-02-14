#include "particle_pool.h"
#include "particle.h"

#include "../../gllib/log/log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static unsigned int batch(particle_pool_t *pool);
static bool init_render(particle_pool_t *pool);

bool particle_pool_init(particle_pool_t *pool, const particle_props_t props, const size_t capacity)
{
    //l'index est initialisé à la fin du tableau
    //car on va parcourir le tableau à l'envers pour que le blending fonctionne bien
    pool->index = capacity - 1;
    pool->props = props;
    pool->capacity = capacity;

    pool->particles = (particle_t*)malloc(capacity * sizeof(particle_t));
    if(!pool->particles) return (perror("malloc"), false);

    if(!init_render(pool)) {
        LOG_ERROR("failed to initialize particle render", false);
        free(pool->particles);
        pool->particles = NULL;
        return false;
    }

    return true;
}

void particle_pool_destroy(particle_pool_t *pool)
{
    free(pool->particles);
    pool->particles = NULL;

    free(pool->vertices);
    pool->vertices = NULL;

    free(pool->indices);
    pool->indices = NULL;

    vertex_array_destroy(&pool->va);
    vertex_buffer_destroy(&pool->vb);
    index_buffer_destroy(&pool->ib);
    vertex_buffer_layout_destroy(&pool->layout);
}

void particle_pool_update(particle_pool_t *pool, const float delta_time)
{
    for(unsigned int i = 0; i < pool->capacity; i++)
    {
        particle_t *particle = &pool->particles[i];
        if(!particle->active) continue;

        if(!particle_update(particle, delta_time))
        {
            particle_despawn(particle);
        }
    }
}

void particle_pool_render(particle_pool_t *pool, const shader_t *shader, const renderer_t *renderer, const cam_ortho_t *cam)
{
    unsigned int count = batch(pool);

    vertex_buffer_set_data(&pool->vb, pool->vertices, count * VERTEX_PER_PARTICLE * PARTICLE_STRIDE * sizeof(float));
    index_buffer_set_data(&pool->ib, pool->indices, count * INDICE_PER_PARTICLE);

    renderer_draw(renderer, &pool->va, &pool->ib, shader);
}

void particle_pool_emit(particle_pool_t *pool, const size_t count, const vec2 pos)
{
    //pour l'instant blc de count
    (void)count;

    particle_t *particle = &pool->particles[pool->index];

    particle_spawn_props_t props = {0};
    memcpy(props.pos, pos, sizeof(props.pos));

    //velocity
    memcpy(props.vel, pool->props.vel, sizeof(props.vel));
    props.vel[0] += pool->props.vel_variation[0] * (((float)rand() / RAND_MAX) - 0.5f);
    props.vel[1] += pool->props.vel_variation[1] * (((float)rand() / RAND_MAX) - 0.5f);
    
    //rotation
    props.rotation = pool->props.rotation + pool->props.rotation_variation * (((float)rand() / RAND_MAX) - 0.5f);

    //color
    memcpy(props.color_start, pool->props.color_start, sizeof(props.color_start));
    memcpy(props.color_end, pool->props.color_end, sizeof(props.color_end));
    props.color_start[0] += pool->props.color_variation[0] * (((float)rand() / RAND_MAX) - 0.5f);
    props.color_start[1] += pool->props.color_variation[1] * (((float)rand() / RAND_MAX) - 0.5f);
    props.color_start[2] += pool->props.color_variation[2] * (((float)rand() / RAND_MAX) - 0.5f);

    //size
    props.size_begin = pool->props.size_start + pool->props.size_variation * (((float)rand() / RAND_MAX) - 0.5f);
    props.size_end = pool->props.size_end + pool->props.size_variation * (((float)rand() / RAND_MAX) - 0.5f);

    //lifetime
    props.lifetime = pool->props.life_time + pool->props.life_time_variation * (((float)rand() / RAND_MAX) - 0.5f);

    particle_spawn(particle, props);
    pool->index = (--pool->index % pool->capacity + pool->capacity) % pool->capacity;
}

static bool init_render(particle_pool_t *pool)
{
    //render
    //pour le vertex buffer:
    //on batch
    //structure d'un vertex:
    //[ posx, posy, rotation, size, colorr, colorg, colorb, colora ]
    
    const size_t vertex_buffer_size = VERTEX_PER_PARTICLE * PARTICLE_STRIDE * pool->capacity * sizeof(float);
    const size_t indice_buffer_size = INDICE_PER_PARTICLE * pool->capacity * sizeof(unsigned int);
     
    pool->vertices = (float*)malloc(vertex_buffer_size);
    if(!pool->vertices) return (perror("malloc"), free(pool->particles), false);

    pool->indices = (unsigned int*)malloc(indice_buffer_size);
    if(!pool->indices) return (perror("malloc"), free(pool->particles), free(pool->vertices), false);

    vertex_array_init(&pool->va);
    vertex_buffer_init(&pool->vb, pool->vertices, vertex_buffer_size, true);
    
    //layout
    vertex_buffer_layout_init(&pool->layout);
    vertex_buffer_layout_push_float(&pool->layout, 2);//pos
    vertex_buffer_layout_push_float(&pool->layout, 1);//rotation
    vertex_buffer_layout_push_float(&pool->layout, 1);//size
    vertex_buffer_layout_push_float(&pool->layout, 4);//color

    //ajoute le buffer avec le layout
    vertex_array_add_buffer(&pool->va, &pool->vb, &pool->layout);
    
    index_buffer_init(&pool->ib, pool->indices, INDICE_PER_PARTICLE * pool->capacity, true);

    return true;
}

//retourne le nombre de particules actives
static unsigned int batch(particle_pool_t *pool)
{
    const float indices[] = PARTICLE_INDICES_TEMPLATE;

    size_t vert_index = 0;
    size_t indice_index = 0;
    unsigned int count = 0;

    for(size_t i = 0; i < pool->capacity; i++)
    {
        particle_t *particle = &pool->particles[i];
        if(!particle->active) continue;
        
        //vertex
        particle_set_batch_vertex(particle, &pool->vertices[vert_index]);

        //un peu plus explicite
        /* pool->indices[indice_index + 0] = count * vertex_per_particle + indices[0];//premier triangle */
        /* pool->indices[indice_index + 1] = count * vertex_per_particle + indices[1]; */
        /* pool->indices[indice_index + 2] = count * vertex_per_particle + indices[2]; */
        /* pool->indices[indice_index + 3] = count * vertex_per_particle + indices[3];//deuxieme triangle */
        /* pool->indices[indice_index + 4] = count * vertex_per_particle + indices[4]; */
        /* pool->indices[indice_index + 5] = count * vertex_per_particle + indices[5]; */

        //indices
        //on ajoute les indices pour les 2 triangles
        for(size_t i = 0; i < INDICE_PER_PARTICLE; i++)
        {
            pool->indices[indice_index + i] = count * VERTEX_PER_PARTICLE + indices[i];
        }

        vert_index += 4 * PARTICLE_STRIDE;
        indice_index += INDICE_PER_PARTICLE;
        count++;
    }

    return count;
}
