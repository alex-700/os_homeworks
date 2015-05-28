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
    ssize_t last_ans = 0;
    do {
        c = read(fd, buf + ans, count);
        if (c == -1) {
            return -1;
        }
        count -= c;
        ans += c;
        for (;last_ans < ans; last_ans++) {
            if (((char *)buf)[last_ans] == delimiter) {
                break;
            }
        }        
    } while (c != 0 && count != 0 && last_ans == ans);
    return ans;
}
int spawn(const char* file, char* const argv[])
{
    pid_t cpid = fork();
    if (cpid == -1) {
        return -1;
    }
    if (cpid == 0) {
        execvp(file, argv); 
    } else {
        int status;
        pid_t w = waitpid(cpid, &status, 0);
        if (w == -1) {
            return -1;
        }
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

struct execargs_t* create_execargs(char** args, size_t count) {
    struct execargs_t* execargs = (struct execargs_t*) malloc(sizeof(struct execargs_t)); 
    if (execargs == NULL) {
        return NULL;
    }
    execargs->args = malloc((count + 1) * sizeof(char*));
    execargs->args[count] = NULL; 
    for (size_t i = 0; i < count; i++) {
        execargs->args[i] = strdup(args[i]);
        if (execargs->args[i] == NULL) {
            return NULL;
        }
    }
    return execargs;
}
int exec(struct execargs_t* args) {
    execvp(args->args[0], args->args);    
}

int runpiped(struct execargs_t** programs, size_t n) {
    
    #ifdef DEBUG
    printf("debug output start\n");

    printf("get %zu programs\n", n);
    for (size_t i = 0; i < n; i++) {
        printf("%zu program\n", i + 1);
        size_t argc = 0;
        while (programs[i]->args[argc] != NULL) {
            printf("%zu argument = '%s'\n", argc + 1, programs[i]->args[argc]);
            argc++;
        }
    }


    printf("debug output finish\n\n");
    #endif
    int pipefd[n][2];
    for (size_t i = 0; i < n - 1; i++) {
        if (pipe(pipefd[i]) == -1) {
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            for (size_t j = 0; j < n - 1; j++) {
                if (j != i - 1) close(pipefd[j][0]); 
                if (j != i) close(pipefd[j][1]);
            }
            if (i != 0) dup2(pipefd[i - 1][0], STDIN_FILENO);
            if (i != n - 1) dup2(pipefd[i][1], STDOUT_FILENO);
            exec(programs[i]);       
        }
    }
    for (size_t i = 0; i < n - 1; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }
    int status;
    waitpid(0, &status, 0);
    return WEXITSTATUS(status) == 0 ? 0 : -1;
}
