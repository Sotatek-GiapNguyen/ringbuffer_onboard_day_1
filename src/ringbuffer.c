#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/ringbuffer.h"

// Static buffer and related variables
static int* buffer = NULL;
static int buffer_size = 0;
static int head = 0;
static int tail = 0;
static int count = 0;

// Mutex for thread safety
static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

void ringbuffer_init(int size) {
    pthread_mutex_lock(&buffer_mutex);
    
    // Clean up existing buffer if any
    if (buffer != NULL) {
        free(buffer);
    }
    
    // Allocate new buffer
    buffer = (int*)malloc(size * sizeof(int));
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for ring buffer\n");
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
    
    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }
    buffer_size = 0;
    head = 0;
    tail = 0;
    count = 0;
    
    pthread_mutex_unlock(&buffer_mutex);
}