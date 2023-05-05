
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <arraylist.c>

// priority queue implementation


struct Element {
    uint16_t key;
    uint16_t data;
};

struct Node {
    struct Element left;
    struct Element right;
};

typedef struct PQHeap {
    ArrayList *array_list;
} PQHeap;


PQHeap *pq_heap_create() {
    PQHeap *heap = (PQHeap *)malloc(sizeof(PQHeap));
    heap->array_list = array_list_create(10);
    return heap;
}

void pq_heap_insert(PQHeap *heap, struct Element *e) {
    int i = heap->array_list->size;
    array heap->array_list
}


void pq_heap_exchange(int i, int j) {

}

int pq_heap_parent(int i) {
    return floor((i - 1) / 2);
}

int pq_heap_left(int i) {
    return 2 * i + 1;
}

int pq_heap_right(int i) {
    return 2 * i + 2;
}