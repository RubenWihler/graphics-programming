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
#include "../gllib/addons/camera_controller/cam_persp_controller.h"

#include "../gllib/components/transform/transform.h"
#include "../gllib/render/mesh/mesh.h"

// #include "../gllib/addons/camera_controller/cam_ortho_controller.h"


struct _test_game_t {
    game_t game;
    test_game_config_t config;

    input_manager_t input_manager;
    renderer_t renderer;
    cam_persp_controller_t cam_ctrl;

    //objet 1 : arbre
    mesh_t mesh;
    transform_t transform;
    material_t gold_mat;
    texture_t texture;
    shader_t shader;

    float time;
    float time_incr;
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

    //desactive cruseur
    glfwSetInputMode(game->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //camera
    int winw, winh;
    glfwGetWindowSize(game->window, &winw, &winh);
    const float aspect_ratio = (float)winw / (float)winh;

    cam_persp_controller_init(&tg->cam_ctrl, &tg->input_manager, aspect_ratio);

    tg->time = 0.0f;
    tg->time_incr = 0.1f;

    return true;
}


static void test_game_start(game_t *game)
{
    printf("%s\n", __func__);
    test_game_t *tg = container_of(game, test_game_t, game);

    // C'est quand même plus propre qu'un tableau de 200 lignes !
    if (!mesh_load_from_obj(&tg->mesh, "res/models/tree.obj")) {
        LOG_ERROR("Impossible de charger le modele 3D", true);
    }

    // Transform
    transform_init(&tg->transform);
    tg->transform.position[0] = 0.0f;

    // Texture
    texture_init(&tg->texture, "res/textures/tree.png");
    texture_bind(&tg->texture, &(uint){0});

    // Shader
    shader_init(&tg->shader, "res/shaders/default");
    shader_bind(&tg->shader);

    // Material
    material_init_default(&tg->gold_mat);
    glm_vec3_copy((vec3){0.247f, 0.199f, 0.074f}, tg->gold_mat.ambient);
    glm_vec3_copy((vec3){0.751f, 0.606f, 0.226f}, tg->gold_mat.diffuse);
    glm_vec3_copy((vec3){0.628f, 0.555f, 0.366f}, tg->gold_mat.specular);
    tg->gold_mat.shininess = 51.2f; // Brillance métallique
    // tg->gold_mat.diffuse_map = &tg->texture;

    // #define SHINYING_COLOR 1.0f, 1.0f, 1.0f, 1.0f
    // shader_set_uniform(&tg->shader, "u_time", 0.0);
    // shader_set_uniform(&tg->shader, "u_color", (vec4){ SHINYING_COLOR});
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

    cam_persp_controller_update(&tg->cam_ctrl, delta_time);

    // Faire tourner le cube via son transform (sur l'axe Y et Z par exemple)
    // float rotation_speed = 1.0f;
    // tg->cube_transform.rotation[1] += rotation_speed * delta_time; // Rotation Y
    // tg->cube_transform.rotation[2] += (rotation_speed * 0.5f) * delta_time; // Rotation Z

    //modifie la valeur temps du shader
    if(tg->time > 2 * M_PI) tg->time = 0;
    tg->time += tg->time_incr;
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);
    renderer_begin_scene(&tg->renderer, &tg->cam_ctrl.cam);

    shader_bind(&tg->shader);

    // --- VARIABLES D'ÉCLAIRAGE ---
    vec3 light_pos = {50.0f, 100.0f, 2.0f}; 
    vec3 light_color = {1.0f, 1.0f, 1.0f}; 
    vec3 view_pos;
    glm_vec3_copy(tg->cam_ctrl.cam.position, view_pos);
    shader_set_uniform_vec3(&tg->shader, "u_lightPos", light_pos);
    shader_set_uniform_vec3(&tg->shader, "u_lightColor", light_color);
    shader_set_uniform_vec3(&tg->shader, "u_viewPos", view_pos);

    // uint32_t slot = 0;
    // texture_bind(&tg->texture, &slot);
    // shader_set_uniform(&tg->shader, "u_texture", 0);

    mat4 model_matrix;
    transform_get_matrix(&tg->transform, model_matrix);
    shader_set_uniform(&tg->shader, "u_model", &model_matrix[0]);

    renderer_draw(
        &tg->renderer,
        &tg->mesh.vao,
        &tg->mesh.ibo,
        &tg->shader,
        &tg->gold_mat
    );

    renderer_end_scene(&tg->renderer);
}

static void test_game_clean(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    mesh_destroy(&tg->mesh);
    shader_destroy(&tg->shader);
    texture_destroy(&tg->texture);

    renderer_destroy(&tg->renderer);
    // cam_ortho_controller_destroy(&tg->cam_controller);
}


static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);
    glViewport(0, 0, width, height);

    float aspect_ratio = (float)width / (float)height;
    cam_persp_controller_resize(&tg->cam_ctrl, aspect_ratio);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)yoffset;
    (void)xoffset;
    // test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);
    // cam_ortho_controller_zoom(&tg->cam_controller, yoffset);
}
