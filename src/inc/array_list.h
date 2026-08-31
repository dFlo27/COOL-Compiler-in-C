#ifndef arraylist_h
#define arraylist_h

#include <stdlib.h>

struct ArrayList
{
    void *array;
    size_t length;
    size_t size;
    size_t element_size;
};

void initialize_list(struct ArrayList *list, size_t element_size);

void realloc_list(struct ArrayList *list, size_t new_size);

void append_list(struct ArrayList *list, void *element);

#endif