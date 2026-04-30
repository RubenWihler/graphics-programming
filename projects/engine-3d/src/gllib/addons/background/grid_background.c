#include "grid_background.h"
#include "../../vendor/glad/glad.h"

#define SQUARE_COLOR 0.145f, 0.149f, 0.188f, 1.0f
#define BACK_COLOR   0.369f, 0.373f, 0.451f, 1.0f

static const size_t row_count = 1000;
static const size_t col_count = 1000;
static const float square_size = 10.0f;
static const float gap = 0.5f;

static void update_vertex_buffer(grid_background_t *grid);

bool grid_background_init(grid_background_t *grid)
{
    grid->vertices = (float*)malloc(4 * 2 * row_count * col_count * sizeof(float));
    if(!grid->vertices) return (perror("malloc"), false);

    grid->indices = (unsigned int*)malloc(6 * row_count * col_count * sizeof(unsigned int));
    if(!grid->indices) return (perror("malloc"), false);

    vertex_array_init(&grid->vao);
    vertex_buffer_init(&grid->vbo, grid->vertices, 4 * 2 * row_count * col_count * sizeof(float), true);
    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 2);
    index_buffer_init(&grid->ibo, NULL, 6 * row_count * col_count, true);
    vertex_array_add_buffer(&grid->vao, &grid->vbo, &layout);

    update_vertex_buffer(grid);

    shader_init(&grid->shader, "res/shaders/grid_bg_flat");
    shader_bind(&grid->shader);
    shader_set_uniform(&grid->shader, "u_color", (vec4){SQUARE_COLOR});

    return true;
}

void grid_background_destroy(grid_background_t *grid)
{
    free(grid->vertices);
    free(grid->indices);

    vertex_buffer_destroy(&grid->vbo);
    index_buffer_destroy(&grid->ibo);
    vertex_array_destroy(&grid->vao);
}

void grid_background_render(grid_background_t *grid, cam_ortho_t *cam, renderer_t *renderer)
{
    glClearColor(BACK_COLOR);

    //on translate de la moitie de la position de la camera
    //pour un effet de parallaxe
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){-cam->position[0] / 5.0f, -cam->position[1] / 5.0f, 0.0f});

    shader_bind(&grid->shader);
    shader_set_uniform_mat4(&grid->shader, "u_model", model);

    renderer_draw(renderer, &grid->vao, &grid->ibo, &grid->shader);
}

static void update_vertex_buffer(grid_background_t *grid)
{
    //simple batch rendering
    //chaque vertex est un coin d'un carre
    //chaque carre est compose de 2 triangles
    //on s'occupera de la position de la camera avec un uniform
    //on part de la position 0x 0y pour le coin en bas a gauche
    //on ajoute les carres de gauche a droite, puis de bas en haut

    float start_x = -((col_count * square_size + (col_count - 1) * gap) / 2.0f);
    float start_y = -((row_count * square_size + (row_count - 1) * gap) / 2.0f);

    float x = start_x;
    float y = start_y;

    for(size_t i = 0; i < row_count; i++)
    {
        for(size_t j = 0; j < col_count; j++)
        {
            //coin bas gauche
            grid->vertices[8 * (i * col_count + j) + 0] = x;
            grid->vertices[8 * (i * col_count + j) + 1] = y;

            //coin bas droit
            grid->vertices[8 * (i * col_count + j) + 2] = x + square_size;
            grid->vertices[8 * (i * col_count + j) + 3] = y;

            //coin haut droit
            grid->vertices[8 * (i * col_count + j) + 4] = x + square_size;
            grid->vertices[8 * (i * col_count + j) + 5] = y + square_size;

            //coin haut gauche
            grid->vertices[8 * (i * col_count + j) + 6] = x;
            grid->vertices[8 * (i * col_count + j) + 7] = y + square_size;

            //on ajoute les indices pour les 2 triangles
            grid->indices[6 * (i * col_count + j) + 0] = 4 * (i * col_count + j) + 0;
            grid->indices[6 * (i * col_count + j) + 1] = 4 * (i * col_count + j) + 1;
            grid->indices[6 * (i * col_count + j) + 2] = 4 * (i * col_count + j) + 2;
            grid->indices[6 * (i * col_count + j) + 3] = 4 * (i * col_count + j) + 2;
            grid->indices[6 * (i * col_count + j) + 4] = 4 * (i * col_count + j) + 3;
            grid->indices[6 * (i * col_count + j) + 5] = 4 * (i * col_count + j) + 0;

            x += square_size + gap;
        }

        x = start_x;
        y += square_size + gap;
    }
    
    vertex_buffer_set_data(&grid->vbo, grid->vertices, 4 * 2 * row_count * col_count * sizeof(float));
    index_buffer_set_data(&grid->ibo, grid->indices, 6 * row_count * col_count);
}
