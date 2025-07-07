#include <stdio.h>
#include <pthread.h>
#include "../include/ringbuffer.h"

#define MAX_BUFFER_SIZE 1024

static int buffer[MAX_BUFFER_SIZE];
static int buffer_size = 0;
static int head = 0;
static int tail = 0;
static int count = 0;

static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

void ringbuffer_init(int size) {
    pthread_mutex_lock(&buffer_mutex);
    
    if (size > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Requested size %d exceeds maximum buffer size %d\n", size, MAX_BUFFER_SIZE);
        pthread_mutex_unlock(&buffer_mutex);
        return;
    }
    
    buffer_size = size;
    head = 0;
    tail = 0;
    count = 0;
    
    pthread_mutex_unlock(&buffer_mutex);
}

int ringbuffer_add(int value) {
    int result = -1;
    
    pthread_mutex_lock(&buffer_mutex);
    
    if (count < buffer_size) {
        buffer[tail] = value;
        tail = (tail + 1) % buffer_size;
        count++;
        result = 0;
    }
    
    pthread_mutex_unlock(&buffer_mutex);
    return result;
}

int ringbuffer_remove(int* value) {
    int result = -1;
    
    pthread_mutex_lock(&buffer_mutex);
    
    if (count > 0) {
        *value = buffer[head];
        head = (head + 1) % buffer_size;
        count--;
        result = 0;
    }
    
    pthread_mutex_unlock(&buffer_mutex);
    return result;
}

int ringbuffer_is_full(void) {
    pthread_mutex_lock(&buffer_mutex);
    int is_full = (count == buffer_size);
    pthread_mutex_unlock(&buffer_mutex);
    return is_full;
}

int ringbuffer_is_empty(void) {
    pthread_mutex_lock(&buffer_mutex);
    int is_empty = (count == 0);
    pthread_mutex_unlock(&buffer_mutex);
    return is_empty;
}

int ringbuffer_size(void) {
    pthread_mutex_lock(&buffer_mutex);
    int size = count;
    pthread_mutex_unlock(&buffer_mutex);
    return size;
}

void ringbuffer_cleanup(void) {
    pthread_mutex_lock(&buffer_mutex);
    
    buffer_size = 0;
    head = 0;
    tail = 0;
    count = 0;
    
    pthread_mutex_unlock(&buffer_mutex);
}