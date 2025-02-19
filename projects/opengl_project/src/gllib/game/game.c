#include "game.h"
#include "../log/log.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static bool gl_init(game_t* game);
static bool callback_init(game_t* game);
static void loop(game_t* game);

int game_init(game_t *game, game_config_t config, game_init_fn init_fn){
    assert(game != NULL);
    memset(game, 0, sizeof(game_t));

    game->config = config;
    game->is_running = false;
    game->state = GAME_STATE_INIT;

    //initialisation de SDL
    if(!gl_init(game))
    {
        LOG_ERROR("game initialization failed!", true);
        game_clean(game);
        return false;
    }
    
    //initialisation du jeu "enfant"
    if(!init_fn(game))
    {
        LOG_ERROR("child game initialization failed!", true);
        game_clean(game);
        return false;
    }

    //initialisation des callbacks
    //on doit initialiser les callbacks après l'initialisation du jeu enfant
    //car c'est dans l'initialisation du jeu enfant que l'on définit les callbacks
    if(!callback_init(game))
    {
        LOG_ERROR("callback initialization failed!", true);
        game_clean(game);
        return false;
    }

    return true;
}

void game_clean(game_t *game){
    assert(game != NULL);

    game->api.on_clean(game);

    if(game->window)
    {
        glfwDestroyWindow(game->window);
        game->window = NULL;
    }

    glfwTerminate();
}

void game_start(game_t *game){
    assert(game != NULL);

    if(game->state == GAME_STATE_INIT)
    {
        game->api.on_start(game);
        game->is_running = true;
        game->state = GAME_STATE_RUNNING;
        loop(game);
    }
}

void game_stop(game_t *game){
    assert(game != NULL);

    if(game->state == GAME_STATE_RUNNING || game->state == GAME_STATE_PAUSED)
    {
        game->api.on_stop(game);
        game->is_running = false;
        game->state = GAME_STATE_STOPPED;
    }
}

void game_pause(game_t *game){
    assert(game != NULL);

    if(game->state == GAME_STATE_RUNNING)
    {
        game->api.on_pause(game);
        game->state = GAME_STATE_PAUSED;
    }
}

void game_play(game_t *game){
    assert(game != NULL);

    if(game->state == GAME_STATE_PAUSED)
    {
        game->api.on_play(game);
        game->state = GAME_STATE_RUNNING;
    }
}

static bool gl_init(game_t* game)
{
    //Init GLFW notre librairie pour gérer les fenêtres et les événements
    if (!glfwInit())
    {
        LOG_ERROR("Error while init GLFW!", true);
        return false;
    }
    
    // Configuration de la version d'OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, game->config.api_version.glfw_context_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, game->config.api_version.glfw_context_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, game->config.api_version.opengl_profile);

    game->window = glfwCreateWindow(game->config.width, game->config.height, 
                                    game->config.window_name, NULL, NULL);
    if (!game->window)
    {
        LOG_ERROR("Error while creating window!", true);
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(game->window);
    
    // Initialisation de GLEW (pour avoir acces aux fonctions OpenGL)
    // GLEW permet de charger les fonctions OpenGL a partir du driver de la carte graphique
    if (glewInit() != GLEW_OK)
    {
        char errmsg[512];
        sprintf(errmsg, "Error while init GLEW: %s\n", glewGetErrorString(glewInit()));
        LOG_ERROR(errmsg, true);
        return false;
    }
    
    //defini le pointeur de la fenêtre pour les callback
    //on peut ainsi utiliser glfwGetWindowUserPointer pour récupérer le pointeur du jeu
    glfwSetWindowUserPointer(game->window, (void*)game);

    //limitation de la framerate
    if(game->config.fps > 0)
    {
        glfwSwapInterval(1.0f / game->config.fps);
    }
    else
    {
        //si fps = 0, on regarde si la vsync est activé
        glfwSwapInterval((game->config.vsync) ? 1 : 0);
    }
    
    //initialisation du blending
    if(game->config.blending.enabled)
    {
        glBlendFunc(game->config.blending.src, game->config.blending.dst);
        glEnable(GL_BLEND);
    }

    return true;
}

static void loop(game_t* game)
{
    double last_time = glfwGetTime();
    double delta_time = 0.0;

    while(game->is_running)
    {
        //calcul du delta time
        double current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        //clear le buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //appel des fonctions de l'api
        game->api.on_update(game, delta_time);
        game->api.on_render(game);

        //si une limite de framerate est définie, on attend
        glfwSwapBuffers(game->window);
        glfwPollEvents();

        //si la fenêtre est fermée, on arrête le jeu
        if(glfwWindowShouldClose(game->window))
        {
            game->is_running = false;
        }
    }
    
    //on arrête le jeu
    //il est possible que le jeu soit arrêté par un appel à game_stop
    //mais ce c'est pas grave car on verifie l'etat dans game_stop
    game_stop(game);

}

static bool callback_init(game_t* game)
{
    //callback pour le redimensionnement de la fenêtre
    if(game->api.glfw_callbacks.framebuffer_size_callback)
        glfwSetFramebufferSizeCallback(game->window, game->api.glfw_callbacks.framebuffer_size_callback);
    
    //callback pour les touches
    if(game->api.glfw_callbacks.key_callback)
        glfwSetKeyCallback(game->window, game->api.glfw_callbacks.key_callback);

    //callback pour les boutons de la souris
    if(game->api.glfw_callbacks.mouse_button_callback)
        glfwSetMouseButtonCallback(game->window, game->api.glfw_callbacks.mouse_button_callback);

    //callback pour la position du curseur
    if(game->api.glfw_callbacks.cursor_position_callback)
        glfwSetCursorPosCallback(game->window, game->api.glfw_callbacks.cursor_position_callback);

    //callback pour la molette de la souris
    if(game->api.glfw_callbacks.scroll_callback)
        glfwSetScrollCallback(game->window, game->api.glfw_callbacks.scroll_callback);

    return true;
}
