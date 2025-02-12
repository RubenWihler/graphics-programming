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
#include "../gllib/render/vertex_buffer/vertex_buffer.h"
#include "../gllib/render/index_buffer/index_buffer.h"
#include "../gllib/render/vertex_array/vertex_array.h"
#include "../gllib/render/shader/shader.h"
#include "../gllib/render/texture/texture.h"
#include "../gllib/render/renderer/renderer.h"

struct _test_game_t {
    game_t game;
    test_game_config_t config;

    vertex_array_t vao;
    vertex_buffer_t vbo;
    index_buffer_t ibo;
    texture_t texture;
    shader_t shader;
    float time;
    float time_incr;

    mat4 *model;//car que 1 obj pour l'instant
    mat4 *view;
    mat4 *proj;
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

    return true;
}


static void test_game_start(game_t *game)
{
    printf("%s\n", __func__);
    test_game_t *tg = (test_game_t*)game;

    //matrices - Model View Projection (MVP)
    mat4 identity = GLM_MAT4_IDENTITY_INIT;
    tg->model = (mat4*)malloc(sizeof(mat4));
    tg->view = (mat4*)malloc(sizeof(mat4));
    tg->proj = (mat4*)malloc(sizeof(mat4));
    memcpy(*tg->model, identity, sizeof(mat4));
    memcpy(*tg->view,  identity, sizeof(mat4));
    memcpy(*tg->proj,  identity, sizeof(mat4));
    
    glm_ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, *tg->proj);
    glm_translate(*tg->view, (vec3){0.0f, 0.0f, 0.0f});
    glm_translate(*tg->model, (vec3){0.0f, 0.0f, 0.0f});

    const float vertex[] = {
    // |     pos    |    tex    |
        -0.5f, -0.5f, 0.0f, 0.0f, // vertex 0
         0.5f, -0.5f, 1.0f, 0.0f, // vertex 1
         0.5f,  0.5f, 1.0f, 1.0f, // vertex 2
        -0.5f,  0.5f, 0.0f, 1.0f, // vertex 3
    };

    const unsigned int indices[] = {
        0, 1, 2, //1er  triangle
        0, 2, 3, //2eme triangle
    };
    
    vertex_array_init(&tg->vao);
    vertex_buffer_init(&tg->vbo, vertex, 4 * 4 * sizeof(float));
    
    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 2);//pos
    vertex_buffer_layout_push_float(&layout, 2);//tex
    vertex_array_add_buffer(&tg->vao, &tg->vbo, &layout);

    index_buffer_init(&tg->ibo, indices, 6 * sizeof(unsigned int));
    index_buffer_bind(&tg->ibo);

    texture_init(&tg->texture, "res/textures/c_logo.png");
    texture_bind(&tg->texture, &(uint){0});
    shader_init(&tg->shader, "res/shaders/shiny_image");
    shader_bind(&tg->shader);
    shader_set_uniform_1i(&tg->shader, "u_texture", 0);
    shader_set_uniform_4f(&tg->shader, "u_color", 0.35f, 0.2f, 0.6f, 0.0f);
    shader_set_uniform_mat4(&tg->shader, "u_mvp", *tg->proj);
    shader_set_uniform_1f(&tg->shader, "u_time", 0);

    tg->time = 0.0f;
    tg->time_incr = 0.1f;
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

    if(tg->time > 2 * M_PI) tg->time = 0;
    tg->time += tg->time_incr;
    
    glfwGetWindowSize(game->window, &game->config.width, &game->config.height);

    /* mat4 proj = GLM_MAT4_IDENTITY_INIT; */
    /* glm_ortho(0.0f, game->config.width, 0.0f, game->config.height, -1.0f, 1.0f, proj); */
    /* memcpy(*tg->proj, proj, sizeof(mat4)); */
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = (test_game_t*)game;
    
    mat4 mvp = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_mul(*tg->proj, *tg->view, mvp);
    glm_mat4_mul(mvp, *tg->model, mvp);
    
    shader_bind(&tg->shader);
    shader_set_uniform_1f(&tg->shader, "u_time", tg->time);
    shader_set_uniform_mat4(&tg->shader, "u_mvp", mvp);

    renderer_draw(NULL, &tg->vao, &tg->ibo, &tg->shader);
}

static void test_game_clean(game_t *game)
{
    test_game_t *tg = (test_game_t*)game;

    free(tg->model);
    free(tg->view);
    free(tg->proj);

    shader_destroy(&tg->shader);
    texture_destroy(&tg->texture);
    index_buffer_destroy(&tg->ibo);
    vertex_buffer_destroy(&tg->vbo);
    vertex_array_destroy(&tg->vao);
}


static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
