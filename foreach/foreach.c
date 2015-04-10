#include <helpers.h>
#include <bufio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STRING_SIZE 8192

char buf[MAX_STRING_SIZE];

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: filter <executable file> <arguments>\n");
        return EXIT_FAILURE;
    }
    
    char* args[argc + 1];
    memcpy(args, argv + 1, (argc - 1) * sizeof(char*));
    args[argc] = NULL;
    args[argc - 1] = buf;

    struct buf_t* buffer = buf_new(MAX_STRING_SIZE);
    ssize_t len;
    while ((len = buf_getline(STDIN_FILENO, buffer, buf)) > 0) {
        if (len % 2) { 
            buf[len - 1] = 0;
            int res = spawn(argv[1], args);
            if (res == 0) {
                buf[len - 1] = '\n';
                if (write_(STDOUT_FILENO, buf, len) == -1) {
                    perror("output fail");
                    return EXIT_FAILURE;
                }
            }
        }
    }
    if (len < 0) {
        perror("input fail");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
