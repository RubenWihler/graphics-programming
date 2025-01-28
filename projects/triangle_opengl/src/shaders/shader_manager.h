#pragma once

#include <GL/glew.h>

/// @brief Cree un programme OpenGL a partir de deux shaders
/// @param vertex_src Le code source du vertex shader
/// @param fragment_src Le code source du fragment shader
/// @return L'identifiant du programme OpenGL cree
/// @note les code source des shaders doivent etres null-terminated
unsigned int create_shader_program(const char* vertex_src, const char* fragment_src);

/// @brief Compile un shader OpenGL et retourne son id
/// @param type Le type du shader (GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER)
/// @param src Le code source du shader
/// @return L'identifiant du shader OpenGL compile
/// @note le code source du shader doit etre null-terminated
/// @note retourn 0 si erreur de compilation
unsigned int compile_shader(const unsigned int type, const char* src);
