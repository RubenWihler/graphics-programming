// Description: Main file for the project.
// Author: Wihler Ruben

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

typedef struct _app_config_t{
    int width, height;
    const char* window_name;
} app_config_t;
                
int init(app_config_t config, GLFWwindow** window);
void clean();
void print_version();

const app_config_t APP_CONFIG = {
    .window_name = "Test OpenGL #2",
    .width =  640,
    .height = 480
};

int main(void)
{
    GLFWwindow* window = NULL;
    
    //Initialisation
    if (init(APP_CONFIG, &window)) return EXIT_FAILURE;
    
    //Affichage des versions
    print_version();

    //Temps que la fenêtre n'est pas fermée
    while (!glfwWindowShouldClose(window))
    {   
        //On efface le buffer de couleur
        //(en gros on dessine un fond noir)
        glClear(GL_COLOR_BUFFER_BIT);
        
        //On dessine notre triangle
        glBegin(GL_TRIANGLES);
        glVertex2f( 0.0f, 0.5f);
        glVertex2f(-0.5f,-0.5f);
        glVertex2f( 0.5f,-0.5f);
        glEnd();

        //On échange les buffers
        //C'est à dire qu'on affiche le buffer ou l'on a dessiné
        //et on met en attente le buffer ou on va dessiner
        glfwSwapBuffers(window);
    
        //On gère les événements
        glfwPollEvents();
    }
    
    //On libère la mémoire
    clean();

    return 0;
}

int init(app_config_t config, GLFWwindow** window)
{
    //Init GLFW
    if (!glfwInit())
    {
        fprintf(stderr,"Error while init GLFW\n");
        return -1;
    }
    
    // Create a windowed mode window and its OpenGL context
    *window = glfwCreateWindow(config.width, config.height, config.window_name, NULL, NULL);
    if (!(*window))
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(*window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr,"Error while init GLEW: %s\n", glewGetErrorString(glewInit()));
        return -1;
    }

    return 0;
}

void clean()
{
    glfwTerminate();
}

void print_version()
{
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLFW version: %s\n", glfwGetVersionString());
    printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));
}

