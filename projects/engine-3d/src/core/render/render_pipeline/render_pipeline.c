#include "render_pipeline.h"
#include "../../../core/vendor/glad/glad.h"
#include "../../../core/log/log.h"
#include "../../../engine/systems/s_render.h"
#include "../../../engine/systems/s_skybox.h"
#include "../../../engine/systems/s_atmosphere.h"
#include "../../../engine/systems/s_clouds.h"
#include "../../../engine/components/components.h"

// Crée un rectangle qui couvre tout l'écran
static void setup_screen_quad(render_pipeline_t* pipeline) {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
    };
    glGenVertexArrays(1, &pipeline->quad_vao);
    glGenBuffers(1, &pipeline->quad_vbo);
    glBindVertexArray(pipeline->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, pipeline->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void render_pipeline_init(render_pipeline_t* pipeline, int window_width, int window_height) {
    pipeline->width = window_width;
    pipeline->height = window_height;


    // 1. Initialiser tous les Shaders
    shader_init(&pipeline->pbr_shader, "res/shaders/pbr");
    shader_init(&pipeline->skybox_shader, "res/shaders/skybox");
    shader_init(&pipeline->atmosphere_shader, "res/shaders/atmosphere");
    shader_init(&pipeline->clouds_shader, "res/shaders/clouds");
    shader_init(&pipeline->blur_shader, "res/shaders/blur");
    shader_init(&pipeline->postprocess_shader, "res/shaders/postprocess");
    //initialiser la skybox
    s_skybox_init();

    setup_screen_quad(pipeline);

    // ==========================================
    // 2. CREATION DU FBO HDR (SCENE + BRILLANCE)
    // ==========================================
    glGenFramebuffers(1, &pipeline->hdr_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pipeline->hdr_fbo);

    glGenTextures(2, pipeline->color_buffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, pipeline->color_buffers[i]);
        // GL_RGBA16F est crucial pour stocker la lumière HDR au-dessus de 1.0
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // On attache la texture 0 ou 1 au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, pipeline->color_buffers[i], 0);
    }

    // Depth buffer pour la 3D
    glGenRenderbuffers(1, &pipeline->rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, pipeline->rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, window_width, window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pipeline->rbo_depth);

    // On dit à OpenGL : "Dessine dans ces deux textures en même temps"
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Erreur : HDR Framebuffer incomplet");

    // ==========================================
    // 3. CREATION DES FBO PING-PONG POUR LE FLOU
    // ==========================================
    glGenFramebuffers(2, pipeline->pingpong_fbo);
    glGenTextures(2, pipeline->pingpong_colorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pipeline->pingpong_fbo[i]);
        glBindTexture(GL_TEXTURE_2D, pipeline->pingpong_colorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pipeline->pingpong_colorbuffers[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Retour à l'écran normal
}

void render_pipeline_draw(render_pipeline_t* pipeline, ecs_registry_t* registry, renderer_t* renderer, environment_t* env, camera_component_t* cam, entity_t cam_entity_id) {
    // ==========================================
    // ETAPE 0 : Chercher la camera active
    // ==========================================
    cam = ecs_get_component(registry, cam_entity_id, COMP_CAMERA);
    transform_t* cam_t  = (transform_t*)ecs_get_component(registry, cam_entity_id, COMP_TRANSFORM);

    if (!cam) return; // Pas de camera, on ne dessine rien !

    //donne les matrices de vue et de projection au renderer
    renderer_begin_scene(renderer, &cam->view_matrix, &cam->projection_matrix);

    // ==========================================
    // ETAPE 1 : RENDU DE LA SCENE (Dans HDR FBO)
    // ==========================================
    glBindFramebuffer(GL_FRAMEBUFFER, pipeline->hdr_fbo);
    glViewport(0, 0, pipeline->width, pipeline->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1.1 Entités PBR
    shader_bind(&pipeline->pbr_shader);
    shader_set_uniform_vec3(&pipeline->pbr_shader, "u_viewPos", cam_t->position);
    s_render_update(registry, renderer, &pipeline->pbr_shader, &env->irradiance_map, &env->prefilter_map, &env->brdf_lut);

    // Skybox
    s_skybox_render(&env->env_cubemap, cam, &pipeline->skybox_shader);

    // Nuages
    vec3 sun_dir = {0.5f, 1.0f, 0.5f}; // On donne la même direction que ton soleil dans test_game.c
    s_clouds_render(registry, renderer, &pipeline->clouds_shader, cam, sun_dir);

    // Atmosphere
    s_atmosphere_render(registry, renderer, &pipeline->atmosphere_shader, cam, cam_entity_id, sun_dir);

    // ==========================================
    // ETAPE 2 : FLOU GAUSSIEN (Ping-Pong FBOs)
    // ==========================================
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10; // 10 passes = Flou très large ! (Baisse à 4 pour optimiser si besoin)
    
    shader_bind(&pipeline->blur_shader);
    shader_set_uniform_1i(&pipeline->blur_shader, "image", 0);

    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pipeline->pingpong_fbo[horizontal]);
        shader_set_uniform_1i(&pipeline->blur_shader, "horizontal", horizontal);
        
        glActiveTexture(GL_TEXTURE0);
        // Au premier tour, on lit l'image brillante générée à l'Etape 1. 
        // Ensuite, on lit le FBO précédent.
        glBindTexture(GL_TEXTURE_2D, first_iteration ? pipeline->color_buffers[1] : pipeline->pingpong_colorbuffers[!horizontal]); 
        
        glBindVertexArray(pipeline->quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }


    // ==========================================
    // ETAPE 3 : COMBINAISON ET TONE MAPPING (Ecran)
    // ==========================================
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // FBO 0 = l'écran physique
    glViewport(0, 0, pipeline->width, pipeline->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_bind(&pipeline->postprocess_shader);
    
    // Texture 0 = La scène normale
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pipeline->color_buffers[0]);
    shader_set_uniform_1i(&pipeline->postprocess_shader, "scene", 0);
    
    // Texture 1 = Le flou
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pipeline->pingpong_colorbuffers[!horizontal]);
    shader_set_uniform_1i(&pipeline->postprocess_shader, "bloomBlur", 1);
    
    // Tu peux changer l'exposition globale de ton jeu ici !
    shader_set_uniform_1f(&pipeline->postprocess_shader, "exposure", 1.0f);

    glBindVertexArray(pipeline->quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void render_pipeline_destroy(render_pipeline_t* pipeline) {
    shader_destroy(&pipeline->pbr_shader);
    shader_destroy(&pipeline->skybox_shader);
    shader_destroy(&pipeline->blur_shader);
    shader_destroy(&pipeline->postprocess_shader);
    
    glDeleteFramebuffers(1, &pipeline->hdr_fbo);
    glDeleteTextures(2, pipeline->color_buffers);
    glDeleteRenderbuffers(1, &pipeline->rbo_depth);
    
    glDeleteFramebuffers(2, pipeline->pingpong_fbo);
    glDeleteTextures(2, pipeline->pingpong_colorbuffers);
}

void render_pipeline_resize(render_pipeline_t* pipeline, int width, int height) {
    // Ne rien faire si la taille est invalide (minimisation de la fenêtre)
    if (width == 0 || height == 0) return; 

    pipeline->width = width;
    pipeline->height = height;

    // 1. Redimensionner les textures du FBO HDR
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, pipeline->color_buffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    // 2. Redimensionner le Depth Buffer (TRÈS IMPORTANT pour la 3D !)
    glBindRenderbuffer(GL_RENDERBUFFER, pipeline->rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

    // 3. Redimensionner les textures du Ping-Pong (Flou)
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, pipeline->pingpong_colorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
}
