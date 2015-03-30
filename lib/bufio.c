#include "bufio.h"

struct buf_t* buf_new(size_t capacity)
{
    struct buf_t* buffer = (struct buf_t*) malloc(sizeof(struct buf_t));
    if (!buffer) 
        return NULL;
    buffer->capacity = capacity;
    buffer->size = 0;
    buffer->buf = (char*) malloc(capacity);
    if (!buffer->buf) {
        free(buffer);
        return NULL;
    }
    return buffer;     
}

void buf_free(struct buf_t * buffer)
{
    #ifdef DEBUG
    if (!buffer) 
        abort();
    #endif
    free(buffer);
}

size_t buf_capacity(struct buf_t* buffer)
{
    #ifdef DEBUG
    if (!buffer)
        abort();
    #endif
    return buffer->capacity;
}

size_t buf_size(struct buf_t* buffer)
{
    #ifdef DEBUG   
    if (!buffer)
        abort();
    #endif
    return buffer->size;
}

ssize_t buf_fill(fd_t fd, struct buf_t* buffer, size_t required)
{
    #ifdef DEBUG
    if (!buffer || buffer->capacity < required)
        abort(); 
    #endif
    ssize_t read_bytes = -1;
    while (buffer->size < required || read_bytes != 0) {
        read_bytes = read(fd, buffer->buf + buffer->size, buffer->capacity - buffer->size);
        if (read_bytes == -1) 
            return -1;
        buffer->size += read_bytes;
    }
    return buffer->size;
}

ssize_t buf_flush(fd_t fd, struct buf_t* buffer, size_t required)
{
    #ifdef DEBUG
    if (!buffer)
        abort();
    #endif
    ssize_t wrote_bytes;
    size_t current_position = 0;
    while (current_position < required && current_position < buffer->size) {
        wrote_bytes = write(fd, buffer->buf + current_position, buffer->size - current_position);
        if (wrote_bytes == -1)
            return -1;
        current_position += wrote_bytes;
    }
    if (current_position < buffer->size) 
        memmove(buffer->buf, buffer->buf + current_position, buffer->size - current_position);
    buffer->size = buffer->size - current_position;
    return current_position;
}
