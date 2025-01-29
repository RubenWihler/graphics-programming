
#include <GL/glew.h>
#include <stdio.h>

#include "gl_errors_manager.h"

#define COLOR_ERROR "\e[0;31m"
#define COLOR_RESET "\e[0m"

void gl_clear_errors()
{
    while(glGetError() != GL_NO_ERROR);
}

int gl_log_call(const char* func, const char* file, const int line)
{
    unsigned int count = 0;
    GLenum error;

    while((error = glGetError()) != GL_NO_ERROR)
    {
        fprintf(stderr, COLOR_ERROR "[GL-ERROR](errcode:%u}) in file:%s at ligne:%d : %s" COLOR_RESET "\n",
                                                                                error, file, line, func);
        count++;
    }
    
    return count;
}
