#include <bufio.h>
#include <stdio.h>
#define CHUNK_SIZE 2048

int main()
{
    struct buf_t* buffer = buf_new(CHUNK_SIZE);
    ssize_t read_bytes;
    while ((read_bytes = buf_fill(STDIN_FILENO, buffer, CHUNK_SIZE)) > 0)
    {   
        if (buf_flush(STDOUT_FILENO, buffer, CHUNK_SIZE) < 0) {
            perror("output fail");
            return EXIT_FAILURE;
        }
    }
    if (buf_flush(STDOUT_FILENO, buffer, CHUNK_SIZE) < 0) {
        perror("output fail");
        return EXIT_FAILURE;
    }
    buf_free(buffer);
    if (read_bytes < 0) {
        perror("input fail");
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
