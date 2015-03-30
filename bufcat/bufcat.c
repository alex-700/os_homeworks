#include <bufio.h>
#include <stdio.h>
#define CHUNK_SIZE 2048

int main()
{
    struct buf_t* buffer = buf_new(CHUNK_SIZE);
    ssize_t read_bytes;
    while ((read_bytes = buf_fill(STDIN_FILENO, buffer, CHUNK_SIZE)) > 0)
    {   
        buf_flush(STDOUT_FILENO, buffer, CHUNK_SIZE);
    }
    buf_flush(STDOUT_FILENO, buffer, CHUNK_SIZE);
    return read_bytes;
}
