#include "mesh.h"
#include "../../log/log.h"

// Définir l'implémentation de la librairie (à faire dans UN SEUL fichier .c)
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "../../vendor/obj_loader/tinyobj_loader_c.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// -----------------------------------------------------------------------------
// Callback pour tinyobjloader (lit le contenu du fichier et le met en RAM)
// -----------------------------------------------------------------------------
static void get_file_data(void* ctx, const char* filename, const int is_mtl,
                          const char* obj_filename, char** data, size_t* len) {
    (void)ctx; (void)is_mtl; (void)obj_filename;
    FILE* file = fopen(filename, "rb");
    if (!file) {
        *data = NULL;
        *len = 0;
        return;
    }
    fseek(file, 0, SEEK_END);
    *len = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);
    *data = (char*)malloc(*len + 1);
    if (*data) {
        fread(*data, 1, *len, file);
        (*data)[*len] = '\0'; // Null-terminate la string
    }
    fclose(file);
}

// Structure locale pour notre table de hachage ultra-rapide
typedef struct {
    int v_idx, vt_idx, vn_idx; // Les 3 indices du fichier .obj
    unsigned int new_index;    // Le nouvel index unique pour OpenGL
    int next_hash;             // Pour gérer les collisions de hash
} vertex_key_t;


// -----------------------------------------------------------------------------
// La fonction de chargement principale
// -----------------------------------------------------------------------------
bool model_load_from_obj(model_t *model, const char *filepath, const char *base_dir) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* tiny_materials = NULL;
    size_t num_materials;

    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &tiny_materials, &num_materials, 
                                filepath, get_file_data, NULL, flags);

    if (ret != TINYOBJ_SUCCESS) {
        LOG_ERROR("Erreur chargement OBJ : %s !\n", filepath);
        return false;
    }

    // --- 1. CHARGEMENT DES MATERIAUX ---
    model->material_count = num_materials;
    if (num_materials > 0) {
        model->materials = (material_t*)calloc(num_materials, sizeof(material_t));
        
        for (size_t i = 0; i < num_materials; i++) {
            material_init_default(&model->materials[i]);
            if (tiny_materials[i].name) strncpy(model->materials[i].name, tiny_materials[i].name, 63);
            
            glm_vec3_copy(tiny_materials[i].ambient, model->materials[i].ambient);
            glm_vec3_copy(tiny_materials[i].diffuse, model->materials[i].diffuse);
            glm_vec3_copy(tiny_materials[i].specular, model->materials[i].specular);
            model->materials[i].shininess = tiny_materials[i].shininess;

            // Si le matériau a une texture, on prépare le chargement
            if (tiny_materials[i].diffuse_texname != NULL) {
                char full_tex_path[512];
                snprintf(full_tex_path, sizeof(full_tex_path), "%s%s", base_dir, tiny_materials[i].diffuse_texname);
                
                model->materials[i].diffuse_map = (texture_t*)malloc(sizeof(texture_t));
                if (!texture_init(model->materials[i].diffuse_map, full_tex_path)) {
                    LOG_ERROR("Impossible de charger la texture : %s", full_tex_path);
                    free(model->materials[i].diffuse_map);
                    model->materials[i].diffuse_map = NULL;
                }
            }
        }
    }

// --- 2. DÉCOUPAGE EN SUBMESHES PAR MATÉRIAU (Correction Quads/Triangles) ---
    int fallback_mat = num_materials; // Index virtuel pour les faces sans matériau
    int* tri_count_per_mat = (int*)calloc(num_materials + 1, sizeof(int));
    
    // 2.a Compter le nombre de triangles exact pour chaque matériau
    for (size_t i = 0; i < attrib.num_face_num_verts; i++) {
        int mat = attrib.material_ids[i];
        if (mat < 0 || mat >= (int)num_materials) mat = fallback_mat;
        tri_count_per_mat[mat]++;
    }

    // 2.b Compter combien de matériaux sont réellement utilisés
    size_t active_submeshes = 0;
    for (size_t m = 0; m <= num_materials; m++) {
        if (tri_count_per_mat[m] > 0) active_submeshes++;
    }

    model->submesh_count = active_submeshes;
    model->submeshes = (submesh_t*)calloc(active_submeshes, sizeof(submesh_t));
    size_t current_submesh = 0;

    // --- 3. GÉNÉRATION DES BUFFERS POUR CHAQUE MATÉRIAU ---
    for (size_t m = 0; m <= num_materials; m++) {
        if (tri_count_per_mat[m] == 0) continue; // On ignore les matériaux non utilisés

        size_t max_vertices = tri_count_per_mat[m] * 3; 
        float* final_vertices = (float*)malloc(max_vertices * 8 * sizeof(float));
        unsigned int* final_indices = (unsigned int*)malloc(max_vertices * sizeof(unsigned int));
        size_t vertex_cursor = 0, index_count = 0;

        size_t hash_table_size = max_vertices;
        int* hash_table = (int*)malloc(hash_table_size * sizeof(int));
        for (size_t h = 0; h < hash_table_size; h++) hash_table[h] = -1;
        vertex_key_t* keys = (vertex_key_t*)malloc(max_vertices * sizeof(vertex_key_t));
        unsigned int num_unique_vertices = 0;

        size_t global_index_offset = 0;
        
        // On parcourt TOUS les triangles générés par tinyobjloader de manière sûre
        for (size_t f = 0; f < attrib.num_face_num_verts; f++) {
            int num_verts = attrib.face_num_verts[f]; // Vaut 3 grâce à TINYOBJ_FLAG_TRIANGULATE
            int face_mat = attrib.material_ids[f];
            if (face_mat < 0 || face_mat >= (int)num_materials) face_mat = fallback_mat;

            // Si ce triangle appartient au submesh/matériau en cours de construction
            if (face_mat == (int)m) {
                for (int v = 0; v < num_verts; v++) {
                    tinyobj_vertex_index_t idx = attrib.faces[global_index_offset + v];

                    unsigned int hash = ((unsigned int)idx.v_idx * 73856093 ^ 
                                         (unsigned int)idx.vt_idx * 19349663 ^ 
                                         (unsigned int)idx.vn_idx * 83492791) % hash_table_size;

                    int key_idx = hash_table[hash];
                    bool found = false;
                    unsigned int final_index = 0;

                    while (key_idx != -1) {
                        if (keys[key_idx].v_idx == idx.v_idx && keys[key_idx].vt_idx == idx.vt_idx && keys[key_idx].vn_idx == idx.vn_idx) {
                            found = true; final_index = keys[key_idx].new_index; break;
                        }
                        key_idx = keys[key_idx].next_hash;
                    }

                    if (!found) {
                        final_index = num_unique_vertices++;
                        keys[final_index].v_idx = idx.v_idx; keys[final_index].vt_idx = idx.vt_idx; keys[final_index].vn_idx = idx.vn_idx;
                        keys[final_index].new_index = final_index; keys[final_index].next_hash = hash_table[hash]; hash_table[hash] = final_index;

                        final_vertices[vertex_cursor++] = attrib.vertices[3 * idx.v_idx + 0];
                        final_vertices[vertex_cursor++] = attrib.vertices[3 * idx.v_idx + 1];
                        final_vertices[vertex_cursor++] = attrib.vertices[3 * idx.v_idx + 2];

                        if (idx.vn_idx >= 0) {
                            final_vertices[vertex_cursor++] = attrib.normals[3 * idx.vn_idx + 0];
                            final_vertices[vertex_cursor++] = attrib.normals[3 * idx.vn_idx + 1];
                            final_vertices[vertex_cursor++] = attrib.normals[3 * idx.vn_idx + 2];
                        } else { final_vertices[vertex_cursor++] = 0; final_vertices[vertex_cursor++] = 1; final_vertices[vertex_cursor++] = 0; }

                        if (idx.vt_idx >= 0) {
                            final_vertices[vertex_cursor++] = attrib.texcoords[2 * idx.vt_idx + 0];
                            final_vertices[vertex_cursor++] = 1.0f - attrib.texcoords[2 * idx.vt_idx + 1];
                        } else { final_vertices[vertex_cursor++] = 0; final_vertices[vertex_cursor++] = 0; }
                    }
                    final_indices[index_count++] = final_index;
                }
            }
            global_index_offset += num_verts; // On avance de 3 indices dans le tableau global
        }

        // Création des Buffers OpenGL pour ce Submesh
        model->submeshes[current_submesh].material_index = (m == (size_t)fallback_mat) ? -1 : (int)m;
        
        vertex_buffer_layout_t layout;
        vertex_buffer_layout_init(&layout);
        vertex_buffer_layout_push_float(&layout, 3); // Pos
        vertex_buffer_layout_push_float(&layout, 3); // Norm
        vertex_buffer_layout_push_float(&layout, 2); // UV

        vertex_array_init(&model->submeshes[current_submesh].vao);
        vertex_buffer_init(&model->submeshes[current_submesh].vbo, final_vertices, vertex_cursor * sizeof(float), false);
        vertex_array_add_buffer(&model->submeshes[current_submesh].vao, &model->submeshes[current_submesh].vbo, &layout);
        index_buffer_init(&model->submeshes[current_submesh].ibo, final_indices, index_count, false);

        free(final_vertices); free(final_indices); free(hash_table); free(keys);
        current_submesh++;
    }
    
    free(tri_count_per_mat);

    // --- 4. Nettoyage tinyobj ---
    tinyobj_attrib_free(&attrib);
    if (shapes) tinyobj_shapes_free(shapes, num_shapes);
    if (tiny_materials) tinyobj_materials_free(tiny_materials, num_materials);

    LOG_INFO("Model loaded: %s (%zu submeshes, %zu materials)", filepath, num_shapes, num_materials);

    return true;
}

void model_destroy(model_t *model)
{
    for (size_t i = 0; i < model->submesh_count; i++){
        mesh_destroy(&model->submeshes[i]);
    }

    for (size_t i = 0; i < model->material_count; i++){
        material_t mat = model->materials[i];
        if (mat.diffuse_map) texture_destroy(mat.diffuse_map);
    }

}

void mesh_destroy(submesh_t *mesh) {
    index_buffer_destroy(&mesh->ibo);
    vertex_buffer_destroy(&mesh->vbo);
    vertex_array_destroy(&mesh->vao);
}

// void mesh_init(mesh_t *mesh, const float *vertices, size_t vertex_size, 
//                const unsigned int *indices, size_t index_count, 
//                vertex_buffer_layout_t *layout) 
// {
//     vertex_array_init(&mesh->vao);
//     vertex_buffer_init(&mesh->vbo, vertices, vertex_size, false);
//
//     vertex_array_add_buffer(&mesh->vao, &mesh->vbo, layout);
//
//     index_buffer_init(&mesh->ibo, indices, index_count, false);
// }
//
