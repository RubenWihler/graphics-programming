/*
* Pour gérer les inputs, nous pourrions utiliser deux hashmaps, une pour les touches et une pour la souris.
* C'est d'ailleurs ce que j'avais prévu de faire. Cependant, je me suis rendu compte que cela n'était pas nécessaire car Glfw nous fournit déjà un système de cache pour les inputs.
*
* Avec les glfwGetKey et glfwGetMouseButton, nous pouvons savoir si une touche ou un bouton de la souris est pressé ou non.
* Nous n'avons donc pas besoin de stocker les états des touches et des boutons de la souris dans des hashmaps.
*/

#include "input_manager.h"
#include <GLFW/glfw3.h>


bool input_manager_init(input_manager_t *input_manager, GLFWwindow *window)
{
    input_manager->window = window;
    return true;
}

bool input_manager_is_key_pressed(input_manager_t *input_manager, int key)
{
    int state = glfwGetKey(input_manager->window, key);
    return state == GLFW_PRESS;
}

bool input_manager_is_mouse_pressed(input_manager_t *input_manager, int btn)
{
    int state = glfwGetMouseButton(input_manager->window, btn);
    return state == GLFW_PRESS;
}

void input_manager_get_mouse_pos(input_manager_t *input_manager, vec2 pos)
{
    double x, y;
    glfwGetCursorPos(input_manager->window, &x, &y);
    pos[0] = (float)x;
    pos[1] = (float)y;
}

