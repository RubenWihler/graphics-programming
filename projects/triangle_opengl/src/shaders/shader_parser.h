#pragma once

typedef struct _shader_program_source_t {
    const char* vertex;
    const char* fragment;
} shader_program_source_t;

shader_program_source_t* shader_program_source_parse(const char* dirpath);
void print_shader_program_source(shader_program_source_t *src);
