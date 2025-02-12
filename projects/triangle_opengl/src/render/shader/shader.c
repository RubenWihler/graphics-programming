#include "shader.h"
#include "shader_parser.h"
#include "../../log/log.h"

#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const size_t uniform_cache_initial_capacity = 8;

static bool shader_build(shader_t *shader);
static unsigned int shader_compile(const unsigned int type, const char* source);
static unsigned int shader_create_program(const char* vertex_src, const char* fragment_src);
static int shader_get_uniform_location(const shader_t *shader, const char* name);

bool shader_init(shader_t *shader, const char* dirpath)
{
    memset(shader, 0, sizeof(*shader));

    //copie le chemin du répertoire
    shader->dirpath = strdup(dirpath);
    if(!shader->dirpath) return (perror("strdup"), false);

    //build le shader
    if(!shader_build(shader))
    {
        LOG_ERROR("Cannot build shader", true);
        return false;
    }

    //initialise la hashmap des uniforms
    shader->uniforms = hashmap_create(uniform_cache_initial_capacity, HASH_FUNC_DJB2, 
                                      sizeof(char*), sizeof(int));

    hashmap_set_fn_compare(shader->uniforms, HASHMAP_COMPARE_STRING);
    hashmap_set_fn_alloc_copy_key(shader->uniforms, HASHMAP_ALLOC_COPY_STRING);

    return true;
}

void shader_destroy(shader_t *shader)
{
    ASSERT_GL_CALL(glDeleteProgram(shader->renderer_id));
    hashmap_destroy(shader->uniforms);
}

void shader_bind(const shader_t *shader)
{
    ASSERT_GL_CALL(glUseProgram(shader->renderer_id));
}

void shader_unbind(__attribute__((unused)) const shader_t *shader)
{
    ASSERT_GL_CALL(glUseProgram(0));
}


void shader_set_uniform_1i(const shader_t *shader, const char* name, int v0)
{ ASSERT_GL_CALL(glUniform1i(shader_get_uniform_location(shader, name), v0)); }

void shader_set_uniform_1f(const shader_t *shader, const char* name, float v0)
{ ASSERT_GL_CALL(glUniform1f(shader_get_uniform_location(shader, name), v0)); }

void shader_set_uniform_4f(const shader_t *shader, const char* name, float v0, float v1, float v2, float v3)
{ ASSERT_GL_CALL(glUniform4f(shader_get_uniform_location(shader, name), v0, v1, v2, v3)); }

void shader_set_uniform_mat4(const shader_t *shader, const char* name, mat4 v0)
{ ASSERT_GL_CALL(glUniformMatrix4fv(shader_get_uniform_location(shader, name), 1, GL_FALSE, &v0[0][0])); }

static int shader_get_uniform_location(const shader_t *shader, const char* name)
{
    int *location_ptr = hashmap_get(shader->uniforms, name);
    if(location_ptr) return *location_ptr;

    int location = glGetUniformLocation(shader->renderer_id, name);
    if(location == -1)
    {
        char errmsg[255];
        memset(errmsg, 0, 255);
        sprintf(errmsg, "Cannot find uniform location : %s !", name);
        LOG_ERROR(errmsg, false);    
    }

    hashmap_add(shader->uniforms, name, &location);    
    return location;
}

static bool shader_build(shader_t *shader)
{
    //parse les fichiers sources
    shader_program_source_t *src = shader_program_source_parse(shader->dirpath);
    if(!src)
    {
        LOG_ERROR("Cannot parse shader program source", false);
        return false;
    }

    //compile les shaders
    shader->renderer_id = shader_create_program(src->vertex, src->fragment);
    if(shader->renderer_id == 0)
    {
        LOG_ERROR("Cannot parse shader program source", false);
        return false;
    }

    //libere la memoire des sources
    shader_program_source_destroy(src);

    return true;
}

static unsigned int shader_create_program(const char* vertex_src, const char* fragment_src)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = shader_compile(GL_VERTEX_SHADER, vertex_src);
    unsigned int fs = shader_compile(GL_FRAGMENT_SHADER, fragment_src);

    if(vs == 0 || fs == 0)
    {
        LOG_ERROR("Cannot create shader program due to a compilation error!", false);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static unsigned int shader_compile(const unsigned int type, const char* source)
{
    const char *src = &source[0];

    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char *msg = alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, NULL, msg);

        fprintf(stderr, "Error while compiling %s shader : %s\n", 
                (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", msg);

        glDeleteShader(id);
        return 0;
    }

    return id;
}

