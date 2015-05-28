#ifndef HELPERS_H
#define HELPERS_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEBUG 

struct execargs_t {
    char** args;
};

ssize_t read_(int fd, void *buf, size_t count);
ssize_t write_(int fd, const void *buf, size_t count);
ssize_t read_until(int fd, void * buf, size_t count, char delimiter);
int spawn(const char* file, char* const argv[]);

struct execargs_t* create_execargs(char** args, size_t count);
int exec(struct execargs_t* args);
int runpiped(struct execargs_t** programs, size_t n);

#endif // HELPERS_H
