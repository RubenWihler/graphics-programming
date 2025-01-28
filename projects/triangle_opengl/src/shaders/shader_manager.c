#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include "shader_manager.h"

unsigned int create_shader_program(const char* vertex_src, const char* fragment_src)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    
    if(vs == 0 || fs == 0)
    {
        fprintf(stderr, "Cannot create shader program due to a compilation error!\n");
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int compile_shader(const unsigned int type, const char* source)
{
    const char *src = &source[0];

    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    
    int status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        
        char *msg = alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, NULL, msg);
        
        fprintf(stderr, "Error while compiling %s shader : %s\n", 
                (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", msg);

        glDeleteShader(id);
        return 0;
    }

    return id;
}
