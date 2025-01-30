#pragma once

#include <stdbool.h>
#include <assert.h>

#ifdef DEBUG
    //ces macros ne sont seulement actives en mode DEBUG
    
    //juste un alias pour log_gl_clear_errors()
    #define ASSERT_GL_BEGIN() log_gl_clear_errors();
    
    //assertion sur les erreurs OpenGL sans interruption du programme. Retournent le nombre d'erreurs.
    #define ASSERT_GL_ERROR_OCCURED(msg) log_gl_error_all(msg, __FILE__, __LINE__, false)

    //assertion sur les erreurs OpenGL avec interruption du programme .
    //a utiliser après plusieurs appels OpenGL (appeler log_gl_clear_errors() avant).
    //si il n'y a que un seul appel OpenGL, l'utilisation de ASSERT_GLCALL est préférée,
    //car elle appelle log_gl_clear_errors() automatiquement et affiche le nom de la fonction OpenGL dans le message d'erreur.
    #define ASSERT_GL(msg) log_gl_error_all(msg, __FILE__, __LINE__, true);
    
    //assertion sur les erreurs OpenGL avec interruption du programme.
    //A utiliser pour un seul appel OpenGL.
    //Affiche le nom de la fonction OpenGL dans le message d'erreur. 
    //Appelle log_gl_clear_errors() automatiquement.
    //Si plusieurs appels OpenGL sont effectués, utiliser ASSERT_GL.
    #define ASSERT_GL_CALL(func) log_gl_clear_errors(); func; log_gl_error_all(#func, __FILE__, __LINE__, true);
#else
    #define ASSERT_GL_BEGIN() ;
    #define ASSERT_GL_ERROR_OCCURED(msg) 0
    #define ASSERT_GL(msg) ;
    #define ASSERT_GL_CALL(func) func
#endif

void log_error(char* msg, const char* func, const char* file, const int line, bool interrupt);
int log_gl_error_all(const char* func, const char* file, const int line, bool interrupt);

void log_gl_clear_errors();
int log_gl_check_errors();
