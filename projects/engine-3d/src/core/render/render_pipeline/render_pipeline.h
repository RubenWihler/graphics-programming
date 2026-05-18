#pragma once

#include "../../../core/render/shader/shader.h"
#include "../../../core/render/renderer/renderer.h"
#include "../../../core/ecs/ecs.h"
#include "../../../engine/components/c_camera.h"
#include "../environment/environment.h" // Modifie le chemin si besoin

typedef struct {
    int width;
    int height;

    // --- Shaders de la scène ---
    shader_t pbr_shader;
    shader_t skybox_shader;
    shader_t atmosphere_shader;
    shader_t clouds_shader;

    // --- Shaders de Post-Processing ---
    shader_t blur_shader;
    shader_t postprocess_shader;

    // --- FBO HDR (Multiple Render Targets) ---
    unsigned int hdr_fbo;
    unsigned int color_buffers[2]; // 0: Scène normale, 1: Zones brillantes
    unsigned int rbo_depth;

    // --- FBO Ping-Pong (Pour le flou) ---
    unsigned int pingpong_fbo[2];
    unsigned int pingpong_colorbuffers[2];

    // --- Géométrie de l'écran virtuel ---
    unsigned int quad_vao;
    unsigned int quad_vbo;
} render_pipeline_t;

void render_pipeline_init(render_pipeline_t* pipeline, int window_width, int window_height);
void render_pipeline_draw(render_pipeline_t* pipeline, ecs_registry_t* registry, renderer_t* renderer, environment_t* env, camera_component_t* cam, entity_t cam_entity_id);
void render_pipeline_destroy(render_pipeline_t* pipeline);
void render_pipeline_resize(render_pipeline_t* pipeline, int width, int height);
