// Description: Main file for the project.
// Author: Wihler Ruben

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "config/app_config.h"
#include "shaders/shader_manager.h"
#include "shaders/shader_parser.h"

int init(app_config_t config, GLFWwindow** window);
void print_version();

extern app_config_t APP_CONFIG;

int main(void)
{
    GLFWwindow* window = NULL;
    
    //Initialisation
    if (init(APP_CONFIG, &window)) return EXIT_FAILURE;
    
    //Affichage des versions
    print_version();
        


    //un vertex par ligne
    //1 seule attribut: 2 float - position xy
    const float triangle_vertices[6] = {
         0.0f,  0.5f, 
        -0.5f, -0.5f,
         0.5f, -0.5f,
    };

    unsigned int buffer_id;
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), triangle_vertices, GL_STATIC_DRAW);

    //attributs
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0); // on active cette attribut
    
    shader_program_source_t* shader_source_code = 
        shader_program_source_parse("res/shaders/basic");

    unsigned int shader_program = create_shader_program(shader_source_code->vertex,
                                                        shader_source_code->fragment);
    glUseProgram(shader_program);

    //Temps que la fenêtre n'est pas fermée
    while (!glfwWindowShouldClose(window))
    {   
        //On efface le buffer de couleur
        //(en gros on dessine un fond noir)
        glClear(GL_COLOR_BUFFER_BIT);
            
        //On fais le draw call sur le buffer bindé
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //On échange les buffers
        //C'est à dire qu'on affiche le buffer ou l'on a dessiné
        //et on met en attente le buffer ou on va dessiner
        glfwSwapBuffers(window);
    
        //On gère les événements
        glfwPollEvents();
    }
    


    //On libère la mémoire
    glDeleteProgram(shader_program);
    glfwTerminate();

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

void print_version()
{
    #ifdef DEBUG
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLFW version: %s\n", glfwGetVersionString());
    printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));
    #endif
}

