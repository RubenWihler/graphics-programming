#include "game_config.h"
#include <GLFW/glfw3.h>
#include <sys/ucontext.h>

game_config_t DEFAULT_GAME_CONFIG = 
{
    //window settings
    .window_name = "game",
    .width = 960,
    .height = 540,
    .vsync = true,
    .fps = 0,
 
    //api version settings
    .api_version = {
        .glfw_context_major_version = 4,
        .glfw_context_minor_version = 6,
        .opengl_profile = GLFW_OPENGL_CORE_PROFILE
    },

    //render settings
    .blending = {
        .enabled = true,
        .src = GL_SRC_ALPHA,
        .dst = GL_ONE_MINUS_SRC_ALPHA
    },

    .depth = {
        .enabled = true
    }
};

