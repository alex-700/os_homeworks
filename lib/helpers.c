#include "helpers.h"

ssize_t read_(int fd, void *buf, size_t count) 
{
    ssize_t c;
    ssize_t ans = 0;
    do {
        c = read(fd, buf + ans, count);
        if (c == -1) {
            return -1;
        }
        count -= c;
        ans += c;
    } while (c != 0 && count != 0);
    return ans;
}
ssize_t write_(int fd, const void *buf, size_t count)
{
    ssize_t c;
    ssize_t ans = 0;
    do {
        c = write(fd, buf + ans, count);
        if (c == -1) {
            return -1;
        }
        count -= c;
        ans += c;
    } while (count != 0 && c != 0);    
    return ans;  
}
ssize_t read_until(int fd, void * buf, size_t count, char delimiter)
{
    
    ssize_t c; 
    ssize_t ans = 0;
    do {
        c = read(fd, buf + ans, 1);
        if (c == -1) {
            return -1;
        }
        count-=c;
        ans+=c;
    } while (count != 0 && c != 0 && (*((char*) (buf + ans - 1)) != delimiter));
    return ans;
}
