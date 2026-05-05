#include "test_game.h"

#include <GLFW/glfw3.h>

#include "../../core/vendor/cglm/types.h"
// #include "../../core/vendor/cglm/mat4.h"
// #include "../../core/vendor/cglm/cglm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../api/game.h"
#include "../../core/log/log.h"
#include "../../core/utils/common.h"
#include "../../core/inputs/input_manager.h"
#include "../../core/render/shader/shader.h"
#include "../../core/render/texture/texture.h"
#include "../../core/render/renderer/renderer.h"
#include "../../core/render/camera/cam_persp.h"
#include "../../engine/camera_controller/cam_persp_controller.h"
#include "../../core/math/transform/transform.h"
#include "../../core/render/mesh/mesh.h"

#include "../../core/ecs/ecs.h"
#include "../../engine/components/c_mesh.h"
#include "../../engine/components/components.h"
#include "../../engine/systems/s_render.h"
#include "../../engine/systems/s_rotator.h"

struct _test_game_t {
    game_t game;
    test_game_config_t config;

    ecs_registry_t registry;
    input_manager_t input_manager;
    renderer_t renderer;
    cam_persp_controller_t cam_ctrl;

    //objet 1
    shader_t shader;

    //si le model n'a pas de material defini dans le .mtl
    //on le fait a la main
    material_t gold_mat;
    texture_t texture;

    //en attendant le ressource manager
    model_t model;
};

//game api
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

//lancement du jeu
static void test_game_start(game_t *game)
{
    LOG_INFO("%s\n", __func__);
    test_game_t *tg = container_of(game, test_game_t, game);

    // Shader
    shader_init(&tg->shader, "res/shaders/default");
    //si le shader a besoin d'uniforms additionnel:
    // shader_bind(&tg->shader);
    // uniforms specifique...

    // Charger texture manuelement
    texture_init(&tg->texture, "res/models/moon_tex.png");
    // texture_bind(&tg->texture, &(uint){0});

    // Material
    material_init_default(&tg->gold_mat);
    glm_vec3_copy((vec3){0.247f, 0.199f, 0.074f}, tg->gold_mat.ambient);
    glm_vec3_copy((vec3){0.751f, 0.606f, 0.226f}, tg->gold_mat.diffuse);
    glm_vec3_copy((vec3){0.628f, 0.555f, 0.366f}, tg->gold_mat.specular);
    tg->gold_mat.shininess = 51.2f; // Brillance métallique
    tg->gold_mat.diffuse_map = &tg->texture;

    for (size_t i = 0; i < 10000; i++){
        // 2. Création de l'Entité "Lune"
        entity_t moon = ecs_create_entity(&tg->registry);

        // 3. Ajout du Transform
        transform_t t;
        transform_init(&t);
        t.position[0] = ((float)(rand() % 100) - 50.0f);
        t.position[1] = ((float)(rand() % 100) - 50.0f);
        t.position[2] = ((float)(rand() % 100) - 50.0f);
        ecs_add_component(&tg->registry, moon, COMP_TRANSFORM, &t);

        // 4. Ajout du Mesh
        mesh_component_t m;
        m.model = &tg->model;
        model_load_from_obj(m.model, "res/models/sphere.obj", "res/models/");
        m.use_material_override = true;
        m.material_override = tg->gold_mat; // Ton matériau doré
        ecs_add_component(&tg->registry, moon, COMP_MESH, &m);
    }
}

static void test_game_update(game_t *game, float delta_time)
{
    (void)delta_time;
    test_game_t *tg = container_of(game, test_game_t, game);

    cam_persp_controller_update(&tg->cam_ctrl, delta_time);

    // Faire tourner un objet via son transform (sur l'axe Y et Z par exemple)
    s_rotator_update(&tg->registry, 1.0f, delta_time);

    //
    // float rotation_speed = 1.0f;
    // tg->cube_transform.rotation[1] += rotation_speed * delta_time; // Rotation Y
    // tg->cube_transform.rotation[2] += (rotation_speed * 0.5f) * delta_time; // Rotation Z
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);
    renderer_begin_scene(&tg->renderer, &tg->cam_ctrl.cam);

    // --- VARIABLES D'ÉCLAIRAGE ---
    vec3 light_pos = {0.0f, 0.0f, 0.0f}; 
    vec3 light_color = {1.0f, 1.0f, 1.0f}; 
    vec3 view_pos;
    glm_vec3_copy(tg->cam_ctrl.cam.position, view_pos);
    shader_bind(&tg->shader);
    shader_set_uniform_vec3(&tg->shader, "u_lightPos", light_pos);
    shader_set_uniform_vec3(&tg->shader, "u_lightColor", light_color);
    shader_set_uniform_vec3(&tg->shader, "u_viewPos", view_pos);

    s_render_update(&tg->registry, &tg->renderer, &tg->shader);

    renderer_end_scene(&tg->renderer);
}

//s'execute avant le lancement du jeu
static bool test_game_init(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    ecs_init(&tg->registry);
    ecs_register_component(&tg->registry, COMP_TRANSFORM, sizeof(transform_t));
    ecs_register_component(&tg->registry, COMP_MESH, sizeof(mesh_component_t));

    // Input manager
    if(!input_manager_init(&tg->input_manager, game->window))
    {
        LOG_ERROR("input manager initialization failed!");
        return false;
    }

    // Renderer
    if(!renderer_init(&tg->renderer, 0))
    {
        LOG_ERROR("renderer initialization failed!");
        return false;
    }

    // Camera Controller
    int winw, winh;
    glfwGetWindowSize(game->window, &winw, &winh);
    float ratio = (float)winw / (float)winh;
    cam_persp_controller_init(&tg->cam_ctrl, &tg->input_manager, ratio, tg->config.camera_ctrl);

    //game api (mapper les fonction de l'api)
    game->api.on_start = test_game_start;
    game->api.on_stop = test_game_stop;
    game->api.on_pause = test_game_pause;
    game->api.on_play = test_game_play;
    game->api.on_update = test_game_update;
    game->api.on_render = test_game_render;
    game->api.on_clean = test_game_clean;

    //glfw callbacks (mapper les callbacks de l'api)
    game->api.glfw_callbacks.framebuffer_size_callback = framebuffer_size_callback;
    game->api.glfw_callbacks.key_callback = NULL;
    game->api.glfw_callbacks.mouse_button_callback = NULL;
    game->api.glfw_callbacks.cursor_position_callback = NULL;
    game->api.glfw_callbacks.scroll_callback = scroll_callback;

    //desactive cruseur
    glfwSetInputMode(game->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return true;
}

static void test_game_stop(game_t *game)
{
    (void)game;
    LOG_INFO("%s\n", __func__);
}

static void test_game_pause(game_t *game)
{
    (void)game;
    LOG_INFO("%s\n", __func__);
}

static void test_game_play(game_t *game)
{
    (void)game;
    LOG_INFO("%s\n", __func__);
}

static void test_game_clean(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    model_destroy(&tg->model);
    shader_destroy(&tg->shader);
    texture_destroy(&tg->texture);
    renderer_destroy(&tg->renderer);
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

game_t* test_game_create(test_game_config_t* config)
{
    test_game_t *test_game = (test_game_t*)calloc(1, sizeof(test_game_t));
    if(!test_game) return (perror("malloc"), NULL);

    test_game->config = *config;

    if (!game_init(&test_game->game, &config->game_config, test_game_init))
    {
        LOG_ERROR("game initialization failed!");
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

