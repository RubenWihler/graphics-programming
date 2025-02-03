#pragma once

typedef struct _shader_program_source_t {
    const char* vertex;
    const char* fragment;
} shader_program_source_t;

/// @brief Parse un répertoire contenant les fichiers vertex et fragment
/// @param dirpath Chemin du répertoire contenant les fichiers vertex et fragment
/// @return Un pointeur vers une structure shader_program_source_t
/// @note La structure doit être libérée avec shader_program_source_destroy
/// @see shader_program_source_destroy
shader_program_source_t* shader_program_source_parse(const char* dirpath);

/// @brief Libère la mémoire allouée par shader_program_source_parse
/// @param src Pointeur vers la structure à libérer
void shader_program_source_destroy(shader_program_source_t* src);
