#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

void ringbuffer_init(int size);

int ringbuffer_add(int value);

int ringbuffer_remove(int* value);

int ringbuffer_is_full(void);

int ringbuffer_is_empty(void);

int ringbuffer_size(void);

void ringbuffer_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFFER_H */