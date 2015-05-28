
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
        return execvp(file, argv);
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
            for (size_t j = 0; j < i; j++) {
                free(execargs->args[j]);
            }
            free(execargs->args);
            free(execargs);
            return NULL;
        }
    }
    return execargs;
}
int exec(struct execargs_t* args) {
    return execvp(args->args[0], args->args);    
}

pid_t children[2048];
size_t count, flag;
void termination_handler() {
    flag =  0;
    for (size_t i = 0; i < count; i++) {
        if (children[i] != 0) {
            kill(children[i], SIGKILL);
        }
    }
}

int runpiped(struct execargs_t** programs, size_t n) {
    count = n;
    flag = 1;

    //signals
    struct sigaction new_action, old_action;
    new_action.sa_handler = termination_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, &old_action);

    //pipes
    int pipefd[n][2];
    for (size_t i = 0; i < n - 1; i++) {
        if (pipe(pipefd[i]) == -1) {
            for (size_t j = 0; j < i; j++) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }
            return EXIT_FAILURE;
        }
    }
    
    //start
    for (size_t i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            for (size_t j = 0; j < n - 1; j++) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }
            termination_handler();
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            for (size_t j = 0; j < n - 1; j++) {
                if (j != i - 1) close(pipefd[j][0]); 
                if (j != i) close(pipefd[j][1]);
            }
            if (i != 0) dup2(pipefd[i - 1][0], STDIN_FILENO);
            if (i != n - 1) dup2(pipefd[i][1], STDOUT_FILENO);
            exit(exec(programs[i]) == -1);
        } else {
            children[i] = pid;
        }
    }
    
    //closing pipes
    for (size_t i = 0; i < n - 1; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }
    
    //canceling sigaction

    int status;
    int ret = 0;
    for (int i = 0; i < n; i++) {
        if (children[i] != 0) {
            waitpid(children[i], &status, 0);
            if (WEXITSTATUS(status) != 0) {
                ret = -1;
            }
        } else {
            ret = -1;
        }
    }
    
    sigaction(SIGINT, &old_action, NULL);
    return flag * ret;
}
