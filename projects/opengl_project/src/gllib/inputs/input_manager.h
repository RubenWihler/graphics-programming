#pragma once

#include "../vendor/cglm/ivec2.h"

#include <stdbool.h>
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow *window;
} input_manager_t;

bool input_manager_init(input_manager_t *input_manager, GLFWwindow *window);
void input_manager_destroy(input_manager_t *input_manager);

bool input_manager_is_key_pressed(input_manager_t *input_manager, int key);
bool input_manager_is_mouse_pressed(input_manager_t *input_manager, int btn);

void input_manager_get_mouse_pos(input_manager_t *input_manager, vec2 pos);
