#include <stdio.h>
#include <string.h>
#include "arrayList.h"

#define INITIAL_LIST_LENGTH 8

void initialize_list(struct ArrayList *list, size_t element_size)
{
    if (element_size > 8)
    {
        printf("Array List larger than void pointer size; pass a pointer instead\n");
        /* Change Number */ exit(1);
    }
    list->array = malloc(INITIAL_LIST_LENGTH * element_size);
    if (list->array == NULL)
    {
        printf("Malloc failed to allocate space\n");
        /* Change Number */ exit(1);
    }
    list->length = 0;
    list->size = INITIAL_LIST_LENGTH;
    list->element_size = element_size;
}

void realloc_list(struct ArrayList *list, size_t new_size)
{
    void *new_loc;
    size_t list_byte_length = new_size * list->element_size;
    list->size = new_size;

    new_loc = realloc(list->array, list_byte_length);
    if (new_loc == NULL)
    {
        new_loc = malloc(list_byte_length);
        if (new_loc == NULL)
        {
            printf("Malloc failed to allocate space\n");
            /* Change Number */ exit(1);
        }
        memcpy(new_loc, list->array, list_byte_length);
        free(list->array);
    }
    list->array = new_loc;
}

void append_list(struct ArrayList *list, void *element)
{
    if (list->length == list->size)
        realloc_list(list, list->size * 2);
    memcpy(list->array + list->length * list->element_size, &element, list->element_size);
    list->length++;
}

void *remove_last(struct ArrayList *list)
{
    void *last;
    if (list->length == 0)
    {
        printf("Warning: List is already empty\n");
        return NULL;
    }
    memcpy(last, list->array + (list->length - 1) * list->element_size, list->element_size);
    list->length -= 1;
    return last;
}