/*
*   Pour plus d'informations sur les textures, voir:
*   - https://docs.gl/gl4/glTexParameter (pour les paramètres de texture)
*   - https://docs.gl/gl4/glTexImage2D (pour les paramètres de glTexImage2D)
*
*/

#include "texture.h"
#include "../../log/log.h"
#include "../../vendor/stb_image/stb_image.h"

#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const int channels = 4;

bool texture_init(texture_t *tex, const char *filepath)
{
    memset(tex, 0, sizeof(*tex));

    tex->filepath = strdup(filepath);
    if(!tex->filepath) return (perror("strdup"), false);

    stbi_set_flip_vertically_on_load(1);
    tex->local_buffer = stbi_load(tex->filepath, &tex->width, &tex->height, &tex->bpp, channels);


    ASSERT_GL_BEGIN();
    glGenTextures(1, &tex->renderer_id);
    glBindTexture(GL_TEXTURE_2D, tex->renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//s is like x
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//t is like y

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->width, tex->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex->local_buffer);

    glBindTexture(GL_TEXTURE_2D, 0);
    if(ASSERT_GL_ERROR_OCCURED("error while creating texture"))
    {
        texture_destroy(tex);
        return false;
    }
    
    //pour l'instant on n'a pas besoin de la texture en mémoire
    stbi_image_free(tex->local_buffer);

    return true;
}

void texture_destroy(texture_t *tex)
{
    ASSERT_GL_CALL(glDeleteTextures(1, &tex->renderer_id));
    free(tex->filepath);
    tex->renderer_id = 0;
}

void texture_bind(texture_t *tex, unsigned int* slot_ptr)
{
    unsigned int slot = (slot_ptr != NULL) ? *slot_ptr : 0;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex->renderer_id);
}

void texture_unbind()
{
    ASSERT_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
