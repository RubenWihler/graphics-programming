#include "test_game.h"

#include <GLFW/glfw3.h>

#include "../gllib/vendor/cglm/types.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../gllib/log/log.h"
#include "../gllib/utils/common.h"
#include "../gllib/game/game.h"
#include "../gllib/inputs/input_manager.h"
#include "../gllib/render/vertex_buffer/vertex_buffer.h"
#include "../gllib/render/index_buffer/index_buffer.h"
#include "../gllib/render/vertex_array/vertex_array.h"
#include "../gllib/render/shader/shader.h"
#include "../gllib/render/texture/texture.h"
#include "../gllib/render/renderer/renderer.h"

#include "../gllib/addons/camera_controller/cam_ortho_controller.h"
#include "../gllib/addons/background/grid_background.h"

#include "../gllib/addons/particle_system/particle_pool.h"


struct _test_game_t {
    game_t game;
    test_game_config_t config;

    input_manager_t input_manager;
    renderer_t renderer;
    cam_ortho_controller_t cam_controller;
    grid_background_t grid;

    particle_pool_t particle_pool;
    shader_t particle_shader;

    vertex_array_t vao;
    vertex_buffer_t vbo;
    index_buffer_t ibo;
    texture_t texture;
    shader_t shader;
    float time;
    float time_incr;

    //mat4 *model;//car que 1 obj pour l'instant
    vec2 last_emission_pos;
    bool first_click;
};

static bool test_game_init(game_t *game);
static void test_game_start(game_t *game);
static void test_game_stop(game_t *game);
static void test_game_pause(game_t *game);
static void test_game_play(game_t *game);
static void test_game_update(game_t *game, float delta_time);
static void test_game_render(game_t *game);
static void test_game_clean(game_t *game);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

game_t* test_game_create(test_game_config_t* config)
{
    test_game_t *test_game = (test_game_t*)calloc(1, sizeof(test_game_t));
    if(!test_game) return (perror("malloc"), NULL);

    test_game->config = *config;

    if (!game_init(&test_game->game, &config->game_config, test_game_init))
    {
        LOG_ERROR("game initialization failed!", true);
        free(test_game);
        return NULL;
    }

    return (game_t*)test_game;
}

void test_game_destroy(game_t *game)
{
    test_game_t *test_game = container_of(game, test_game_t, game);
    game_clean(&test_game->game);
    free(test_game);
}

static bool test_game_init(game_t *game)
{
    //game api
    game->api.on_start = test_game_start;
    game->api.on_stop = test_game_stop;
    game->api.on_pause = test_game_pause;
    game->api.on_play = test_game_play;
    game->api.on_update = test_game_update;
    game->api.on_render = test_game_render;
    game->api.on_clean = test_game_clean;

    //glfw callbacks
    game->api.glfw_callbacks.framebuffer_size_callback = framebuffer_size_callback;
    game->api.glfw_callbacks.key_callback = NULL;
    game->api.glfw_callbacks.mouse_button_callback = NULL;
    game->api.glfw_callbacks.cursor_position_callback = NULL;
    game->api.glfw_callbacks.scroll_callback = scroll_callback;

    test_game_t *tg = container_of(game, test_game_t, game);

    //input manager
    if(!input_manager_init(&tg->input_manager, game->window))
    {
        LOG_ERROR("input manager initialization failed!", true);
        return false;
    }
    
    //renderer
    if(!renderer_init(&tg->renderer, 0))
    {
        LOG_ERROR("renderer initialization failed!", true);
        return false;
    }

    //camera controller
    int winw, winh;
    glfwGetWindowSize(game->window, &winw, &winh);
    const float aspect_ratio = (float)winw / (float)winh;
    cam_ortho_controller_config_t cam_controller_config = CAM_ORTHO_CONTROLLER_DEFAULT_CONFIG;
    cam_controller_config.enable_rotation = true;
    if(!cam_ortho_controller_init(&tg->cam_controller, &tg->input_manager, game->window, cam_controller_config, aspect_ratio))
    {
        LOG_ERROR("camera controller initialization failed!", true);
        return false;
    }

    //grid background
    if(!grid_background_init(&tg->grid))
    {
        LOG_ERROR("grid background initialization failed!", true);
        return false;
    }

    //particle pool
    if(!particle_pool_init(&tg->particle_pool, tg->config.particle.particle_props, 
                           tg->config.particle.particle_capacity))
    {
        LOG_ERROR("particle pool initialization failed!", true);
        return false;
    }

    tg->time = 0.0f;
    tg->time_incr = 0.1f;

    return true;
}


static void test_game_start(game_t *game)
{
    printf("%s\n", __func__);
    test_game_t *tg = container_of(game, test_game_t, game);

    const float vertex[] = {
    // |     pos    |    tex    |
        5.0f, 5.0f, 0.0f, 0.0f, // vertex 0
        10.0f, 5.0f, 1.0f, 0.0f, // vertex 1
        10.0f, 10.0f, 1.0f, 1.0f, // vertex 2
        5.0f, 10.0f, 0.0f, 1.0f, // vertex 3

        20.0f, 20.0f, 0.0f, 0.0f, // vertex 0
        30.0f, 20.0f, 1.0f, 0.0f, // vertex 1
        30.0f, 30.0f, 1.0f, 1.0f, // vertex 2
        20.0f, 30.0f, 0.0f, 1.0f, // vertex 3
    };

    const unsigned int indices[] = {
        0, 1, 2, //1er  triangle
        0, 2, 3, //2eme triangle

        4, 5, 6, //1er  triangle
        4, 6, 7, //2eme triangle
    };

    vertex_array_init(&tg->vao);
    vertex_buffer_init(&tg->vbo, vertex, sizeof(vertex), false);

    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 2);//pos
    vertex_buffer_layout_push_float(&layout, 2);//tex
    vertex_array_add_buffer(&tg->vao, &tg->vbo, &layout);

    int index_count = sizeof(indices) / sizeof(indices[0]);
    index_buffer_init(&tg->ibo, indices, index_count, false);
    index_buffer_bind(&tg->ibo);

    texture_init(&tg->texture, "res/textures/c_logo.png");
    texture_bind(&tg->texture, &(uint){0});
    shader_init(&tg->shader, "res/shaders/shiny_2d_tex");
    shader_bind(&tg->shader);

    #define SHINYING_COLOR 0.35f, 0.2f, 0.6f, 0.0f
    shader_set_uniform(&tg->shader, "u_texture", 0);
    shader_set_uniform(&tg->shader, "u_color", (vec4){ SHINYING_COLOR});
    shader_set_uniform(&tg->shader, "u_time", 0.0);

    //particle shader
    shader_init(&tg->particle_shader, "res/shaders/particle/flat");
    shader_bind(&tg->particle_shader);
}

static void test_game_stop(game_t *game)
{
    (void)game;
    printf("%s\n", __func__);
}

static void test_game_pause(game_t *game)
{
    (void)game;
    printf("%s\n", __func__);
}

static void test_game_play(game_t *game)
{
    (void)game;
    printf("%s\n", __func__);
}

static void test_game_update(game_t *game, float delta_time)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    particle_pool_update(&tg->particle_pool, delta_time);
    cam_ortho_controller_update(&tg->cam_controller, delta_time);

    if(input_manager_is_mouse_pressed(&tg->input_manager, GLFW_MOUSE_BUTTON_1))
    {
        vec2 pos = GLM_VEC2_ZERO_INIT;
        input_manager_get_mouse_pos(&tg->input_manager, pos);
        cam_ortho_controller_screen_to_world(&tg->cam_controller, pos, pos);

        if(!tg->first_click)
        {
            glm_vec2_copy(pos, tg->last_emission_pos);
            tg->first_click = true;
        }
        else {
            // Calcul de la distance parcourue depuis la dernière frame
            float distance = glm_vec2_distance(tg->last_emission_pos, pos);

            // Plus ce chiffre est petit, plus la ligne est dense.
            float spacing = 0.01f;

            // Si on a bougé suffisamment pour insérer des particules
            if (distance > spacing)
            {
                // Vecteur direction normalisé
                vec2 direction;
                glm_vec2_sub(pos, tg->last_emission_pos, direction);
                glm_vec2_normalize(direction);

                // Combien de particules on peut mettre dans cet espace ?
                int count = (int)(distance / spacing);

                vec2 spawn_pos;
                for(int i = 1; i <= count; i++)
                {
                    // On avance pas à pas de 'spacing' le long de la ligne
                    glm_vec2_scale(direction, i * spacing, spawn_pos);
                    glm_vec2_add(tg->last_emission_pos, spawn_pos, spawn_pos);

                    particle_pool_emit(&tg->particle_pool, spawn_pos);
                }

                glm_vec2_copy(pos, tg->last_emission_pos);
            }
        }
        // for(int i = 0; i < tg->config.particle.particle_emmision_rate; i++)
        // {
        //     //interpolation de la position de la souris
        //     vec2 pos_interpolated = GLM_VEC2_ZERO_INIT;
        //     float t = (float)(i+1) / tg->config.particle.particle_emmision_rate;
        //     glm_vec2_lerp(tg->last_emission_pos, pos, t, pos_interpolated);
        //
        //     particle_pool_emit(&tg->particle_pool, pos_interpolated);
        // }
        //
        // tg->last_emission_pos[0] = pos[0];
        // tg->last_emission_pos[1] = pos[1];
    }
    else
    {
        tg->first_click = false;
    }

    //modifie la valeur temps du shader
    if(tg->time > 2 * M_PI) tg->time = 0;
    tg->time += tg->time_incr;
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);
    
    renderer_begin_scene(&tg->renderer, &tg->cam_controller.cam);

    grid_background_render(&tg->grid, &tg->cam_controller.cam, &tg->renderer);
    
    shader_bind(&tg->shader);
    shader_set_uniform_1f(&tg->shader, "u_time", tg->time);
    renderer_draw(&tg->renderer, &tg->vao, &tg->ibo, &tg->shader);

    particle_pool_render(&tg->particle_pool, &tg->particle_shader, &tg->renderer);

    renderer_end_scene(&tg->renderer);
}

static void test_game_clean(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    shader_destroy(&tg->shader);
    texture_destroy(&tg->texture);
    index_buffer_destroy(&tg->ibo);
    vertex_buffer_destroy(&tg->vbo);
    vertex_array_destroy(&tg->vao);

    renderer_destroy(&tg->renderer);
    cam_ortho_controller_destroy(&tg->cam_controller);
}


static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);
    glViewport(0, 0, width, height);

    cam_ortho_controller_resize(&tg->cam_controller, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)xoffset;
    test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);
    cam_ortho_controller_zoom(&tg->cam_controller, yoffset);
}
