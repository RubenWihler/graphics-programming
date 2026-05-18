#include "s_skybox.h"
#include "../../core/vendor/glad/glad.h" 

static unsigned int skyboxVAO = 0;
static unsigned int skyboxVBO = 0;

void s_skybox_init(void) {
    if (skyboxVAO != 0) return; // Déjà initialisé

    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void s_skybox_render(cubemap_t* skybox, camera_component_t* camera, shader_t* skybox_shader) {
    if (!skybox || !camera || !skybox_shader) return;

    // 1. On modifie la fonction de profondeur (GL_LEQUAL car notre shader sort Z=1.0)
    glDepthFunc(GL_LEQUAL);  
    
    shader_bind(skybox_shader);
    
    // 2. L'astuce mathématique : On retire la traduction de la matrice de vue
    mat4 view;
    glm_mat4_copy(camera->view_matrix, view);
    view[3][0] = 0.0f; // Reset X
    view[3][1] = 0.0f; // Reset Y
    view[3][2] = 0.0f; // Reset Z
    
    shader_set_uniform_mat4(skybox_shader, "u_view", view);
    shader_set_uniform_mat4(skybox_shader, "u_projection", camera->projection_matrix);
    
    // 3. On lie le cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->id);
    shader_set_uniform_1i(skybox_shader, "u_skybox", 0);

    // 4. On dessine
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // 5. On remet la profondeur à son état normal
    glDepthFunc(GL_LESS); 
}
