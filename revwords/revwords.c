#include<helpers.h>
#include<stdio.h>


const size_t MAX_WORD_SIZE = 4096;

int main() 
{
    char buf[MAX_WORD_SIZE];
    ssize_t read_bytes, wrote_bytes;
    do {
        read_bytes = read_until(STDIN_FILENO, buf, MAX_WORD_SIZE, ' ');
        if (read_bytes == -1) {
            perror("input fail");
            return -1;
        }
        for (ssize_t i = 0, j =  read_bytes - (buf[read_bytes - 1] == ' ' ? 2 : 1); i < j; i++, j--) {
            char temp = buf[i];
            buf[i] = buf[j];
            buf[j] = temp;            
        }
        wrote_bytes = write_(STDOUT_FILENO, buf, read_bytes);
        if (wrote_bytes == -1) {
            perror("output fail");
            return -1;
        }
    } while (read_bytes != 0);
}
