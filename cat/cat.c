#include<helpers.h>
#include<stdio.h>

const size_t CHUNK_SIZE = 2048;

int main()
{
    char buf[CHUNK_SIZE];
    ssize_t read_bytes, wrote_bytes;
    
    do {
        read_bytes = read_(STDIN_FILENO, buf, CHUNK_SIZE);
        if (read_bytes == -1) {
            perror("input fail");
            return -1; 
        }
        wrote_bytes = write_(STDOUT_FILENO, buf, read_bytes);
        if (wrote_bytes == -1) {
            perror("output fail");
            return -1;
        }
    } while (read_bytes != 0);
    
    return 0;
}
