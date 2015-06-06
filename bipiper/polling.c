#define _XOPEN_SOURCE 600

#include <bufio.h>
#include <helpers.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

#define ACCEPT_QUEUE_SIZE 10
#define TIMEOUT 1000 * 1000
#define MAX 2048
#define MAX_COUNT 256 

int socket_fd[2];
struct pollfd poll_set[MAX_COUNT];
struct buf_t* buffs[MAX_COUNT - 2];
int state[MAX_COUNT - 2];

void out(const char* msg) {
    write_(STDOUT_FILENO, msg, strlen(msg));
}

int main(int argc, char** argv) {
    if (argc <= 2) {
        fprintf(stderr, "Usage %s <port1> <port2>\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    for (int i = 0; i < 2; i++) {
        if (getaddrinfo(NULL, argv[i + 1], &hints, &result) != 0) {
            perror("getaddrinfo");
            if (i == 1) close(socket_fd[0]);
            return EXIT_FAILURE;
        }
        for (rp = result; rp != NULL; rp = rp->ai_next) {
            socket_fd[i] = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (socket_fd[i] == -1) continue;
            if (bind(socket_fd[i], rp->ai_addr, rp->ai_addrlen) == 0) break;
            close(socket_fd[i]);
        }  
        if (rp == NULL) {
            fprintf(stderr, "Could not bind to %s\n", argv[i + 1]);
            if (i == 1) close(socket_fd[0]);
            return EXIT_FAILURE;
        }
        freeaddrinfo(result);
        if (listen(socket_fd[i], ACCEPT_QUEUE_SIZE) != 0) {
            perror("listen");
            if (i == 1) close(socket_fd[0]);
            close(socket_fd[i]);
            return EXIT_FAILURE;
        }
        poll_set[i].fd = socket_fd[i];
    }
    nfds_t count = 2;
    int ret, first_fd;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    poll_set[0].events = POLLIN;
    while (1) {
        ret = poll(poll_set, count, TIMEOUT);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("poll");
                break;
            }
        } else if (ret == 0) {
            out("timeout expired\n");
        } else {
            if (count < MAX_COUNT) {
                if (poll_set[0].revents & POLLIN) {
                    //first acceptor 
                    first_fd = accept(poll_set[0].fd, (struct sockaddr*) &cli_addr, &clilen);
                    if (first_fd == -1) {
                        perror("accept");
                    } else {
                        poll_set[0].events = 0;
                        poll_set[1].events = POLLIN;
                    }
                }
                if (poll_set[1].revents & POLLIN) {
                    //second acceptor
                    poll_set[count + 1].fd = accept(poll_set[1].fd, (struct sockaddr*) &cli_addr, &clilen);
                    if (poll_set[count + 1].fd == -1) {
                        perror("accept");
                    } else {
                        poll_set[1].events = 0;
                        poll_set[0].events = poll_set[count].events = poll_set[count + 1].events = POLLIN;
                        poll_set[count].fd = first_fd;
                        buffs[count - 2] = buf_new(MAX);
                        buffs[count - 1] = buf_new(MAX);
                        state[count - 2] = 0;
                        state[count - 1] = 0;
                        count += 2;
                    }
                }
            } else {
                poll_set[0].events = 0;
            }
            for (int i = 2; i < count; i++) {
                if (poll_set[i].revents & POLLIN) {
                    //read
                    int prev_size = buffs[i - 2]->size;
                    if (buf_fill(poll_set[i].fd, buffs[i - 2], 1) <= prev_size) {
                        shutdown(poll_set[i].fd, SHUT_RD);
                        state[i - 2] = 1;
                        poll_set[i].events &= ~POLLIN;
                        if (prev_size == 0) {
                            shutdown(poll_set[i ^ 1].fd, SHUT_WR);
                            state[i - 2] = 2;
                        }
                    } else {
                        poll_set[i ^ 1].events |= POLLOUT;
                        if (buffs[i - 2]->size == buffs[i - 2]->capacity) {
                            poll_set[i].events &= ~POLLIN;                                                
                        }
                    }
                } else if (poll_set[i].revents & POLLOUT) {
                    //write
                    buf_flush(poll_set[i].fd, buffs[(i - 2) ^ 1], 1);
                    if (state[(i - 2) ^ 1] == 1 && buffs[(i - 2) ^ 1]->size == 0) {
                        shutdown(poll_set[i].fd, SHUT_WR);
                        state[(i - 2) ^ 1] = 2;
                    }
                    if (state[(i - 2) ^ 1] == 0) poll_set[i ^ 1].events |= POLLIN;
                    if (buffs[(i - 2) ^ 1]->size == 0) {
                        poll_set[i].events &= ~POLLOUT;
                    }
                }
                if (state[i - 2] == 2 && state[(i - 2) ^ 1] == 2) {
                    //closing
                    close(poll_set[i].fd);
                    close(poll_set[i ^ 1].fd);
                    poll_set[i] = poll_set[count - 2 + (i & 1)];
                    poll_set[i ^ 1] = poll_set[count - 2 + ((i ^ 1) & 1)];
                    buf_free(buffs[i - 2]);
                    buf_free(buffs[(i - 2) ^ 1]);
                    buffs[i - 2] = buffs[count - 4 + (i & 1)];
                    buffs[(i - 2) ^ 1] = buffs[count - 4 + ((i ^ 1) & 1)];
                    state[i - 2] = state[count - 4 + (i & 1)];
                    state[(i - 2) ^ 1] = state[count - 4 + ((i ^ 1) & 1)];
                    count -= 2;
                    if (count < MAX_COUNT) poll_set[0].events = POLLIN; 
                    i = (i & (~1)) - 1;
                    continue;
                }
            }
        }
    }
}
