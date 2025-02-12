#include "vertex_array.h"
#include "../../utils/gl_utils/gl_utils.h"
#include "../../log/log.h"

#include <GL/glew.h>

bool vertex_array_init(vertex_array_t *va)
{
    ASSERT_GL_BEGIN();

    //On crée un vertex array object
    glGenVertexArrays(1, &va->renderer_id);

    if (ASSERT_GL_ERROR_OCCURED("error while creating vertex array"))
    {
        vertex_array_destroy(va);
        return false;
    }

    return true;
}

void vertex_array_destroy(vertex_array_t *va)
{
    ASSERT_GL_CALL(glDeleteVertexArrays(1, &va->renderer_id));
    va->renderer_id = 0;
}

void vertex_array_bind(const vertex_array_t *va)
{
    ASSERT_GL_CALL(glBindVertexArray(va->renderer_id));
}

void vertex_array_unbind(const vertex_array_t *va)
{
    unsigned int bound_vao;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (int*)&bound_vao);

    if(bound_vao == va->renderer_id)
        ASSERT_GL_CALL(glBindVertexArray(0));
}

void vertex_array_add_buffer(vertex_array_t* va, const vertex_buffer_t *vb, const vertex_buffer_layout_t* layout)
{
    vertex_array_bind(va);  //on bind le VAO
    vertex_buffer_bind(vb); //on bind le buffer
    size_t offest = 0;
    
    for(size_t i = 0; i < dyn_array_count(layout->elements); i++)
    {
        vertex_buffer_element_t *element = dyn_array_get(layout->elements, i);
        ASSERT_GL_CALL(glEnableVertexAttribArray(i));
        ASSERT_GL_CALL(glVertexAttribPointer(
            i,
            element->count,
            element->type,
            element->normalized,
            layout->stride,
            (const void*)offest
        ));
    
        offest += element->count * gl_get_type_size(element->type);
    }
}
