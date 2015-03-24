#include <helpers.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STRING_SIZE 8192

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: filter <executable file> <arguments>\n");
        return EXIT_FAILURE;
    }

    char buf[MAX_STRING_SIZE];
    char* args[argc + 1];
    memcpy(args, argv + 1, (argc - 1) * sizeof(char*));
    args[argc] = NULL;
    ssize_t read_bytes;
    size_t start = 0, start_i;
    while ((read_bytes = read_until(STDIN_FILENO, buf + start, MAX_STRING_SIZE - start, '\n')) > 0) {
        start_i = 0;
        for (size_t i = start; i < start + read_bytes; i++) {
            if (buf[i] == '\n') {
                buf[i] = 0;
                args[argc - 1] = buf + start_i;
                int res = spawn(argv[1], args);
                if (res == 0) {
                    buf[i] = '\n';
                    if (write_(STDOUT_FILENO, buf + start_i, i - start_i + 1) == -1) {
                        perror("output fail");
                        return EXIT_FAILURE;
                    }                    
                }
                start_i = i + 1;
            }
        }
        start += read_bytes - start_i;
        memmove(buf, buf + start_i, start);
    }
    if (read_bytes < 0) {
        perror("input fail");
        return EXIT_FAILURE;
    } else if (start != 0) {
        buf[start] = 0;
        args[argc - 1] = buf;
        if (spawn(argv[1], args) == 0) {
            if (write_(STDOUT_FILENO, buf, start) == -1) {
                perror("output fail");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}
