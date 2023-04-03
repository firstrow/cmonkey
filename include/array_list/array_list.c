#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_list.h"

static void fatal(char *msg)
{
    printf("%s\r\n", msg);
    abort();
}

array_list *array_list_create(size_t item_size, size_t initial_capacity)
{
    array_list *list = malloc(sizeof(array_list));

    if (!list) {
        fatal("Could not allocate memory for array_list");
    }

    list->item_size = item_size;
    list->capacity = initial_capacity;
    list->len = 0;
    list->items = malloc(item_size * initial_capacity);

    if (!list->items) {
        fatal("Could not allocate memory for array_list\n");
    }

    return list;
}

void array_list_reset(array_list *list)
{
    free(list->items);
    list->len = 0;
    list->items = malloc(list->item_size * list->capacity);
}

size_t array_list_append(array_list *list, void *item)
{
    if (list->len == list->capacity) {
        list->capacity = list->capacity > 0 ? list->capacity * 2 : 1;
        void *items = realloc(list->items, list->item_size * list->capacity);

        if (!items)
            fatal("Could not allocate memory for array_list");

        list->items = items;
    }

    size_t index = list->len++;

    memcpy((uint8_t *)list->items + index * list->item_size, item,
           list->item_size);

    return index;
}

void *array_list_get(array_list *list, size_t index)
{
    if (index > list->len) {
        fatal("Index out of bounds");
    }

    return (uint8_t *)list->items + index * list->item_size;
}

void array_list_remove(array_list *list, size_t index)
{
    if (list->len == 0) {
        fatal("List is empty");
    }
    if (index > list->len) {
        fatal("Index out of bounds");
    }

    if (list->len == 1) {
        list->len = 0;
        return;
    }

    --list->len;

    uint8_t *item_ptr = (uint8_t *)list->items + index * list->item_size;
    uint8_t *end_ptr = (uint8_t *)list->items + list->len * list->item_size;
    memcpy(item_ptr, end_ptr, list->item_size);

    return;
}

// index should not exceed len
void array_list_set(array_list *list, void *item, size_t index)
{
    if (list->len == list->capacity) {
        list->capacity = list->capacity > 0 ? list->capacity * 2 : 1;
        void *items = realloc(list->items, list->item_size * list->capacity);

        if (!items) {
            fatal("Could not allocate memory for array_list\n");
        }

        list->items = items;
    }

    if (index >= list->len) {
        list->len = index;
    }

    memcpy((uint8_t *)list->items + index * list->item_size, item, list->item_size);
}
