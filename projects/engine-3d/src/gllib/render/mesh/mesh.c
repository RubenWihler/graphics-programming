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
bool mesh_load_from_obj(mesh_t *mesh, const char *filepath) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    // Demande à tinyobj de forcer les faces en triangles
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, 
                                filepath, get_file_data, NULL, flags);

    if (ret != TINYOBJ_SUCCESS) {
        LOG_ERROR("Erreur lors du chargement du fichier OBJ : %s", filepath);
        return false;
    }

    // --- 1. Préparation des buffers finaux ---
    // Au pire des cas, chaque face a 3 sommets totalement uniques.
    // Chaque sommet fait 8 floats (3 pos + 3 norm + 2 uv).
    size_t max_vertices = attrib.num_faces * 3; 
    float* final_vertices = (float*)malloc(max_vertices * 8 * sizeof(float));
    unsigned int* final_indices = (unsigned int*)malloc(max_vertices * sizeof(unsigned int));
    
    size_t vertex_cursor = 0; // Pointeur dans le tableau des floats
    size_t index_count = 0;   // Nombre total d'indices

    // --- 2. Préparation de la table de hachage ---
    size_t hash_table_size = max_vertices;
    int* hash_table = (int*)malloc(hash_table_size * sizeof(int));
    for (size_t i = 0; i < hash_table_size; i++) hash_table[i] = -1;
    
    vertex_key_t* keys = (vertex_key_t*)malloc(max_vertices * sizeof(vertex_key_t));
    unsigned int num_unique_vertices = 0;

    // --- 3. Parcours des géométries ---
    for (size_t i = 0; i < num_shapes; i++) {
        // Au lieu de f = 0, on commence au face_offset de cette shape
        // Et on s'arrête après avoir lu 'length' faces.
        size_t offset = shapes[i].face_offset;
        size_t length = shapes[i].length;

        // Note: Dans tinyobjloader-c, attrib.faces contient tous les sommets de toutes les faces (triangulées)
        // Donc on boucle sur les indices globaux. Chaque face triangulée a 3 sommets.
        for (size_t f = 0; f < length * 3; f++) {
            
            // On récupère l'index global depuis attrib !
            tinyobj_vertex_index_t idx = attrib.faces[offset * 3 + f];

            // Fonction de hachage spatiale
            unsigned int hash = ((unsigned int)idx.v_idx * 73856093 ^ 
                                 (unsigned int)idx.vt_idx * 19349663 ^ 
                                 (unsigned int)idx.vn_idx * 83492791) % hash_table_size;

            int key_idx = hash_table[hash];
            bool found = false;
            unsigned int final_index = 0;

            // Cherche si cette combinaison exacte de sommets/uv/normale existe déjà
            while (key_idx != -1) {
                if (keys[key_idx].v_idx == idx.v_idx &&
                    keys[key_idx].vt_idx == idx.vt_idx &&
                    keys[key_idx].vn_idx == idx.vn_idx) {
                    found = true;
                    final_index = keys[key_idx].new_index;
                    break;
                }
                key_idx = keys[key_idx].next_hash;
            }

            if (!found) {
                // C'est un sommet inédit ! On l'enregistre.
                final_index = num_unique_vertices++;

                // 1. Sauvegarde dans la table de hachage
                keys[final_index].v_idx = idx.v_idx;
                keys[final_index].vt_idx = idx.vt_idx;
                keys[final_index].vn_idx = idx.vn_idx;
                keys[final_index].new_index = final_index;
                keys[final_index].next_hash = hash_table[hash];
                hash_table[hash] = final_index;

                // 2. Extraction des Positions (Attention : idx.v_idx est garanti d'être valide)
                final_vertices[vertex_cursor++] = attrib.vertices[3 * idx.v_idx + 0];
                final_vertices[vertex_cursor++] = attrib.vertices[3 * idx.v_idx + 1];
                final_vertices[vertex_cursor++] = attrib.vertices[3 * idx.v_idx + 2];

                // 3. Extraction des Normales (si présentes, sinon 0,1,0 par défaut)
                if (idx.vn_idx >= 0) {
                    final_vertices[vertex_cursor++] = attrib.normals[3 * idx.vn_idx + 0];
                    final_vertices[vertex_cursor++] = attrib.normals[3 * idx.vn_idx + 1];
                    final_vertices[vertex_cursor++] = attrib.normals[3 * idx.vn_idx + 2];
                } else {
                    final_vertices[vertex_cursor++] = 0.0f; 
                    final_vertices[vertex_cursor++] = 1.0f;
                    final_vertices[vertex_cursor++] = 0.0f;
                }

                // 4. Extraction des UVs (si présentes, sinon 0,0 par défaut)
                if (idx.vt_idx >= 0) {
                    final_vertices[vertex_cursor++] = attrib.texcoords[2 * idx.vt_idx + 0];
                    // IMPORTANT : En OpenGL, les coordonnées Y de textures sont souvent inversées
                    // Si ta texture s'affiche à l'envers, remplace la ligne en dessous par : 
                    // 1.0f - attrib.texcoords[2 * idx.vt_idx + 1];
                    final_vertices[vertex_cursor++] = attrib.texcoords[2 * idx.vt_idx + 1];
                } else {
                    final_vertices[vertex_cursor++] = 0.0f;
                    final_vertices[vertex_cursor++] = 0.0f;
                }
            }

            // On ajoute l'index au IBO final !
            final_indices[index_count++] = final_index;
        }
    }

    // --- 4. Envoi à OpenGL ---
    vertex_buffer_layout_t layout;
    vertex_buffer_layout_init(&layout);
    vertex_buffer_layout_push_float(&layout, 3); // Pos
    vertex_buffer_layout_push_float(&layout, 3); // Norm
    vertex_buffer_layout_push_float(&layout, 2); // UV

    size_t vertex_bytes = vertex_cursor * sizeof(float);
    
    // Appel de ta fonction d'initialisation de mesh
    mesh_init(mesh, final_vertices, vertex_bytes, final_indices, index_count, &layout);

    // --- 5. Nettoyage de la RAM ---
    free(final_vertices);
    free(final_indices);
    free(hash_table);
    free(keys);
    tinyobj_attrib_free(&attrib);
    if (shapes) tinyobj_shapes_free(shapes, num_shapes);
    if (materials) tinyobj_materials_free(materials, num_materials);

    // LOG_INFO("Modele OBJ charge avec succes : %s (%u sommets uniques, %zu triangles)\n", 
    //          filepath, num_unique_vertices, index_count / 3);

    return true;
}

void mesh_init(mesh_t *mesh, const float *vertices, size_t vertex_size, 
               const unsigned int *indices, size_t index_count, 
               vertex_buffer_layout_t *layout) 
{
    vertex_array_init(&mesh->vao);
    vertex_buffer_init(&mesh->vbo, vertices, vertex_size, false);

    vertex_array_add_buffer(&mesh->vao, &mesh->vbo, layout);

    index_buffer_init(&mesh->ibo, indices, index_count, false);
}

void mesh_destroy(mesh_t *mesh) {
    index_buffer_destroy(&mesh->ibo);
    vertex_buffer_destroy(&mesh->vbo);
    vertex_array_destroy(&mesh->vao);
}
