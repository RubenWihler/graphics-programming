#pragma once

#include <stdbool.h>

typedef enum _app_mode_e{
    APP_MODE_NONE,
    APP_MODE_HELP,
    APP_MODE_VERSION,
    APP_MODE_LIST_SHADERS,
    APP_MODE_RUN
} app_mode_e;

typedef struct _app_config_t{
    const char* version;
    int width, height;
    const char* window_name;
    bool vsync;
    int glfw_context_major_version;
    int glfw_context_minor_version;
    int opengl_profile;

    //user defined
    const char* image_path;
    const char* shader_name;
    app_mode_e mode;
} app_config_t;
