#include "test_game.h"

#include <GLFW/glfw3.h>

#include "../gllib/vendor/cglm/types.h"
#include "../gllib/vendor/cglm/mat4.h"
#include "../gllib/vendor/cglm/cglm.h"

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
#include "../gllib/render/camera/cam_persp.h"

// #include "../gllib/addons/camera_controller/cam_ortho_controller.h"


struct _test_game_t {
    game_t game;
    test_game_config_t config;

    input_manager_t input_manager;
    renderer_t renderer;
    cam_persp_t cam;
    // cam_ortho_controller_t cam_controller;

    //objet 1 : cube
    mat4 model;
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

    //camera
    float fov = glm_rad(45.0), nearz = 0.1f, farz = 100.0f;
    int winw, winh;
    glfwGetWindowSize(game->window, &winw, &winh);
    const float aspect_ratio = (float)winw / (float)winh;

    if (!cam_persp_init(&tg->cam, fov, aspect_ratio, nearz, farz))
    {
        LOG_ERROR("camera initialization failed!", true);
        return false;
    }

    vec3 new_pos = {0.0f, 0.0f, 5.0f};
    cam_persp_set_position(&tg->cam, new_pos);

    tg->time = 0.0f;
    tg->time_incr = 0.1f;

    return true;
}


static void test_game_start(game_t *game)
{
    printf("%s\n", __func__);
    test_game_t *tg = container_of(game, test_game_t, game);

    //a modifier pour cube 3d
    const float vertex[] = {
        //face avant z=1
         -0.5f, -0.5f,  -0.5f,   // bas  gauche
          0.5f, -0.5f,  -0.5f,   // bas  droite
          0.5f,  0.5f,  -0.5f,   // haut droite
         -0.5f, 0.5f,  -0.5f,   // haut gauche

        //face arriere z=0.5
         -0.5f, -0.5f,  0.5f,   // bas  gauche
          0.5f, -0.5f,  0.5f,   // bas  droite
          0.5f,  0.5f,  0.5f,   // haut droite
         -0.5f, 0.5f,  0.5f,   // haut gauche
    };

    const unsigned int indices[] = {
        //face avant
        0, 1, 2,
        0, 3, 2,

        //face arriere
        4, 5, 6,
        4, 7, 6,

        //face haut
        3, 2, 6,
        3, 7, 6,

        //face bas
        0, 1, 5,
        0, 4, 5,

        //face gauche
        0, 4, 7,
        0, 3, 7,

        //face droite
        1, 5, 6,
        1, 2, 6
    };

    vertex_array_init(&tg->vao);
    vertex_buffer_init(&tg->vbo, vertex, sizeof(vertex), false);

    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 3);//pos
    vertex_array_add_buffer(&tg->vao, &tg->vbo, &layout);

    int index_count = sizeof(indices) / sizeof(indices[0]);
    index_buffer_init(&tg->ibo, indices, index_count, false);
    index_buffer_bind(&tg->ibo);

    // texture_init(&tg->texture, "res/textures/c_logo.png");
    // texture_bind(&tg->texture, &(uint){0});
    shader_init(&tg->shader, "res/shaders/default");
    shader_bind(&tg->shader);

    glm_mat4_identity(tg->model);
    shader_set_uniform(&tg->shader, "u_model", tg->model);

    #define SHINYING_COLOR 1.0f, 1.0f, 1.0f, 1.0f
    // shader_set_uniform(&tg->shader, "u_time", 0.0);
    shader_set_uniform(&tg->shader, "u_color", (vec4){ SHINYING_COLOR});
    // shader_set_uniform(&tg->shader, "u_texture", 0);
    // shader_set_uniform(&tg->shader, "u_time", 0.0);
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
    (void)delta_time;
    test_game_t *tg = container_of(game, test_game_t, game);

    //a changer pour un nouveau cam_persp_controller
    // cam_ortho_controller_update(&tg->cam_controller, delta_time);

    // Définir la vitesse de rotation (ex: 1 radian par seconde)
    float rotation_speed = 1.0f;
    vec3 rotation_axis = {1.0f, 1.0f, 0.0f};
    glm_vec3_normalize(rotation_axis);
    glm_rotate(tg->model, rotation_speed * delta_time, rotation_axis);

    //modifie la valeur temps du shader
    if(tg->time > 2 * M_PI) tg->time = 0;
    tg->time += tg->time_incr;
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);
    renderer_begin_scene(&tg->renderer, &tg->cam);

    shader_bind(&tg->shader);
    shader_set_uniform(&tg->shader, "u_model", tg->model);
    //mat view_proj update dans renderer_draw()
    renderer_draw(&tg->renderer, &tg->vao, &tg->ibo, &tg->shader);

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
    // cam_ortho_controller_destroy(&tg->cam_controller);
}


static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);
    glViewport(0, 0, width, height);

    float aspect_ratio = (float)width / (float)height;
    cam_persp_set_aspect_ratio(&tg->cam, aspect_ratio);
    // cam_ortho_controller_resize(&tg->cam_controller, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)yoffset;
    (void)xoffset;
    // test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);
    // cam_ortho_controller_zoom(&tg->cam_controller, yoffset);
}
