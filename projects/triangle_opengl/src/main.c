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
    const float triangle_vertices[] = {
        -0.5f, -0.5f, 
         0.5f, -0.5f,
         0.5f,  0.5f,
        
        -0.5f, -0.5f, 
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    //Création du buffer
    unsigned int buffer_id;
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), triangle_vertices, GL_STATIC_DRAW);

    //Attribut 0: position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0); // on active cette attribut
    
    //Chargement des shaders
    shader_program_source_t* shader_source_code_basic = shader_program_source_parse("res/shaders/basic");
    shader_program_source_t* shader_source_code_red = shader_program_source_parse("res/shaders/red");

    //Création des programmes a partir des sources
    unsigned int shader_program_basic = create_shader_program(shader_source_code_basic->vertex, shader_source_code_basic->fragment);
    unsigned int shader_program_red   = create_shader_program(shader_source_code_basic->vertex, shader_source_code_red->fragment);

    //Libération de la mémoire des sources
    shader_program_source_destroy(shader_source_code_basic);
    shader_program_source_destroy(shader_source_code_red);

    //Temps que la fenêtre n'est pas fermée
    while (!glfwWindowShouldClose(window))
    {   
        //On efface le buffer de couleur
        //(en gros on dessine un fond noir)
        glClear(GL_COLOR_BUFFER_BIT);
            
        //dessine le triangle bleu
        glUseProgram(shader_program_basic);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //dessine le triangle rouge
        glUseProgram(shader_program_red);
        glDrawArrays(GL_TRIANGLES, 3, 3);

        //On échange les buffers
        //C'est à dire qu'on affiche le buffer ou l'on a dessiné
        //et on met en attente le buffer ou on va dessiner
        glfwSwapBuffers(window);
    
        //On gère les événements
        glfwPollEvents();
    }

    //On libère la mémoire
    glDeleteProgram(shader_program_basic);
    glDeleteProgram(shader_program_red);
    glDeleteBuffers(1, &buffer_id);
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

