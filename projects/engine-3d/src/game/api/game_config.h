#pragma once

#include <stdbool.h>

typedef struct _game_config_t {
    int width, height;
    const char* window_name;
    bool vsync;
    unsigned int fps;

    //api version settings
    struct {
        int glfw_context_major_version;
        int glfw_context_minor_version;
        int opengl_profile;
    } api_version;

    //render settings
    struct {
        bool enabled;
        unsigned int src;
        unsigned int dst;
    } blending;

    struct {
        bool enabled;
    } depth;
} game_config_t;

extern game_config_t DEFAULT_GAME_CONFIG;
