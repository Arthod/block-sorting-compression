#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct ArrayList {
    int *data;
    int size;
    int capacity;
} ArrayList;

ArrayList *array_list_create(int initial_size) {
    ArrayList *list = (ArrayList *)malloc(sizeof(ArrayList));
    list->data = (int *)malloc(initial_size * sizeof(int));
    list->size = 0;
    list->capacity = initial_size;
    return list;
}

void array_list_ensure_capacity(ArrayList *list, int min_capacity) {
    if (list->capacity < min_capacity) {
        int new_capacity = list->capacity * 2;
        if (new_capacity < min_capacity) {
            new_capacity = min_capacity;
        }
        printf("Increasing list array size from %d to %d\n", list->capacity, new_capacity);
        list->data = (int *)realloc(list->data, new_capacity * sizeof(int));
        list->capacity = new_capacity;
    }
}

int array_list_get(ArrayList *list, int index) {
    if (index < 0 || index >= list->size) {
        printf("Index out of bounds\n");
        return 1;
    }
    return list->data[index];
}

int array_list_size(ArrayList *list) {
    return list->size;
}

void array_list_set(ArrayList *list, int index, int val) {
    if (index < 0 || index >= list->size) {
        printf("Index out of bounds\n");
        return 1;
    }
    list->data[index] = val;
}

void array_list_remove(ArrayList *list, int index) {
    if (index < 0 || index >= list->size) {
        printf("Index out of bounds\n");
        return 1;
    }
    for (int i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
}

void array_list_add(ArrayList *list, int val) {
    array_list_ensure_capacity(list, list->size + 1);
    list->data[list->size++] = val;
}

void array_list_free(ArrayList *list) {
    free(list->data);
    free(list);
}