#pragma once

#include <GLFW/glfw3.h>
#include <signal.h>

#ifdef DEBUG
    #define GLCALL(func) gl_clear_errors();\
        func;\
        if (gl_log_call(#func, __FILE__, __LINE__) > 0) raise(SIGINT);
#else
    #define GLCALL(func) func
#endif

void gl_clear_errors();
int gl_log_call(const char* func, const char* file, const int line);
