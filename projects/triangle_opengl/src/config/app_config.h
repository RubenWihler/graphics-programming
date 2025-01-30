#pragma once

#include <stdbool.h>

typedef struct _app_config_t{
    int width, height;
    const char* window_name;
    bool vsync;
    int glfw_context_major_version;
    int glfw_context_minor_version;
    int opengl_profile;
} app_config_t;
