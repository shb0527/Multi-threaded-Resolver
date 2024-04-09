#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include "multi-lookup.h"


pthread_mutex_t buffer_lock;
pthread_mutex_t input_lock; 
pthread_mutex_t thread_count; 
pthread_mutex_t serviced_lock; 
pthread_mutex_t results_lock; 
pthread_cond_t condc, condp;

queue q;
int err_c, err_p;
int num_input_files;
int num_threads = 0;
char serviced[50];
char results[50];


//producer
void* requester(char *filename){
    struct timeval s, e;
    gettimeofday(&s, NULL);
    char hostname[MAX_NAME_LENGTH];
    pthread_mutex_lock(&input_lock);
    
    FILE* inputfp = fopen(filename, "r");
    FILE* fservice = fopen(serviced, "a+");
    while(fscanf(inputfp, "%1024s", hostname)> 0){
        pthread_mutex_lock(&buffer_lock);
        
        queue_push(&q, strdup(hostname));
        pthread_mutex_lock(&serviced_lock); 
        fprintf(fservice, "%s, %s\n", filename, hostname);


        pthread_mutex_unlock(&serviced_lock);
        pthread_mutex_unlock(&buffer_lock);
    }

	pthread_mutex_lock(&thread_count);
	num_threads++; 
	pthread_mutex_unlock(&thread_count);


    gettimeofday(&e, NULL);
	double request_time;
	request_time = (e.tv_sec - s.tv_sec) * 1e6;
	request_time = (request_time + (e.tv_usec = s.tv_usec)) * 1e-6;

	printf("thread %u serviced %d files %f seconds\n", (unsigned int) pthread_self(), num_input_files, request_time);
	fclose(fservice);
    fclose(inputfp);
    pthread_cond_signal(&condc);
    pthread_mutex_unlock(&input_lock);
    pthread_exit(0); 

}

//consumer
void* resolver(){

    struct timeval s2, e2;
    gettimeofday(&s2, NULL);
    FILE *fresult = fopen(results, "a+");

    pthread_mutex_lock(&buffer_lock);
    
    if(queue_empty(&q)){
        pthread_cond_wait(&condc, &buffer_lock);
    }


    char* hostname = (char*)queue_pop(&q);
    
    char ip[MAX_IP_LENGTH];

    if(dnslookup(hostname, ip, sizeof(ip))==UTIL_FAILURE) {
        strncpy(ip,"NOT_RESOLVED",MAX_IP_LENGTH);
    }

    pthread_mutex_lock(&results_lock);
    fprintf(fresult,"%s, %s\n",hostname, ip);
    pthread_mutex_unlock(&results_lock);
    pthread_mutex_unlock(&buffer_lock);

    strncpy(ip,"",sizeof(ip));


    
    fclose(fresult);

    int host = 20;
    gettimeofday(&e2, NULL);
	double resolve_time;
	resolve_time = (e2.tv_sec - s2.tv_sec) * 1e6;
	resolve_time = (resolve_time + (e2.tv_usec = s2.tv_usec)) * 1e-6;
    printf("thread %lu resolved %d hosts in %f seconds\n", (unsigned long) pthread_self(), host, resolve_time);

    pthread_cond_signal(&condp);	
   

    pthread_exit(0); 
}


int main(int argc, char *argv[]){


    struct timeval start, end;
    gettimeofday(&start, NULL);
    int req_num;
	int res_num;
    char * req_char;
    char * res_char;

    req_num = strtol(argv[1], &req_char, 10); 
	res_num = strtol(argv[2], &res_char, 10);

    strcpy(serviced, argv[3]); 
	strcpy(results, argv[4]); 

    num_input_files = argc - 5;

    if (num_input_files > MAX_INPUT_FILES) {
		printf("Too many input files provided. Please provide less than 100.\n");
		exit(EXIT_FAILURE);
	}

	if (num_input_files < 1) {
		printf("Must provide at least one input file.\n");
		exit(EXIT_FAILURE);
	}

    if (req_num > MAX_REQUESTER_THREADS) {
		printf("Can't create more than 10 requester threads.\n");
		exit(EXIT_FAILURE);
	}

    if (req_num < 1) {
		printf("Must create at least one requester thread.\n");
		exit(EXIT_FAILURE);
    }

	if (res_num > MAX_RESOLVER_THREADS) {
		printf("Can't create more than 10 resolver threads.\n");
		exit(EXIT_FAILURE);
	}

	if (res_num < 1) {
		printf("Must create at least one resolver thread.\n");
		exit(EXIT_FAILURE);
	}


    
    char* input_files[num_input_files];
    

	for(int i=0; i< num_input_files;i++) {
		input_files[i] = argv[i + 5];
	}

    queue_init(&q, ARRAY_SIZE);
   
    if (req_num > num_input_files) { req_num = num_input_files;}
    if (res_num > num_input_files) { res_num = num_input_files;}

    pthread_t requester_threads[req_num]; 
	pthread_t resolver_threads[res_num];

	pthread_mutex_init(&buffer_lock, NULL);	
    pthread_mutex_init(&input_lock, NULL);
	pthread_mutex_init(&results_lock, NULL);
	pthread_mutex_init(&serviced_lock, NULL);
	pthread_cond_init(&condc, NULL); 
	pthread_cond_init(&condp, NULL); 

    
	for(int i=0; i < req_num; i++) {
		pthread_create(&requester_threads[i], NULL,  (void*)requester, input_files[i]);
	}

	for(int i=0; i< req_num; i++){ 
		pthread_join(requester_threads[i], NULL);
	}

	for(int i=0; i < res_num; i++){ 
		pthread_create(&resolver_threads[i], NULL, (void*)resolver, NULL);
	}

	for(int i=0; i < res_num; i++){ 
		pthread_join(resolver_threads[i], NULL);
	}
	

	pthread_mutex_destroy(&buffer_lock);	
    pthread_mutex_destroy(&input_lock);
	pthread_mutex_destroy(&results_lock);
	pthread_mutex_destroy(&serviced_lock);
	pthread_mutex_destroy(&thread_count);
	pthread_cond_destroy(&condc);		
	pthread_cond_destroy(&condp);		
	gettimeofday(&end, NULL);
	double total_time;
	total_time = (end.tv_sec - start.tv_sec) * 1e6;
	total_time = (total_time + (end.tv_usec = start.tv_usec)) * 1e-6;
	printf("Time taken: %f seconds \n", total_time);
}