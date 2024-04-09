#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE 8
#define MAX_NAME_LENGTH 1024

#include <stdio.h>


typedef struct queue_node_struct{
    void* data;
} queue_node;

typedef struct queue_struct{
    queue_node* array;
    int front;
    int rear;
    int capacity;
} queue;


int queue_init(queue* q, int size);

int queue_empty(queue* q);

int queue_full(queue* q);

int queue_push(queue* q, void* data);

void* queue_pop(queue* q);

void queue_free(queue* q);

#endif