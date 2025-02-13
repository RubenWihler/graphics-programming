#include "gl_utils.h"

#include <GL/glew.h>
#include <stdio.h>
#include <assert.h>

unsigned int gl_get_type_size(const unsigned int type)
{
    switch(type)
    {
        case GL_FLOAT: return sizeof(float);
        case GL_UNSIGNED_INT: return sizeof(unsigned int);
        case GL_UNSIGNED_BYTE: return sizeof(unsigned char);
    }

    fprintf(stderr, "gl_get_type_size(%u): Unknown type\n", type);
    assert(0);
    return 0;
}
