#pragma once

#include "game_config.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>

typedef enum _game_state_e {
    GAME_STATE_INIT,
    GAME_STATE_RUNNING,
    GAME_STATE_PAUSED,
    GAME_STATE_STOPPED
} game_state_e;

typedef struct _game_t game_t;

typedef struct _game_api_t{
    void (*on_start)(game_t *game);
    void (*on_stop)(game_t *game);
    void (*on_pause)(game_t *game);
    void (*on_play)(game_t *game);
    void (*on_update)(game_t *game, float delta_time);
    void (*on_render)(game_t *game);
    void (*on_clean)(game_t *game);

    struct {
        void (*framebuffer_size_callback)(GLFWwindow *window, int width, int height);
        void (*key_callback)(GLFWwindow *window, int key, int scancode, int action, int mods);
        void (*mouse_button_callback)(GLFWwindow *window, int button, int action, int mods);
        void (*cursor_position_callback)(GLFWwindow *window, double xpos, double ypos);
        void (*scroll_callback)(GLFWwindow *window, double xoffset, double yoffset);
    } glfw_callbacks;

} game_api_t;

struct _game_t {
    game_config_t config;
    game_state_e state;
    
    GLFWwindow *window;
    bool is_running;
    
    game_api_t api;
};


typedef bool (*game_init_fn)(game_t *game);

int game_init(game_t *game, game_config_t config, game_init_fn init);
void game_clean(game_t *game);
void game_start(game_t *game);
void game_stop(game_t *game);
void game_pause(game_t *game);
void game_play(game_t *game);
