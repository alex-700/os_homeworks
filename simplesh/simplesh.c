#define _POSIX_C_SOURCE 201505L

#include <bufio.h>
#include <helpers.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#define CHUNK 2048

void start_line() {
    write_(STDOUT_FILENO, "$", 1);    
}

void ignore_handler() {
    write_(STDOUT_FILENO, "\n", 1);
}

char str[CHUNK];
char* args[CHUNK];
struct execargs_t* programs[CHUNK];

int main() { 
    struct sigaction ignore;
    ignore.sa_handler = ignore_handler;
    sigaction(SIGINT, &ignore, NULL);

    struct buf_t* buffer = buf_new(CHUNK);
    ssize_t read_bytes;
    size_t count_args, count_progs, start_str;
    while (1) {
        start_line();
        if ((read_bytes = buf_getline(STDOUT_FILENO, buffer, str)) <= 0) {
            if (read_bytes == 0 || errno != EINTR) {
                break;
            } else {
                continue;
            }
        }
        count_args = 0;
        count_progs = 0;
        start_str = 0;
        int was = 0;
        for (size_t i = 0; i < read_bytes; i++) {
            if (str[i] == '|' || str[i] == '\n') {
                if (was) {
                    str[i] = 0;
                    args[count_args++] = str + start_str;
                }
                programs[count_progs++] = create_execargs(args, count_args);
//                printf("%zu -> count = %zu; args = '%s', '%s'\n", count_progs, count_args, args[0], args[1]);
                was = 0;
                count_args = 0;
            } else if (str[i] == ' ') {
                if (was) {
                    str[i] = 0;
                    args[count_args++] = str + start_str;
                }
                was = 0;
            } else {
                if (was != 1) {
                    was = 1;
                    start_str = i;
                }
            }
        }
        if (runpiped(programs, count_progs) == -1) {
            write_(STDERR_FILENO, "something wrong\n", 16);
        }
    }

    buf_free(buffer);
    if (read_bytes == -1) {
        perror("input");
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
    
}
