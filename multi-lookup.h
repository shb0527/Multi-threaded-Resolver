#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "util.h"
#include "array.h"


#define ARRAY_SIZE 8  //Number of elements in the shared array used by the requester and resolver threads to communicate
#define MAX_INPUT_FILES 100 // Maximum number of hostname file arguments allowed
#define MAX_REQUESTER_THREADS 10 // Maximum number of concurrent requester threads allowed
#define MAX_RESOLVER_THREADS 10 // Maximum number of concurrent resolver threads allowed
#define MAX_NAME_LENGTH 1024 //Maximum size of a hostname including the null terminator
#define MAX_IP_LENGTH INET6_ADDRSTRLEN // INET6_ADDRSTRLEN is the maximum size IP address string util.c will return

void* requester(char *filename);
void* resolver();

#endif