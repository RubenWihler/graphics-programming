/*
*   Pour plus d'informations sur les textures, voir:
*   - https://docs.gl/gl4/glTexParameter (pour les paramètres de texture)
*   - https://docs.gl/gl4/glTexImage2D (pour les paramètres de glTexImage2D)
*
*/


#include "../../vendor/glad/glad.h"
#include "texture.h"
#include "../../log/log.h"
#include "../../vendor/stb_image/stb_image.h"

#include "../../vendor/cglm/cglm.h"
#include "../shader/shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static const int channels = 4;
static unsigned int quadVAO = 0;
static unsigned int quadVBO;
static void render_quad() {
    if (quadVAO == 0) {
        // Un rectangle qui couvre tout l'écran (Positions X/Y/Z, TexCoords U/V)
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

bool texture_init(texture_t *tex, const char *filepath)
{
    memset(tex, 0, sizeof(*tex));

    tex->filepath = strdup(filepath);
    if(!tex->filepath) return (perror("strdup"), false);

    stbi_set_flip_vertically_on_load(1);
    tex->local_buffer = stbi_load(tex->filepath, &tex->width, &tex->height, &tex->bpp, channels);


    ASSERT_GL_BEGIN();
    glGenTextures(1, &tex->renderer_id);
    glBindTexture(GL_TEXTURE_2D, tex->renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//s is like x
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//t is like y

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->width, tex->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex->local_buffer);

    glBindTexture(GL_TEXTURE_2D, 0);
    if(ASSERT_GL_ERROR_OCCURED("error while creating texture"))
    {
        texture_destroy(tex);
        return false;
    }
    
    //pour l'instant on n'a pas besoin de la texture en mémoire
    stbi_image_free(tex->local_buffer);

    return true;
}

void texture_destroy(texture_t *tex)
{
    ASSERT_GL_CALL(glDeleteTextures(1, &tex->renderer_id));
    free(tex->filepath);
    tex->renderer_id = 0;
}

void texture_bind(texture_t *tex, unsigned int* slot_ptr)
{
    unsigned int slot = (slot_ptr != NULL) ? *slot_ptr : 0;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->renderer_id);
}

void texture_unbind()
{
    ASSERT_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}


// Un petit cube en dur, nécessaire pour dessiner les 6 faces depuis l'intérieur
static unsigned int captureVAO = 0;
static unsigned int captureVBO = 0;
static void render_capture_cube() {
    if (captureVAO == 0) {
        float vertices[] = {
            // ... [Astuce: Copie ici les 36 sommets d'un cube basique 1x1x1 (X, Y, Z)] ...
            // (Pour garder la réponse concise, je te laisse insérer un VBO de cube classique avec juste les positions)
            -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
        };
        glGenVertexArrays(1, &captureVAO);
        glGenBuffers(1, &captureVBO);
        glBindVertexArray(captureVAO);
        glBindBuffer(GL_ARRAY_BUFFER, captureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
    glBindVertexArray(captureVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

bool texture_load_cubemap_from_hdr(cubemap_t* out_cubemap, const char* filepath, shader_t* conversion_shader) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    
    // 1. Charger l'image HDR (stbi_loadf au lieu de stbi_load pour les Float 32-bit !)
    float *data = stbi_loadf(filepath, &width, &height, &nrComponents, 0);
    if (!data) {
        LOG_ERROR("Echec du chargement du HDR : %s", filepath);
        return false;
    }

    // Créer la texture 2D HDR sur le GPU
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    // 2. Créer le FBO (Framebuffer)
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // 3. Créer le Cubemap vide (Taille de rendu 2048x2048 par face)
    glGenTextures(1, &out_cubemap->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_cubemap->id);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 4. Les 6 "Caméras" (Matrices View) pointant dans les 6 directions du cube
    mat4 captureProjection, captureViews[6];
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, 10.0f, captureProjection); // FOV de 90° parfait pour un cube

    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 1.0f,  0.0f,  0.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[0]); // Right
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){-1.0f,  0.0f,  0.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[1]); // Left
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  1.0f,  0.0f}, (vec3){0.0f,  0.0f,  1.0f}, captureViews[2]); // Top
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f, -1.0f,  0.0f}, (vec3){0.0f,  0.0f, -1.0f}, captureViews[3]); // Bottom
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  0.0f,  1.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[4]); // Front
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  0.0f, -1.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[5]); // Back

    // 5. Rendu (La "Photographie" des 6 faces)
    shader_bind(conversion_shader);
    shader_set_uniform_1i(conversion_shader, "u_equirectangularMap", 0);
    shader_set_uniform_mat4(conversion_shader, "u_projection", captureProjection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 2048, 2048); // La résolution de notre Cubemap
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i) {
        // On attache la face du cubemap au Framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, out_cubemap->id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_set_uniform_mat4(conversion_shader, "u_view", captureViews[i]);
        render_capture_cube();
    }
    
    // Nettoyage et retour à la normale
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteTextures(1, &hdrTexture); // On n'a plus besoin de la texture plate 2D !
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    
    out_cubemap->width = 2048;
    out_cubemap->height = 2048;

    LOG_INFO("Cubemap genere avec succes depuis %s", filepath);
    return true;
}

bool texture_create_irradiance_map(cubemap_t* out_cubemap, cubemap_t* env_map, shader_t* irrad_shader) {
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32); // 32x32 !
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &out_cubemap->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_cubemap->id);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mat4 captureProjection, captureViews[6];
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, 10.0f, captureProjection);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 1.0f,  0.0f,  0.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[0]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){-1.0f,  0.0f,  0.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[1]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  1.0f,  0.0f}, (vec3){0.0f,  0.0f,  1.0f}, captureViews[2]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f, -1.0f,  0.0f}, (vec3){0.0f,  0.0f, -1.0f}, captureViews[3]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  0.0f,  1.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[4]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  0.0f, -1.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[5]);

    shader_bind(irrad_shader);
    shader_set_uniform_1i(irrad_shader, "u_environmentMap", 0);
    shader_set_uniform_mat4(irrad_shader, "u_projection", captureProjection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_map->id); // On lit l'ancien cubemap HDR !

    glViewport(0, 0, 32, 32); 
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, out_cubemap->id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader_set_uniform_mat4(irrad_shader, "u_view", captureViews[i]);
        render_capture_cube(); // Appelle ta fonction static qui dessine un cube !
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    out_cubemap->width = 32; out_cubemap->height = 32;
    return true;
}

bool texture_create_prefilter_map(cubemap_t* out_prefilter, cubemap_t* env_map, shader_t* prefilter_shader) {
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    // 1. Allouer le Cubemap de base (128x128 suffit pour des reflets)
    glGenTextures(1, &out_prefilter->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_prefilter->id);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // ATTENTION : On utilise LINEAR_MIPMAP_LINEAR pour permettre la lecture fluide entre les niveaux de flou
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Générer les mipmaps vides
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    mat4 captureProjection, captureViews[6];
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, 10.0f, captureProjection);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 1.0f,  0.0f,  0.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[0]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){-1.0f,  0.0f,  0.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[1]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  1.0f,  0.0f}, (vec3){0.0f,  0.0f,  1.0f}, captureViews[2]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f, -1.0f,  0.0f}, (vec3){0.0f,  0.0f, -1.0f}, captureViews[3]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  0.0f,  1.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[4]);
    glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){ 0.0f,  0.0f, -1.0f}, (vec3){0.0f, -1.0f,  0.0f}, captureViews[5]);

    shader_bind(prefilter_shader);
    shader_set_uniform_1i(prefilter_shader, "u_environmentMap", 0);
    shader_set_uniform_mat4(prefilter_shader, "u_projection", captureProjection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_map->id); 

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    
    // 2. LA BOUCLE DES MIPMAPS (0 à 4)
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // La taille diminue à chaque niveau (128 -> 64 -> 32 -> 16 -> 8)
        unsigned int mipWidth  = (unsigned int)(128.0 * pow(0.5, mip));
        unsigned int mipHeight = (unsigned int)(128.0 * pow(0.5, mip));
        
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        // On envoie la rugosité au shader (0.0 pour mip 0, 1.0 pour mip 4)
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shader_set_uniform_1f(prefilter_shader, "u_roughness", roughness);
        
        for (unsigned int i = 0; i < 6; ++i)
        {
            // Note le "mip" à la fin : on dessine spécifiquement dans le niveau de mipmap ciblé !
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, out_prefilter->id, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            shader_set_uniform_mat4(prefilter_shader, "u_view", captureViews[i]);
            render_capture_cube();
        }
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    
    out_prefilter->width = 128;
    out_prefilter->height = 128;
    return true;
}

bool texture_create_brdf_lut(texture_t* out_texture, shader_t* brdf_shader) {
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    // Format RG16F (On a juste besoin de deux canaux : Rouge et Vert en float)
    glGenTextures(1, &out_texture->renderer_id);
    glBindTexture(GL_TEXTURE_2D, out_texture->renderer_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    
    // IMPORTANT : On force la texture à être lue correctement aux bords
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_texture->renderer_id, 0);

    glViewport(0, 0, 512, 512);
    shader_bind(brdf_shader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    render_quad(); // On dessine !

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);

    out_texture->width = 512;
    out_texture->height = 512;
    return true;
}
