#include "test_game.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../gllib/vendor/cglm/cam.h"
#include "../gllib/vendor/cglm/cglm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../gllib/log/log.h"
#include "../gllib/game/game.h"
#include "../gllib/render/camera/cam_ortho.h"
#include "../gllib/inputs/input_manager.h"
#include "../gllib/render/vertex_buffer/vertex_buffer.h"
#include "../gllib/render/index_buffer/index_buffer.h"
#include "../gllib/render/vertex_array/vertex_array.h"
#include "../gllib/render/shader/shader.h"
#include "../gllib/render/texture/texture.h"
#include "../gllib/render/renderer/renderer.h"

struct _test_game_t {
    game_t game;
    test_game_config_t config;

    input_manager_t input_manager;
    renderer_t renderer;
    cam_ortho_t cam;

    vertex_array_t vao;
    vertex_buffer_t vbo;
    index_buffer_t ibo;
    texture_t texture;
    shader_t shader;
    float time;
    float time_incr;

    //mat4 *model;//car que 1 obj pour l'instant
};

static bool test_game_init(game_t *game, game_api_t api);
static void test_game_start(game_t *game);
static void test_game_stop(game_t *game);
static void test_game_pause(game_t *game);
static void test_game_play(game_t *game);
static void test_game_update(game_t *game, float delta_time);
static void test_game_render(game_t *game);
static void test_game_clean(game_t *game);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void update_camera_position(cam_ortho_t *cam, input_manager_t *input_manager, float delta_time);

game_t* test_game_create(test_game_config_t config)
{
    test_game_t *test_game = (test_game_t*)malloc(sizeof(test_game_t));
    if(!test_game) return (perror("malloc"), NULL);

    test_game->config = config;

    if (!game_init(&test_game->game, config.game_config, test_game_init))
    {
        LOG_ERROR("game initialization failed!", true);
        free(test_game);
        return NULL;
    }

    return (game_t*)test_game;
}

void test_game_destroy(game_t *game)
{
    test_game_t *test_game = (test_game_t*)game;
    game_clean(&test_game->game);
    free(test_game);
}

static bool test_game_init(game_t *game, game_api_t api)
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
    game->api.glfw_callbacks.scroll_callback = NULL;

    test_game_t *tg = (test_game_t*)game;

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
    int winw, winh;
    glfwGetWindowSize(game->window, &winw, &winh);
    if (!cam_ortho_init(&tg->cam, 0.0f, winw, 0.0f, winh))
    {
        LOG_ERROR("camera initialization failed!", true);
        return false;
    }

    tg->time = 0.0f;
    tg->time_incr = 0.1f;

    return true;
}


static void test_game_start(game_t *game)
{
    printf("%s\n", __func__);
    test_game_t *tg = (test_game_t*)game;

    /* const float vertex[] = { */
    /* // |     pos    |    tex    | */
    /*     -0.5f, -0.5f, 0.0f, 0.0f, // vertex 0 */
    /*      0.5f, -0.5f, 1.0f, 0.0f, // vertex 1 */
    /*      0.5f,  0.5f, 1.0f, 1.0f, // vertex 2 */
    /*     -0.5f,  0.5f, 0.0f, 1.0f, // vertex 3 */
    /* }; */

    const float vertex[] = {
    // |     pos    |    tex    |
        100.0f, 100.0f, 0.0f, 0.0f, // vertex 0
        200.0f, 100.0f, 1.0f, 0.0f, // vertex 1
        200.0f, 200.0f, 1.0f, 1.0f, // vertex 2
        100.0f, 200.0f, 0.0f, 1.0f, // vertex 3


        400.0f, 400.0f, 0.0f, 0.0f, // vertex 0
        600.0f, 400.0f, 1.0f, 0.0f, // vertex 1
        600.0f, 600.0f, 1.0f, 1.0f, // vertex 2
        400.0f, 600.0f, 0.0f, 1.0f, // vertex 3
    };

    const unsigned int indices[] = {
        0, 1, 2, //1er  triangle
        0, 2, 3, //2eme triangle

        4, 5, 6, //1er  triangle
        4, 6, 7, //2eme triangle
    };
    
    vertex_array_init(&tg->vao);
    vertex_buffer_init(&tg->vbo, vertex, 4 * 8 * sizeof(float));
    
    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 2);//pos
    vertex_buffer_layout_push_float(&layout, 2);//tex
    vertex_array_add_buffer(&tg->vao, &tg->vbo, &layout);

    index_buffer_init(&tg->ibo, indices, 12);
    index_buffer_bind(&tg->ibo);

    texture_init(&tg->texture, "res/textures/c_logo.png");
    texture_bind(&tg->texture, &(uint){0});
    shader_init(&tg->shader, "res/shaders/shiny_2d_tex");
    shader_bind(&tg->shader);
    shader_set_uniform_1i(&tg->shader, "u_texture", 0);
    shader_set_uniform_4f(&tg->shader, "u_color", 0.35f, 0.2f, 0.6f, 0.0f);
    shader_set_uniform_mat4(&tg->shader, "u_view_proj", tg->cam.view_projection_matrix);
    shader_set_uniform_1f(&tg->shader, "u_time", 0);
}

static void test_game_stop(game_t *game)
{
    printf("%s\n", __func__);
}

static void test_game_pause(game_t *game)
{
    printf("%s\n", __func__);
}

static void test_game_play(game_t *game)
{
    printf("%s\n", __func__);
}

static void test_game_update(game_t *game, float delta_time)
{
    test_game_t *tg = (test_game_t*)game;

    update_camera_position(&tg->cam, &tg->input_manager, delta_time);

    //modifie la valeur temps du shader
    if(tg->time > 2 * M_PI) tg->time = 0;
    tg->time += tg->time_incr;
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = (test_game_t*)game;
    
    renderer_begin_scene(&tg->renderer, &tg->cam);
    
    shader_bind(&tg->shader);
    shader_set_uniform_1f(&tg->shader, "u_time", tg->time);
    renderer_draw(&tg->renderer, &tg->vao, &tg->ibo, &tg->shader);

    renderer_end_scene(&tg->renderer);
}

static void test_game_clean(game_t *game)
{
    test_game_t *tg = (test_game_t*)game;

    shader_destroy(&tg->shader);
    texture_destroy(&tg->texture);
    index_buffer_destroy(&tg->ibo);
    vertex_buffer_destroy(&tg->vbo);
    vertex_array_destroy(&tg->vao);

    renderer_destroy(&tg->renderer);
    cam_ortho_destroy(&tg->cam);
}


static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    test_game_t *tg = (test_game_t*)glfwGetWindowUserPointer(window);
    glViewport(0, 0, width, height);

    cam_ortho_set_viewport(&tg->cam, 0.0f, width, 0.0f, height);
}

static void update_camera_position(cam_ortho_t *cam, input_manager_t *input_manager, float delta_time)
{
    //en attendant de mettre dans la config
    static const int UP_KEY     = GLFW_KEY_W;
    static const int LEFT_KEY   = GLFW_KEY_A;
    static const int DOWN_KEY   = GLFW_KEY_S;
    static const int RIGHT_KEY  = GLFW_KEY_D;
    static const float CAMERA_SPEED = 300;

    vec3 move = GLM_VEC3_ZERO_INIT;
    
    if(input_manager_is_key_pressed(input_manager, UP_KEY))
        glm_vec3_add(move, (vec3){0, 1, 0}, move);

    if(input_manager_is_key_pressed(input_manager, LEFT_KEY))
        glm_vec3_add(move, (vec3){-1, 0, 0}, move);

    if(input_manager_is_key_pressed(input_manager, DOWN_KEY))
        glm_vec3_add(move, (vec3){0, -1, 0}, move);

    if(input_manager_is_key_pressed(input_manager, RIGHT_KEY))
        glm_vec3_add(move, (vec3){1, 0, 0}, move);

    //si on a pas bouger
    if(glm_vec3_eqv_eps(move, GLM_VEC3_ZERO)) return;

    //normalise le vecteur et multiplie par vitesse * delta time
    glm_vec3_normalize(move);
    glm_vec3_scale(move, CAMERA_SPEED * delta_time, move);
    
    //modifie la position de la camera
    glm_vec3_add(cam->position, move, move);
    cam_ortho_set_position(cam, move);
}

