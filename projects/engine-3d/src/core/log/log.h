#pragma once

#include <stdbool.h>
#include <assert.h>

// --- CONSTANTES DE COULEURS ---
#define COLOR_ERROR   "\e[0;31m" // Rouge
#define COLOR_WARNING "\e[0;33m" // Jaune
#define COLOR_INFO    "\e[0;36m" // Cyan
#define COLOR_RESET   "\e[0m"    // Reset

#ifdef DEBUG
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

    #define LOG_ERROR(fmt, ...) log_error(false, __func__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_ERROR_FATAL(fmt, ...) log_error(true, __func__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_WARNING(fmt, ...)          log_warning(__func__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...)             log_info(fmt, ##__VA_ARGS__)
#else
    #define ASSERT_GL_BEGIN() ;
    #define ASSERT_GL_ERROR_OCCURED(msg) 0
    #define ASSERT_GL(msg) ;
    #define ASSERT_GL_CALL(func) func
    
    #define LOG_ERROR(fmt, ...) ;
    #define LOG_ERROR_FATAL(fmt, ...) ;
    #define LOG_WARNING(fmt, ...)          ;
    #define LOG_INFO(fmt, ...)             ;
#endif

void log_info(const char* fmt, ...);
void log_warning(const char* func, const char* file, const int line, const char* fmt, ...);
void log_error(bool interrupt, const char* func, const char* file, const int line, const char* fmt, ...);

int log_gl_error_all(const char* func, const char* file, const int line, bool interrupt);
void log_gl_clear_errors();
int log_gl_check_errors();
