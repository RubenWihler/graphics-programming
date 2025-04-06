// Author: Ruben Wihler
// Date: 2025-01-29

/*
*   Ce fichier contient les fonctions de lecture des fichiers de shader.
*   Les fichiers de shader sont des fichiers texte contenant le code source des shaders OpenGL (GLSL).
*   Pour l'instant, seuls les shaders de type vertex et fragment sont supportés.
*
*   Organisation des fichiers de shader:
*   - Chaque paire de fichiers de shader (vertex et fragment) est placée dans un dossier qui porte le nom du shader.
*   - Les fichiers de shader doivent être nommés `vert.glsl` et `frag.glsl`. (en tout cas pour l'instant voir #defines)
*   - Exemple:
*        - res/shaders/basic/
*            - vert.glsl
*            - frag.glsl
*
*   Note:
*   Il est possible de ne pas avoir de fichier vertex ou fragment. Cela provoquera un warning en mode de compilation DEBUG.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "shader_parser.h"

#define SHADER_FILE_EXT ".glsl"
#define SHADER_FILE_NAME_FRAGMENT "frag"
#define SHADER_FILE_NAME_VERTEX "vert"

static char* get_file_content(const char* filepath);

shader_program_source_t* shader_program_source_parse(const char* dirpath)
{
    //calcul des tailles des chemins
    size_t pathlen, vert_pathlen, frag_pathlen;
    pathlen = strlen(dirpath) + strlen(SHADER_FILE_EXT) + 2;//+2 pour le '/' et le '\0'
    vert_pathlen = pathlen + strlen(SHADER_FILE_NAME_VERTEX);
    frag_pathlen = pathlen + strlen(SHADER_FILE_NAME_FRAGMENT);
    
    //allocation sur la stack des chemins
    char *vertpath, *fragpath;
    vertpath = (char*)alloca(vert_pathlen * sizeof(*vertpath));
    fragpath = (char*)alloca(frag_pathlen * sizeof(*fragpath));
    
    //concatenation des chemins
    strcpy(vertpath, dirpath);
    strcpy(fragpath, dirpath);
    strcat(vertpath, "/" SHADER_FILE_NAME_VERTEX SHADER_FILE_EXT);
    strcat(fragpath, "/" SHADER_FILE_NAME_FRAGMENT SHADER_FILE_EXT);
    vertpath[vert_pathlen-1] = '\0';
    fragpath[frag_pathlen-1] = '\0';
    
    //allocation du shader_program_source_t
    shader_program_source_t *program = (shader_program_source_t*)malloc(sizeof(*program));
    if(!program) return (perror("malloc"), NULL);
    
    //lecture des fichiers
    program->vertex = get_file_content(vertpath);
    program->fragment = get_file_content(fragpath);

    #ifdef DEBUG
    
    if(!program->vertex)
        fprintf(stderr, "Cannot read vertex shader file: %s\n", vertpath);

    if(!program->fragment)
        fprintf(stderr, "Cannot read fragment shader file: %s\n", fragpath);

    if(!program->vertex || !program->fragment)
    {
        fprintf(stderr, "shader_program_source_parse: "
                "Some shaders files are set to NULL. This may be intentional.\n");
    }

    #endif

    return program;
}

void shader_program_source_destroy(shader_program_source_t* src)
{
    assert(src && "shader_program_source_destroy: src to destroy is NULL");

    free((void*)src->vertex);
    free((void*)src->fragment);
    free(src);
}

/// Retourne NULL en cas d'erreur.
/// Le contenu du fichier est alloué sur le tas et doit être libéré.
static char* get_file_content(const char* filepath)
{
    //ouverture du fichier
    FILE *file = fopen(filepath, "r");
    if(!file) return NULL;

    //calcul de la taille du fichier
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    //allocation du contenu
    char* content = (char*)malloc((length+1) * sizeof(char));
    if(!content) return (perror("malloc"), fclose(file), NULL);
    
    //lecture du fichier
    size_t i = 0; char c;
    while((c = fgetc(file)) != EOF)
        content[i++] = c;
    content[i] = '\0';

    //fermeture du fichier
    fclose(file);
    return content;
}
