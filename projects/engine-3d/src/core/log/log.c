#include <GL/glew.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

#include "log.h"

void log_error(bool interrupt, const char* func, const char* file, const int line, const char* fmt, ...)
{
    fprintf(stderr, COLOR_ERROR "[ERROR]: ");

    // Traitement des arguments variables avec vfprintf
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, " in file:%s at ligne:%d\n    -> %s" COLOR_RESET "\n", file, line, func);

    if(interrupt) raise(SIGINT);
}

void log_warning(const char* func, const char* file, const int line, const char* fmt, ...)
{
    fprintf(stdout, COLOR_WARNING "[WARNING]: ");
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    
    fprintf(stdout, " in file:%s at ligne:%d\n    -> %s" COLOR_RESET "\n", file, line, func);
}

void log_info(const char* fmt, ...)
{
    fprintf(stdout, COLOR_INFO "[INFO]: ");
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    
    fprintf(stdout, COLOR_RESET "\n");
}

int log_gl_error_all(const char* func, const char* file, const int line, bool interrupt)
{
    int count = 0;
    GLenum error;

    while((error = glGetError()) != GL_NO_ERROR){
        log_error(false, func, file, line, "OpenGL-Error (code:%#010x)", error);
        count++;
    }

    if(interrupt && count > 0) raise(SIGINT);

    return count;
}

void log_gl_clear_errors()
{
    while(glGetError() != GL_NO_ERROR);
}

int log_gl_check_errors()
{
    GLenum error;
    int count = 0;

    while((error = glGetError()) != GL_NO_ERROR) 
        count++;

    return count;
}
