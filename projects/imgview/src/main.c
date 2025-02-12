// Description: Main file for the project.
// Author: Wihler Ruben

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vendor/cglm/cam.h"
#include "vendor/cglm/cglm.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "config/app_config.h"

#include "render/vertex_buffer/vertex_buffer.h"
#include "render/index_buffer/index_buffer.h"
#include "render/vertex_array/vertex_array.h"
#include "render/shader/shader.h"
#include "render/texture/texture.h"
#include "render/renderer/renderer.h"

static int init(app_config_t config, GLFWwindow** window);
static bool parse_args(int argc, char **argv, app_config_t *config);
static void print_version();
static void print_help(const char *name);
static char** get_available_shaders(const char *shader_dir_path);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

extern app_config_t APP_DEFAULT_CONFIG;

static float zoom = 1.0f;
static mat4 *proj_ptr = NULL;
static shader_t *shader_ptr = NULL;

int main(int argc, char **argv)
{
    GLFWwindow* window = NULL;

    app_config_t config = APP_DEFAULT_CONFIG;
    if(!parse_args(argc, argv, &config)) return EXIT_FAILURE;

    if(config.mode == APP_MODE_HELP)
    {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }
    else if(config.mode == APP_MODE_VERSION)
    {
        if (init(APP_DEFAULT_CONFIG, &window)) return EXIT_FAILURE;
        print_version();
        glfwTerminate();
        return EXIT_SUCCESS;
    }
    else if(config.mode == APP_MODE_LIST_SHADERS)
    {
        char **shaders = get_available_shaders("res/shaders");
        if(!shaders) return EXIT_FAILURE;

        for(int i = 0; shaders[i] != NULL; i++)
        {
            printf("%s\n", shaders[i]);
            //free(shaders[i]); //en commentaire car on a hardcoder les noms des shaders
        }
        
        free(shaders);
        return EXIT_SUCCESS;
    }
    
    //On va concaténer le chemin du shader avec le dossier des shaders
    char *shader_path = malloc(strlen("res/shaders/") + strlen(config.shader_name) + 1);
    if(!shader_path) return (perror("malloc"), EXIT_FAILURE);
    strcpy(shader_path, "res/shaders/");
    strcat(shader_path, config.shader_name);

    //Initialisation
    if (init(APP_DEFAULT_CONFIG, &window)) return EXIT_FAILURE;
        
    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(-zoom, zoom, -zoom, zoom, -1.0f, 1.0f, proj);
    proj_ptr = &proj;

    //un vertex par ligne
    //1er  attribut: 2 float - position (xy)
    //2eme attribut: 2 float - texture coordinate (st)
    const float vertex[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, // vertex 0
         1.0f, -1.0f, 1.0f, 0.0f, // vertex 1
         1.0f,  1.0f, 1.0f, 1.0f, // vertex 2
        -1.0f,  1.0f, 0.0f, 1.0f, // vertex 3
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

    shader_t shader;
    shader_ptr = &shader;
    shader_init(&shader, shader_path);
    shader_bind(&shader);
    shader_set_uniform_mat4(&shader, "u_mvp", proj);
    shader_set_uniform_4f(&shader, "u_color", 0.35f, 0.2f, 0.6f, 0);
    shader_set_uniform_1f(&shader, "u_time", 0);

    texture_t texture;
    texture_init(&texture, config.image_path);
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

        glfwSwapBuffers(window);
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
    glfwSetScrollCallback(*window, &scroll_callback);

    return 0;
}

void print_version()
{
    printf("Version: %s\n", APP_DEFAULT_CONFIG.version);
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLFW version: %s\n", glfwGetVersionString());
    printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));
}

static void print_help(const char *name)
{
    printf("Usage: %s [OPTION]\n", name);
    printf("Options:\n");
    printf("  -h, --help\t\t\tDisplay this help and exit\n");
    printf("  -v, --version\t\t\tDisplay version information and exit\n");
    printf("  -p, --path\t\t\tPath to the image\n");
    printf("  -s, --shader\t\t\tName of the shader\n");
    printf("  -ls, --list-shaders\t\tList all available shaders\n");
}

static bool parse_args(int argc, char **argv, app_config_t *config)
{
    if(argc == 1)
    {
        fprintf(stderr, "Error: missing argument\n");
        return false;
    }

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            config->mode = APP_MODE_HELP;
            return true;
        }
        else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            config->mode = APP_MODE_VERSION;
            return true;
        }
        else if(strcmp(argv[i], "-ls") == 0 || strcmp(argv[i], "--list-shaders") == 0)
        {
            config->mode = APP_MODE_LIST_SHADERS;
            return true;
        }
        else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0)
        {
            if(i + 1 < argc)
            {
                config->image_path = argv[i + 1];
                i++;
            }
            else
            {
                fprintf(stderr, "Error: missing argument for option %s\n", argv[i]);
                return false;
            }
        }
        else if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--shader") == 0)
        {
            if(i + 1 < argc)
            {
                config->shader_name = argv[i + 1];
                i++;
            }
            else
            {
                fprintf(stderr, "Error: missing argument for option %s\n", argv[i]);
                return false;
            }
        }
        else
        {
            fprintf(stderr, "Error: unknown option %s\n", argv[i]);
            return false;
        }
    }

    return true;
}

static char** get_available_shaders(const char *shader_dir_path)
{
    //scan directory for all subdirectories
    //return a list of all subdirectories names
    (void)shader_dir_path;

    //pour l'instant on va juste hardcoder les noms des shaders
    char **shaders = malloc(4 * sizeof(char*));
    if(!shaders) return (perror("malloc"), NULL);

    shaders[0] = "default";
    shaders[1] = "shiny";
    shaders[2] = "invert";
    shaders[3] = NULL;
    
    return shaders;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    
    //On veut que notre image ne soit pas déformée
    //pour cela on va ajouter des bordures noires
    int mr, ml, mt, mb;//margin

    if(width > height)
    {
        mr = (width - height) / 2;
        ml = mr;
        mt = 0;
        mb = 0;
    }
    else
    {
        mt = (height - width) / 2;
        mb = mt;
        ml = 0;
        mr = 0;
    }

    /* if(zoom < 1.0f) */
    /* { */
    /*     mr = mr * zoom / 2; */
    /*     ml = ml * zoom / 2; */
    /*     mt = mt * zoom / 2; */
    /*     mb = mb * zoom / 2; */
    /* } */

    glViewport(ml, mt, width - mr - ml, height - mb - mt);
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;

    zoom -= yoffset * 0.1f;
    if(zoom < 0.1f) zoom = 0.1f;
    if(zoom > 10.0f) zoom = 10.0f;

    glm_ortho(-zoom, zoom, -zoom, zoom, -1.0f, 1.0f, *proj_ptr);
    shader_set_uniform_mat4(shader_ptr, "u_mvp", *proj_ptr);

    //On recalcule la taille de la fenêtre
    /* int ww, wh; */
    /* glfwGetWindowSize(window, &ww, &wh); */
    /* framebuffer_size_callback(window, ww, wh); */
}
