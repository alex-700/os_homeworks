#include<helpers.h>
#include<stdio.h>


const size_t MAX_WORD_SIZE = 4097;

int main() 
{
    char buf[MAX_WORD_SIZE];
    ssize_t read_bytes, wrote_bytes, last_k, start = 0;
    do {
        read_bytes = read_until(STDIN_FILENO, buf + start, MAX_WORD_SIZE, ' ');
        if (read_bytes == -1) {
            perror("input fail");
            return -1;
        }
        last_k = 0;
        for (ssize_t k = start; k < start + read_bytes; k++) {
            if (buf[k] == ' ') {
                for (ssize_t i = last_k, j = k - 1; i < j; i++, j--) {
                    char temp = buf[i];
                    buf[i] = buf[j];
                    buf[j] = temp;            
                }
                wrote_bytes = write_(STDOUT_FILENO, buf + last_k, k - last_k + 1);
                if (wrote_bytes == -1) {
                    perror("output fail");
                    return -1;
                }
                last_k = k + 1;
            }
        }
        if (last_k != 0) {
            for (ssize_t i = last_k; i < start + read_bytes; i++) {
                buf[i - last_k] = buf[i];
            }
        }
        start += read_bytes - last_k;
    } while (read_bytes != 0);
    if (start != 0) {
        for (ssize_t i = 0, j = start - 1; i < j; i++, j--) {
            char temp = buf[i];
            buf[i] = buf[j];
            buf[j] = temp;            
        }
        wrote_bytes = write_(STDOUT_FILENO, buf, start);
        if (wrote_bytes == -1) {
            perror("output fail");
            return -1;
        }
    }
}
