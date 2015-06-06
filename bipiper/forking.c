#define _XOPEN_SOURCE 600

#include <bufio.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#define ACCEPT_QUEUE_SIZE 10
#define MAX 2048

int socket_fd[2];

int main(int argc, char** argv) {
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <port1> <port2>\n", argv[0]);
        return EXIT_FAILURE;
    }
    signal(SIGCHLD, SIG_IGN);
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    int ret;
    for (int i = 0; i < 2; i++) {
        ret = getaddrinfo(NULL, argv[i + 1], &hints, &result);
        if (ret != 0) {
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
        ret = listen(socket_fd[i], ACCEPT_QUEUE_SIZE);
        if (ret != 0) {
            perror("listen");
            if (i == 1) close(socket_fd[0]);
            close(socket_fd[i]);
            return EXIT_FAILURE;
        }
    }
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    while (1) {
        int first_fd, second_fd;
        while ((first_fd = accept(socket_fd[0], (struct sockaddr*) &cli_addr, &clilen)) == -1) perror("accept");
        while ((second_fd = accept(socket_fd[1], (struct sockaddr*) &cli_addr, &clilen)) == -1) perror("accept");
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            close(first_fd);
            close(second_fd);
            close(socket_fd[0]);
            close(socket_fd[1]);
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            close(socket_fd[0]);
            close(socket_fd[1]);
            pid = fork();
            if (pid == -1) {
                perror("fork2");
                close(first_fd);
                close(second_fd);
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                //from first to second
                struct buf_t* buffer = buf_new(MAX);
                ssize_t read_bytes;
                while ((read_bytes = buf_fill(first_fd, buffer, 1)) > 0) {
                    if (buf_flush(second_fd, buffer, 1) == -1) break;
                }
                buf_free(buffer);
                shutdown(first_fd, SHUT_RD);
                shutdown(second_fd, SHUT_WR);
            } else {
                //from second to first
                struct buf_t* buffer = buf_new(MAX);
                ssize_t read_bytes;
                while ((read_bytes = buf_fill(second_fd, buffer, 1)) > 0) {
                    if (buf_flush(first_fd, buffer, 1) == -1) break;
                }
                buf_free(buffer);
                shutdown(first_fd, SHUT_WR);
                shutdown(second_fd, SHUT_RD);
            }
            close(first_fd);
            close(second_fd);
            exit(EXIT_SUCCESS);
        } else {
            close(first_fd);
            close(second_fd);
        }
    }
}

