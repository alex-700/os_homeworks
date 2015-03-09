#include "helpers.h"
#include "stdio.h"
#include "errno.h"
const size_t CHUNK_SIZE = 1024;

ssize_t read_(int fd, void *buf, size_t count) 
{
    ssize_t  cc = 0;
    int c;
    while ((c = read(fd, buf + cc, CHUNK_SIZE)) == CHUNK_SIZE) {
        cc += c;        
    }
    cc += c;
    return cc;
}
ssize_t write_(int fd, const void *buf, size_t count)
{
    return write(fd, buf, count);    
}
