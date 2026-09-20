#ifndef KUDZU_H
#define KUDZU_H

#include <stddef.h>

typedef struct {
    void    *items;
    size_t  elem_size;
    size_t  len;
    size_t  cap;
} darray;

darray darray_init(size_t size, size_t initial_capacity);
void *darray_append(darray *arr, const void *elem);
void *darray_at(darray *arr, size_t index);
void darray_free(darray *arr);

#endif

#ifdef KUDZU_IMPLEMENTATION

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

darray darray_init(size_t elem_size, size_t initial_cap) {
    darray arr = { NULL, elem_size, 0, 0 };

    if (initial_cap && elem_size <= SIZE_MAX / initial_cap) {
        arr.items = malloc(initial_cap * elem_size);
        if (arr.items)
            arr.cap = initial_cap;
    }
    return arr;
}

void *darray_append(darray *arr, const void *elem) {
    void *slot;

    if (arr->len == arr->cap) {
        size_t new_cap = arr->cap ? arr->cap * 2 : 16;
        void *p;

        if (new_cap && arr->elem_size > SIZE_MAX / new_cap)
            return NULL;
        p = realloc(arr->items, new_cap * arr->elem_size);
        if (!p)
            return NULL;
        arr->items = p;
        arr->cap = new_cap;
    }

    slot = (char *)arr->items + arr->len * arr->elem_size;
    memcpy(slot, elem, arr->elem_size);
    arr->len++;
    return slot;
}

void *darray_at(darray *arr, size_t index) {
    if (index >= arr->len)
        return NULL;
    return (char *)arr->items + index * arr->elem_size;
}

void darray_free(darray *arr) {
    free(arr->items);
    arr->items = NULL;
    arr->len = arr->cap = 0;
}

#endif
