#pragma once

#include <stdbool.h>

typedef struct _app_config_t{
    int width, height;
    const char* window_name;
    bool vsync;
} app_config_t;
