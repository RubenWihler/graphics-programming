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
#include "../../core/math/transform/transform.h"

#include "../../core/ecs/ecs.h"
#include "../../engine/assets/asset_manager.h"
#include "../../engine/components/components.h"
#include "../../engine/systems/s_render.h"
#include "../../engine/systems/s_rotator.h"
#include "../../engine/systems/s_camera_controller.h"

struct _test_game_t {
    game_t game;
    test_game_config_t config;

    ecs_registry_t registry;
    input_manager_t input_manager;
    renderer_t renderer;
    asset_manager_t asset_manager;

    //objet 1
    shader_t shader;

    //si le model n'a pas de material defini dans le .mtl
    //on le fait a la main
    material_t gold_mat;
    texture_t texture;
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

    int winw, winh;
    glfwGetWindowSize(game->window, &winw, &winh);

    // --- CREATION DE LA CAMERA ---
    entity_t camera_ent = ecs_create_entity(&tg->registry);

    transform_t cam_t;
    transform_init(&cam_t);
    ecs_add_component(&tg->registry, camera_ent, COMP_TRANSFORM, &cam_t);

    camera_component_t cam;
    camera_component_init(&cam, tg->config.camera_ctrl.fov_deg, (float)winw / winh, tg->config.camera_ctrl.near_z, tg->config.camera_ctrl.far_z);
    ecs_add_component(&tg->registry, camera_ent, COMP_CAMERA, &cam);

    camera_controller_component_t cam_ctrl;
    camera_controller_init(&cam_ctrl, tg->config.camera_ctrl.movement_speed, tg->config.camera_ctrl.mouse_sensitivity);
    ecs_add_component(&tg->registry, camera_ent, COMP_CAMERA_CONTROLLER, &cam_ctrl);

    // Shader
    shader_init(&tg->shader, "res/shaders/default");
    //si le shader a besoin d'uniforms additionnel:
    // shader_bind(&tg->shader);
    // uniforms specifique...

    // Material
    material_init_default(&tg->gold_mat);
    glm_vec3_copy((vec3){0.247f, 0.199f, 0.074f}, tg->gold_mat.ambient);
    glm_vec3_copy((vec3){0.751f, 0.606f, 0.226f}, tg->gold_mat.diffuse);
    glm_vec3_copy((vec3){0.628f, 0.555f, 0.366f}, tg->gold_mat.specular);
    tg->gold_mat.shininess = 51.2f; // Brillance métallique
    tg->gold_mat.diffuse_map = asset_manager_get_texture(&tg->asset_manager, "res/models/moon_tex.png");


    // 1. Soleil principal (Blanc/Jaune)
    entity_t sun1 = ecs_create_entity(&tg->registry);
    transform_t t1; transform_init(&t1);
    t1.position[0] = 10.0f; t1.position[1] = 20.0f; t1.position[2] = 10.0f;
    ecs_add_component(&tg->registry, sun1, COMP_TRANSFORM, &t1);
    
    light_component_t l1;
    light_component_init(&l1, (vec3){0.4f, 0.9f, 0.5f}, 1.0f);
    ecs_add_component(&tg->registry, sun1, COMP_LIGHT, &l1);

    // 2. Éclairage d'ambiance (Lumière bleue venant de l'autre côté)
    entity_t sun2 = ecs_create_entity(&tg->registry);
    transform_t t2; transform_init(&t2);
    t2.position[0] = -15.0f; t2.position[1] = 5.0f; t2.position[2] = -10.0f;
    ecs_add_component(&tg->registry, sun2, COMP_TRANSFORM, &t2);
    
    light_component_t l2;
    light_component_init(&l2, (vec3){0.2f, 0.2f, 1.0f}, 0.8f); // Bleue
    ecs_add_component(&tg->registry, sun2, COMP_LIGHT, &l2);

    // 3. (Optionnel) Une lumière rouge venant du bas (style lave)
    entity_t sun3 = ecs_create_entity(&tg->registry);
    transform_t t3; transform_init(&t3);
    t3.position[0] = 0.0f; t3.position[1] = -20.0f; t3.position[2] = 0.0f;
    ecs_add_component(&tg->registry, sun3, COMP_TRANSFORM, &t3);
    
    light_component_t l3;
    light_component_init(&l3, (vec3){1.0f, 0.1f, 0.1f}, 0.5f); // Rouge
    ecs_add_component(&tg->registry, sun3, COMP_LIGHT, &l3);

    // entity_t sun = ecs_create_entity(&tg->registry);
    // transform_t t;
    // transform_init(&t);
    // t.position[0] = 10.0f;
    // t.position[1] = 20.0f; // La lumière vient d'en haut
    // t.position[2] = 10.0f;    
    // ecs_add_component(&tg->registry, sun, COMP_TRANSFORM, &t);
    //
    // light_component_t sun_light;
    // light_component_init(&sun_light, (vec3){1.0f, 0.9f, 0.8f}, 1.2f);
    // ecs_add_component(&tg->registry, sun, COMP_LIGHT, &sun_light);

    for (size_t i = 0; i < 10000; i++){
        //Création de l'Entité
        entity_t moon = ecs_create_entity(&tg->registry);

        //Ajout du Transform
        transform_t t;
        transform_init(&t);
        t.position[0] = ((float)(rand() % 100) - 50.0f);
        t.position[1] = ((float)(rand() % 100) - 50.0f);
        t.position[2] = ((float)(rand() % 100) - 50.0f);
        ecs_add_component(&tg->registry, moon, COMP_TRANSFORM, &t);

        //Ajout du Mesh
        mesh_component_t m;
        m.model = asset_manager_get_model(&tg->asset_manager, "res/models/sphere.obj", "res/models/");
        m.use_material_override = true;
        m.material_override = tg->gold_mat;
        ecs_add_component(&tg->registry, moon, COMP_MESH, &m);
    }
}

static void test_game_update(game_t *game, float delta_time)
{
    (void)delta_time;
    test_game_t *tg = container_of(game, test_game_t, game);

    // Faire tourner un objet via son transform (sur l'axe Y et Z par exemple)
    s_rotator_update(&tg->registry, 1.0f, delta_time);
    s_camera_controller_update(&tg->registry, &tg->input_manager, delta_time);
}

static void test_game_render(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    // 1. Chercher la camera active
    camera_component_t* active_cam = NULL;
    transform_t* active_cam_t = NULL;

    signature_t cam_sig = (1 << COMP_TRANSFORM) | (1 << COMP_CAMERA);
    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
        if ((tg->registry.signatures[e] & cam_sig) == cam_sig) {
            camera_component_t* c = ecs_get_component(&tg->registry, e, COMP_CAMERA);
            if (c && c->is_active) {
                active_cam = c;
                active_cam_t = (transform_t*)ecs_get_component(&tg->registry, e, COMP_TRANSFORM);
                break; // On a trouve !
            }
        }
    }

    if (!active_cam) return; // Pas de camera, on ne dessine rien !
    
    //Envoyer les infos au renderer
    renderer_begin_scene(&tg->renderer, &active_cam->view_matrix, &active_cam->projection_matrix);

    // envoiyer les matrices au shaders
    shader_bind(&tg->shader);
    shader_set_uniform_vec3(&tg->shader, "u_viewPos", active_cam_t->position);

    // 3. Dessiner la scene
    s_render_update(&tg->registry, &tg->renderer, &tg->shader);

    renderer_end_scene(&tg->renderer);}

//s'execute avant le lancement du jeu
static bool test_game_init(game_t *game)
{
    test_game_t *tg = container_of(game, test_game_t, game);

    ecs_init(&tg->registry);
    ecs_register_component(&tg->registry, COMP_TRANSFORM, sizeof(transform_t));
    ecs_register_component(&tg->registry, COMP_MESH, sizeof(mesh_component_t));
    ecs_register_component(&tg->registry, COMP_CAMERA, sizeof(camera_component_t));
    ecs_register_component(&tg->registry, COMP_CAMERA_CONTROLLER, sizeof(camera_controller_component_t));
    ecs_register_component(&tg->registry, COMP_LIGHT, sizeof(light_component_t));

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

    // Assets Manager
    asset_manager_init(&tg->asset_manager);

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

    asset_manager_destroy(&tg->asset_manager);
    shader_destroy(&tg->shader);
    texture_destroy(&tg->texture);
    renderer_destroy(&tg->renderer);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    test_game_t *tg = container_of(glfwGetWindowUserPointer(window), test_game_t, game);

    // 1. Chercher la camera active
    camera_component_t* active_cam = NULL;

    signature_t cam_sig = (1 << COMP_TRANSFORM) | (1 << COMP_CAMERA);
    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
        if ((tg->registry.signatures[e] & cam_sig) == cam_sig) {
            camera_component_t* c = ecs_get_component(&tg->registry, e, COMP_CAMERA);
            if (c && c->is_active) {
                active_cam = c;
                break; // On a trouve !
            }
        }
    }

    if (!active_cam) return; // Pas de camera

    glViewport(0, 0, width, height);
    float aspect_ratio = (float)width / (float)height;
    camera_component_resize(active_cam, aspect_ratio);
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

