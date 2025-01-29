
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shader_parser.h"

#define SHADER_FILE_EXT ".glsl"
#define SHADER_FILE_NAME_FRAGMENT "frag"
#define SHADER_FILE_NAME_VERTEX "vert"

shader_program_source_t* shader_program_source_parse(const char* dirpath)
{
    /*
    * Cette fonction retourne un pointeur vers une structure contenant les sources des shaders
    * La memoire allouée est contigue. La structure est suivie des deux chaines de caractères.
    * 
    * 2*4 bytes pour les pointeurs
    * contenu du fichier vertex + 1 pour le '\0'
    * contenu du fichier fragment + 1 pour le '\0'
    *
    * il est de la responsabilité de l'appelant de libérer la mémoire
    */

    //calcul des tailles des chemins
    size_t pathlen, vert_pathlen, frag_pathlen;
    pathlen = strlen(dirpath) + strlen(SHADER_FILE_EXT) + 2;//+2 pour le '/' et le '\0'
    vert_pathlen = pathlen + strlen(SHADER_FILE_NAME_VERTEX);
    frag_pathlen = pathlen + strlen(SHADER_FILE_NAME_FRAGMENT);
    
    //allocation sur la stack des chemins
    char *vertpath, *fragpath;
    vertpath = alloca(sizeof(char) * vert_pathlen);
    fragpath = alloca(sizeof(char) * frag_pathlen);
    
    //concatenation des chemins
    strcpy(vertpath, dirpath);
    strcpy(fragpath, dirpath);
    strcat(vertpath, "/" SHADER_FILE_NAME_VERTEX SHADER_FILE_EXT);
    strcat(fragpath, "/" SHADER_FILE_NAME_FRAGMENT SHADER_FILE_EXT);
    vertpath[vert_pathlen] = '\0';
    fragpath[frag_pathlen] = '\0';
    
    //ouverture des fichiers
    FILE *vertfile, *fragfile;
    vertfile = fopen(vertpath, "r");
    fragfile = fopen(fragpath, "r");
    
    //gestion des erreurs
    if(!vertfile || !fragfile)
    {
        fprintf(stderr, "error while oppening shaders file!\n");
        return NULL;
    }
    
    //calcul des tailles des fichiers
    long vert_file_length, frag_file_length;
    fseek(vertfile, 0, SEEK_END);
    fseek(fragfile, 0, SEEK_END);
    vert_file_length = ftell(vertfile);
    frag_file_length = ftell(fragfile);
    fseek(vertfile, 0, SEEK_SET);
    fseek(fragfile, 0, SEEK_SET);

    //allocation du buffer
    size_t buffer_len = vert_file_length + frag_file_length + 2;
    void *buffer = malloc(sizeof(shader_program_source_t) + (sizeof(char) * buffer_len));
    char *srcbuffer = (char*)(((shader_program_source_t*)buffer)+1);

    size_t i = 0;
    char c = 0;
    
    //lecture du fichier vertex
    char *vertbuffer = srcbuffer;
    while((c = fgetc(vertfile)) != EOF)
        srcbuffer[i++] = c;
    srcbuffer[i++] = '\0';
    
    //lecture du fichier fragment
    char *fragbuffer = srcbuffer+i;
    while((c = fgetc(fragfile)) != EOF)
        srcbuffer[i++] = c;
    srcbuffer[i] = '\0';
    
    //fermeture des fichiers
    fclose(vertfile);
    fclose(fragfile);

    //remplissage de la structure
    *((shader_program_source_t*)buffer) = (shader_program_source_t){
        .vertex = vertbuffer,
        .fragment = fragbuffer
    };
    
    return buffer;
}

void print_shader_program_source(shader_program_source_t *src)
{
    printf("(shader_program_source_t)@{%p}\n", src);
    printf("vertex:                  @{%p}\n", src->vertex);
    printf("fragment:                @{%p}\n", src->fragment);
    printf("vertex as string:\n%s\n", src->vertex);
    printf("fragment as string:\n%s\n", src->fragment);
}
