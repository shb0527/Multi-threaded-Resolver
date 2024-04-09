#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int queue_init(queue* q, int size){
    
    int i;

    if(size>0) {
	q->capacity = size;
    }
    else {
	q->capacity = ARRAY_SIZE;
    }
    q->array = malloc(sizeof(queue_node) * (q->capacity));
    if(!(q->array)){	
        perror("Error on queue Malloc");
        return -1;
    }
    for(i=0; i < q->capacity; ++i){
	    q->array[i].data = NULL;
    }

    q->front = 0;
    q->rear = 0;

    return q->capacity;
}

int queue_empty(queue* q){
    
    if((q->front == q->rear) && (q->array[q->front].data == NULL)){
	    return 1;
    }
    else{
	    return 0;
    }
}

int queue_full(queue* q){
    if((q->front == q->rear) && (q->array[q->front].data != NULL)){
	    return 1;
    }
    else{
	    return 0;
    }
}

void* queue_pop(queue* q){
    
    void* pop_data;
	
    if(queue_empty(q)){
	    return NULL;
    }
	
    pop_data = q->array[q->front].data;
    q->array[q->front].data = NULL;
    q->front = ((q->front + 1) % q->capacity);

    return pop_data;
}

int queue_push(queue* q, void* new_data){
    
    if(queue_full(q)){
	    return -1;
    }

    q->array[q->rear].data = new_data;
    q->rear = ((q->rear+1) % q->capacity);

    return 0;
}

void queue_free(queue* q)
{
    while(!queue_empty(q)){
	    queue_pop(q);
    }

    free(q->array);
}
