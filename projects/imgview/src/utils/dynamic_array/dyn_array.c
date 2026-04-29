#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "dyn_array.h"

/* 
 * 1. Introduction
*  Implémentation d'un tableau dynamique générique.
*  Les éléments sont stockés dans un espace mémoire contigu sur le tas. 
*  La taille des éléments est fixe et définie à la création du tableau.
*  La capacité du tableau est ajustée automatiquement en fonction du nombre d'éléments.
*  
*  2. Insertion
*  Lors d'une insertion, la valeur est copiée dans le tableau.
*  Si la capacité est insuffisante, le tableau est redimensionné (auto_growth_factor * capacité).
*  Si l'insertion est en fin de tableau, la valeur est ajoutée à la fin
*  Sinon, les éléments après l'index sont décalés d'une case vers la droite.
*
*  3. Suppression
*  Lors d'une suppression, les éléments après l'index sont décalés d'une case vers la gauche.
*  Si le nombre d'éléments est inférieur au quart de la capacité(auto_shrink_condition), 
*  le tableau est redimensionné (capacité / auto_shrink_factor).
*
*  4. Reservation
*  Pour des performances optimales, il est recommandé d'utiliser dyn_array_reserve pour ajouter un grand nombre d'éléments.
*  De même, il est recommandé d'utiliser dyn_array_reserve pour éviter une copie inutile.(si c'est dans un endroit critique)
*
*  5. Destruction des éléments
*  Si le tableau contient des pointeurs, ou des éléments qui nécessitent une destruction spécifique, 
*  il est possible de spécifier une fonction de destruction lors de la création du tableau.
*  Cette fonction est appelée quand un élément est retiré du tableau ou quand le tableau est détruit.
*  Voici la listes des fonctions qui l'utilisent:
*       - dyn_array_set()       : détruit l'ancien élément avant de copier la nouvelle valeur
*       - dyn_array_pop_back()  : détruit le dernier élément
*       - dyn_array_remove()    : détruit l'élément à l'index donné
*       - dyn_array_clear()     : détruit tous les éléments
*       - dyn_array_destroy()   : détruit tous les éléments
*/

typedef void (*destroy_fn_t)(void*);

struct _dyn_array_t{
    size_t capacity;
    size_t count;
    size_t size;
    void* data;
    destroy_fn_t destroy_element;
};

static void destroy_elements(dyn_array_t *arr, void* start, size_t count);
static void resize(dyn_array_t *arr, const size_t new_capacity);
static void auto_growth(dyn_array_t *arr);
static void auto_shrink(dyn_array_t *arr);

static inline void* get_at(dyn_array_t *arr, const size_t index);

static const size_t auto_growth_factor = 2;//on double la capacité
static const size_t auto_shrink_factor = 2;//on réduit la capacité de moitié
static const size_t minimal_capacity = 1;

//on agrandit le tableau si le nombre d'élements est egal à la capacité
static inline bool auto_growth_condition(dyn_array_t *arr) 
    { return arr->count == arr->capacity; }

//on ne réduit que si le nombre d'éléments est inférieur ou égal à la capacité / 4
static inline bool auto_shrink_condition(dyn_array_t *arr) 
    { return arr->count <= arr->capacity >> 2; }

dyn_array_t* dyn_array_create(const size_t initial_capacity, const size_t element_size, 
                              destroy_fn_t destroy_element)
{
    assert(element_size > 0);
    
    dyn_array_t *arr = malloc(sizeof(*arr));
    if(!arr) return (perror("malloc"), NULL);
    
    arr->count = 0;
    arr->capacity = initial_capacity;
    arr->size = element_size;
    arr->destroy_element = destroy_element;
    
    //on s'assure que la capacité respecte la capacité minimale
    if(arr->capacity <= minimal_capacity) arr->capacity = minimal_capacity;

    arr->data = calloc(initial_capacity, element_size);
    if(!arr->data) return (perror("calloc"), free(arr), NULL);

    return arr;
}

void dyn_array_destroy(dyn_array_t *arr)
{
    destroy_elements(arr, arr->data, arr->count);
    free(arr->data);
    free(arr);
}

size_t dyn_array_count(dyn_array_t *arr)
{
    return arr->count;
}

size_t dyn_array_size(dyn_array_t *arr)
{
    return arr->size;
}

size_t dyn_array_capacity(dyn_array_t *arr)
{
    return arr->capacity;
}

bool dyn_array_is_empty(dyn_array_t *arr)
{
    return arr->count == 0;
}

void* dyn_array_get(dyn_array_t *arr, const size_t index)
{
    if(index >= arr->count)
    {
        fprintf(stderr, "dyn_array_get(%p, %zu): Index out of bounds\n", (void*)arr, index);
        return NULL;
    }

    return get_at(arr, index);
}

void dyn_array_set(dyn_array_t *arr, const size_t index, const void* value_ptr)
{
    if(index >= arr->count)
    {
        fprintf(stderr, "dyn_array_set(%p, %zu, %p): Index out of bounds\n", 
                (void*)arr, index, (void*)value_ptr);
        return;
    }

    void *dest = get_at(arr, index);

    //on détruit l'ancien élément
    destroy_elements(arr, dest, 1);

    //on copie la nouvelle valeur
    memcpy(dest, value_ptr, arr->size);
}

void dyn_array_push_back(dyn_array_t *arr, const void* value_ptr)
{
    auto_growth(arr);

    void *dest = get_at(arr, arr->count);
    memcpy(dest, value_ptr, arr->size);
    arr->count++;
}

void dyn_array_pop_back(dyn_array_t *arr)
{
    if(arr->count == 0) return;

    //on détruit l'élément
    destroy_elements(arr, get_at(arr, arr->count-1), 1);

    arr->count--;
    auto_shrink(arr);
}

void dyn_array_insert(dyn_array_t *arr, const size_t index, const void* value_ptr)
{
    if(index > arr->count)
    {
        fprintf(stderr, "dyn_array_insert(%p, %zu, %p): Index out of bounds\n", 
                (void*)arr, index, (void*)value_ptr);
        return;
    }

    auto_growth(arr);
 
    //on récupère un pointeur vers l'index
    void *index_ptr = get_at(arr, index);

    //décalage des éléments après l'index
    memmove(get_at(arr, index + 1), index_ptr, (arr->count - index) * arr->size);
    
    //copie de la valeur
    memcpy(index_ptr, value_ptr, arr->size);
    arr->count++;
}

void dyn_array_remove(dyn_array_t *arr, const size_t index)
{
    if(index >= arr->count)
    {
        fprintf(stderr, "dyn_array_remove(%p, %zu): Index out of bounds\n", (void*)arr, index);
        return;
    }

    //si on veut supprimer le dernier élément
    //on appelle dyn_array_pop_back
    if(index == arr->count -1)
    {
        dyn_array_pop_back(arr);
        return;
    }

    //on récupère l'élément à supprimer et ceux a décaler
    void *src =  get_at(arr, index + 1);
    void *dest = get_at(arr, index);

    //on détruit l'élément
    destroy_elements(arr, dest, 1);

    //décalage des éléments après l'index
    memmove(dest, src, (arr->count - index) * arr->size);

    arr->count--;
    auto_shrink(arr);
}

void* dyn_array_reserve(dyn_array_t *arr, const size_t count)
{
    if(count == 0) return NULL;//rien à faire

    size_t new_count = arr->count + count;

    if(new_count > arr->capacity)
    {
        resize(arr, new_count);
    }

    void *reserved = get_at(arr, arr->count);
    arr->count = new_count;

    return reserved;
}

void dyn_array_clear(dyn_array_t *arr)
{
    destroy_elements(arr, arr->data, arr->count);
    arr->count = 0;
}

void dyn_array_resize(dyn_array_t *arr, const size_t new_size)
{
    if(arr->capacity == new_size) return;
    resize(arr, new_size);
}

void dyn_array_shrink_to_fit(dyn_array_t *arr)
{
    if(arr->capacity == arr->count) return;

    size_t new_capacity = arr->count;

    if(new_capacity < minimal_capacity) 
        new_capacity = minimal_capacity;

    resize(arr, new_capacity);
}


void dyn_array_print(dyn_array_t *arr, void (*print)(void*))
{
    printf("(dyn_array_t):\n");
    printf("{\n");
    printf("    size: %zu\n", arr->size);
    printf("    count: %zu\n", arr->count);
    printf("    capacity: %zu\n", arr->capacity);
    printf("    data:\n");
    printf("    [\n");

    for(size_t i = 0; i < arr->count; i++)
    {
        void *value = dyn_array_get(arr, i);
        
        printf("\t");
        print(value);
        
        if(i < arr->count - 1) printf(",\n");
    }

    printf("\n    ]\n");
    printf("}\n");
}

size_t dyn_array_sizeof()
{
    return sizeof(dyn_array_t);
}

static inline void* get_at(dyn_array_t *arr, const size_t index)
{
    return (char*)arr->data + (index * arr->size);
}

static void destroy_elements(dyn_array_t *arr, void* start, size_t count)
{
    if(arr->destroy_element == NULL) return;

    for(size_t i = 0; i < count; i++)
    {
        void *element = (char*)start + (i * arr->size);
        arr->destroy_element(element);
    }
}

static void resize(dyn_array_t *arr, size_t new_capacity)
{
    assert(new_capacity > 0 && "new_capacity must be greater than 0");

    if(new_capacity == arr->capacity) return; //rien à faire
    if(new_capacity < arr->count) new_capacity = arr->count;

    //on redimensionne le tableau
    void *new_data = realloc(arr->data, new_capacity * arr->size);
    if(!new_data) { perror("realloc"); return; }

    arr->capacity = new_capacity;
    arr->data = new_data;
}

static void auto_growth(dyn_array_t *arr)
{
    if(auto_growth_condition(arr))
    {
        resize(arr, arr->capacity * auto_growth_factor);
    }
}

static void auto_shrink(dyn_array_t *arr)
{
    if(auto_shrink_condition(arr))
    {
        resize(arr, arr->capacity / auto_shrink_factor);
    }
}
