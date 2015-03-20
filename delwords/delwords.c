#include<helpers.h>
#include<stdio.h>
#include<string.h>
const size_t CHUNK_SIZE = 10;

int main(int argc, char* argv[])
{
    if (argc == 0) {
        printf("enter argument");
        return 0;
    }
    char buf[CHUNK_SIZE];
    ssize_t read_bytes, wrote_bytes;
    ssize_t start = 0, check_len = 0;
    ssize_t bad_len = strlen(argv[1]);
    char* bad_word = argv[1];
    do {
        read_bytes = read_(STDIN_FILENO, buf + check_len, CHUNK_SIZE - check_len);
        if (read_bytes == -1) {
            perror("input fail");
            return -1;
        }
        ssize_t max_bytes = read_bytes + check_len;
        while (start < max_bytes) {
            while (check_len < bad_len && start + check_len < max_bytes && buf[start + check_len] == (*(bad_word + check_len))) {
                check_len++;                
            }
            if (check_len == bad_len) {
                for (ssize_t i = start + check_len; i < max_bytes; i++) {
                    buf[i - check_len] = buf[i];
                }
                max_bytes -= bad_len;
                check_len = 0;
            } else if (start + check_len == max_bytes) {
                break;
            } else {
                start++;
                check_len = 0;
            }
        };
        wrote_bytes = write_(STDOUT_FILENO, buf, start);
        for (ssize_t i = start; i < max_bytes; i++) {
            buf[i - start] = buf[i];        
        }
        start = 0;
        if (wrote_bytes < 0) {
            perror("output fail");
            return -1;
        }
    } while (read_bytes != 0);
    if (check_len != 0) {
        wrote_bytes = write_(STDOUT_FILENO, buf, check_len);
        if (wrote_bytes < 0) {
            perror("output fail");
            return -1;
        }   
    }
    return 0;
}
