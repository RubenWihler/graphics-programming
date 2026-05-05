#include <GL/glew.h>
#include <stdio.h>
#include <alloca.h>
#include <signal.h>

#include "log.h"

#define COLOR_ERROR "\e[0;31m"
#define COLOR_RESET "\e[0m"

void log_error(char* msg, const char* func, const char* file, const int line, bool interrupt)
{
    fprintf(stderr, COLOR_ERROR "[ERROR]: %s in file:%s at ligne:%d\n"
            "    -> %s" COLOR_RESET "\n", 
            msg, file, line, func);

    if(interrupt) raise(SIGINT);
}

void log_info(char* msg)
{
    fprintf(stdout, "[INFO]: %s\n", msg);
}

int log_gl_error_all(const char* func, const char* file, const int line, bool interrupt)
{
    int count = 0;
    GLenum error;

    while((error = glGetError()) != GL_NO_ERROR)
    {
        //31 = 20(msg) + 10(errcode) + 1('\0')
        char* msg = alloca(31 * sizeof(*msg));
        sprintf(msg, "OpenGL-Error (code:%#010x)", error);

        log_error(msg, func, file, line, false);
        count++;
    }
    
    if(interrupt && count > 0) raise(SIGINT);

    return count;
}

void log_gl_clear_errors()
{
    while(glGetError() != GL_NO_ERROR);
}

//retourn le nombre d'erreurs
int log_gl_check_errors()
{
    GLenum error;
    int count = 0;

    while((error = glGetError()) != GL_NO_ERROR) 
        count++;

    return count;
}

