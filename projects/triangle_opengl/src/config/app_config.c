#include "app_config.h"
#include <GLFW/glfw3.h>

const app_config_t APP_CONFIG = {
    .window_name = "Test OpenGL #2",
    .width =  640,
    .height = 480,
    .vsync = true,

    //version d'OpenGL - 4.6 CORE PROFILE
    .glfw_context_major_version = 4,
    .glfw_context_minor_version = 6,
    .opengl_profile = GLFW_OPENGL_CORE_PROFILE,
};
