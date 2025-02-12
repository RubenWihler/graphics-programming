// Description: Main file for the project.
// Author: Wihler Ruben

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vendor/cglm/cam.h"
#include "vendor/cglm/cglm.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "config/app_config.h"
#include "utils/crypto/math_utils.h"

#include "render/vertex_buffer/vertex_buffer.h"
#include "render/index_buffer/index_buffer.h"
#include "render/vertex_array/vertex_array.h"
#include "render/shader/shader.h"
#include "render/texture/texture.h"
#include "render/renderer/renderer.h"

static int init(app_config_t config, GLFWwindow** window);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void print_version();

extern app_config_t APP_CONFIG;

shader_t *tmp_shader = NULL;

int main(void)
{
    GLFWwindow* window = NULL;
    
    //Initialisation
    if (init(APP_CONFIG, &window)) return EXIT_FAILURE;
    
    //Affichage des versions
    print_version();
        
    //un vertex par ligne
    //1er  attribut: 2 float - position (xy)
    //2eme attribut: 2 float - texture coordinate (st)
    const float vertex[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, // vertex 0
         0.5f, -0.5f, 1.0f, 0.0f, // vertex 1
         0.5f,  0.5f, 1.0f, 1.0f, // vertex 2
        -0.5f,  0.5f, 0.0f, 1.0f, // vertex 3
    };

    const unsigned int indices[] = {
        0, 1, 2, //1er  triangle
        0, 2, 3, //2eme triangle
    };

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //Création d'un vertex array object
    vertex_array_t vao;
    vertex_array_init(&vao);

    //Création d'un vertex buffer object
    vertex_buffer_t vbo;
    vertex_buffer_init(&vbo, vertex, 4 * 4 * sizeof(float));

    //Création d'un vertex buffer layout
    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 2);//position
    vertex_buffer_layout_push_float(&layout, 2);//texture coord

    vertex_array_add_buffer(&vao, &vbo, &layout);

    ///Création d'un index buffer object
    index_buffer_t ibo;
    index_buffer_init(&ibo, indices, 2 * 3);

    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(-4.0f, 4.0f, -4.5f, 4.5f, -1.0f, 1.0f, proj);

    //Chargement des shaders
    shader_t shader;
    shader_init(&shader, "res/shaders/shiny_image");
    shader_bind(&shader);
    shader_set_uniform_4f(&shader, "u_color", 0.35f, 0.2f, 0.6f, 0);
    shader_set_uniform_1f(&shader, "u_time", 0);
    shader_set_uniform_mat4(&shader, "u_mvp", proj);
    tmp_shader = &shader;

    texture_t texture;
    texture_init(&texture, "res/textures/c_logo.png");
    /* texture_init(&texture, "res/textures/img.png"); */
    /* texture_init(&texture, "res/textures/lucky_ring.png"); */
    texture_bind(&texture, &(uint){0});//on pourrais remplacer par NULL
    shader_set_uniform_1i(&shader, "u_texture", 0);

    //debind tout pour simuler une utilisation de plusieurs buffers
    shader_unbind(&shader);
    vertex_array_unbind(&vao);
    vertex_buffer_unbind(&vbo);
    index_buffer_unbind(&ibo);

    //declaration des variables pour l'animation
    float time = 0.0f;
    float time_incr = 0.1f;

    //Temps que la fenêtre n'est pas fermée
    while (!glfwWindowShouldClose(window))
    {   
        //On efface le buffer de couleur
        //(en gros on dessine un fond noir)
        glClear(GL_COLOR_BUFFER_BIT);

        //shaders
        shader_bind(&shader);
        shader_set_uniform_1f(&shader, "u_time", time);

        renderer_draw(NULL, &vao, &ibo, &shader);

        //On change le temps pour l'animation
        if(time > 2 * M_PI) time = 0;
        time += time_incr;

        //On échange les buffers
        //C'est à dire qu'on affiche le buffer ou l'on a dessiné
        //et on met en attente le buffer ou on va dessiner
        //si la vsync est activée, on attend la fin du rafraichissement de l'écran
        glfwSwapBuffers(window);

        //On gère les événements
        glfwPollEvents();
    }

    //On libère la mémoire
    shader_destroy(&shader);
    vertex_buffer_destroy(&vbo);
    index_buffer_destroy(&ibo);
    vertex_array_destroy(&vao);
    glfwTerminate();

    return 0;
}

int init(app_config_t config, GLFWwindow** window)
{
    //Init GLFW notre librairie pour gérer les fenêtres et les événements
    if (!glfwInit())
    {
        fprintf(stderr,"Error while init GLFW\n");
        return -1;
    }
    
    // Configuration de la version d'OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.glfw_context_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.glfw_context_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, config.opengl_profile);

    // Creation de la fenêtre
    *window = glfwCreateWindow(config.width, config.height, config.window_name, NULL, NULL);
    if (!(*window))
    {
        glfwTerminate();
        return -1;
    }

    // On rend le contexte courant
    glfwMakeContextCurrent(*window);
    
    //activer la vsync
    if(config.vsync) glfwSwapInterval(1);

    // Initialisation de GLEW (pour avoir acces aux fonctions OpenGL)
    // GLEW permet de charger les fonctions OpenGL a partir du driver de la carte graphique
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Error while init GLEW: %s\n", glewGetErrorString(glewInit()));
        return -1;
    }

    glfwSetFramebufferSizeCallback(*window, &framebuffer_size_callback);

    return 0;
}

void print_version()
{
    #ifdef DEBUG
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLFW version: %s\n", glfwGetVersionString());
    printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));
    #endif
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    
    if(tmp_shader == NULL) return;

    /*
        * On veut que l'image soit toujours de la même taille
        * Pour cela on va calculer le ratio de l'image
        * et on va adapter la taille de l'image en fonction de la taille de la fenêtre
    */
    
    float ratio = (float)width / (float)height;
    float sw = 4.0f;
    float sh = sw / ratio;

    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(-sw, sw, -sh, sh, -1.0f, 1.0f, proj);
    shader_set_uniform_mat4(tmp_shader, "u_mvp", proj);
}
