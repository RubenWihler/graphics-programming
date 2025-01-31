#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "dyn_array.h"

struct _dyn_array_t{
    size_t capacity;
    size_t count;
    size_t size;
    void* data;
};

static void resize(dyn_array_t *arr, const size_t new_capacity);
static void auto_growth(dyn_array_t *arr);
static void auto_shrink(dyn_array_t *arr);

dyn_array_t* dyn_array_create(const size_t initial_capacity, const size_t element_size)
{
    assert(initial_capacity > 0 && element_size > 0);

    dyn_array_t *arr = malloc(sizeof(*arr));
    if(!arr) return (perror("malloc"), NULL);
    memset(arr, 0, sizeof(*arr));
    
    arr->count = 0;
    arr->capacity = initial_capacity;
    arr->size = element_size;
    
    arr->data = calloc(initial_capacity, element_size);
    if(!arr->data) return (perror("malloc"), free(arr), NULL);

    return arr;
}

void dyn_array_destroy(dyn_array_t *arr)
{
    free(arr->data);
    free(arr);
}

void dyn_array_destroy_complex(dyn_array_t *arr, void (*destroy_element)(void*))
{
    if(destroy_element == NULL) destroy_element = free;

    for(size_t i = 0; i < arr->count; i++)
        destroy_element((char*)arr->data + (i * arr->size));

    dyn_array_destroy(arr);
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

bool dyn_array_empty(dyn_array_t *arr)
{
    return arr->count == 0;
}

void* dyn_array_get(dyn_array_t *arr, const size_t index)
{
    if(index >= arr->count)
    {
        fprintf(stderr, "Index out of bounds\n");
        return NULL;
    }

    return (char*)arr->data + (index * arr->size);
}

void dyn_array_set(dyn_array_t *arr, const size_t index, const void* value_ptr)
{
    if(index >= arr->count)
    {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    void *dest = (char*)arr->data + (index * arr->size);
    memcpy(dest, value_ptr, arr->size);
}

void dyn_array_push_back(dyn_array_t *arr, const void* value_ptr)
{
    auto_growth(arr);

    void *dest = (char*)arr->data + (arr->count * arr->size);
    memcpy(dest, value_ptr, arr->size);
    arr->count++;
}

void dyn_array_pop_back(dyn_array_t *arr)
{
    if(arr->count > 0) arr->count--;
    auto_shrink(arr);
}

void dyn_array_insert(dyn_array_t *arr, const size_t index, const void* value_ptr)
{
    if(index > arr->count)
    {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    auto_growth(arr);

    void *dest = (char*)arr->data + (index * arr->size);
    void *src = (char*)arr->data + ((index + 1) * arr->size);
    size_t size = (arr->count - index) * arr->size;
    memmove(src, dest, size);
    memcpy(dest, value_ptr, arr->size);
    arr->count++;
}

void dyn_array_remove(dyn_array_t *arr, const size_t index)
{
    if(index >= arr->count)
    {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    void *dest = (char*)arr->data + (index * arr->size);
    void *src = (char*)arr->data + ((index + 1) * arr->size);
    size_t size = (arr->count - index) * arr->size;
    memmove(dest, src, size);
    arr->count--;

    auto_shrink(arr);
}

void* dyn_array_reserve(dyn_array_t *arr, const size_t count)
{
    if(count == 0) return NULL;

    size_t new_count = arr->count + count;
    if(new_count > arr->capacity)
        resize(arr, new_count);

    void *dest = (char*)arr->data + (arr->count * arr->size);
    arr->count = new_count;

    return dest;
}

void dyn_array_clear(dyn_array_t *arr)
{
    arr->count = 0;
}

void dyn_array_resize(dyn_array_t *arr, const size_t new_size)
{
    if(new_size > arr->capacity)
        resize(arr, new_size);
    else
    {
        arr->count = new_size;
        dyn_array_shrink_to_fit(arr);
    }
}

void dyn_array_shrink_to_fit(dyn_array_t *arr)
{
    if(arr->capacity > arr->count)
    {
        resize(arr, arr->count);
    }
}


void dyn_array_print(dyn_array_t *arr, void (*print)(void*))
{
    for(size_t i = 0; i < arr->count; i++)
    {
        void *value = dyn_array_get(arr, i);
        print(value);
    }
}

size_t dyn_array_sizeof()
{
    return sizeof(dyn_array_t);
}


static void resize(dyn_array_t *arr, size_t new_capacity)
{
    //si la nouvelle capacité est inférieure au nombre d'éléments, on ajuste la capacité au nombre d'éléments
    if(new_capacity < arr->count) new_capacity = arr->count;

    size_t old_count = arr->count;

    void *new_data = realloc(arr->data, new_capacity * arr->size);
    if(!new_data) { perror("realloc"); return; }

    arr->capacity = new_capacity;
    arr->data = new_data;
    
    //si la nouvelle capacité est plus grande que l'ancienne, on initialise les nouvelles cases à 0
    if(arr->count > old_count)
        memset((char*)arr->data + (old_count * arr->size), 
               0, (arr->count - old_count) * arr->size);
}

static void auto_growth(dyn_array_t *arr)
{
    if(arr->count == arr->capacity)
        resize(arr, arr->capacity * 2);
}

static void auto_shrink(dyn_array_t *arr)
{
    if(arr->count < arr->capacity / 4)
        resize(arr, arr->capacity / 2);
}
