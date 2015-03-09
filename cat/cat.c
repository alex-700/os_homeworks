#include"../lib/helpers.h"
#include<stdio.h>
const size_t CHUNK_SIZE = 2048;

int main()
{
    char buf[CHUNK_SIZE];
    ssize_t c;
    do {
        c = read_(STDIN_FILENO, buf, CHUNK_SIZE);
        if (c == -1) {
            printf("Fail");
            break;
        }
        write_(STDOUT_FILENO, buf, c);
    } while (c != 0);
    return 0;
}
