#define _XOPEN_SOURCE 600

#include <bufio.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#define ACCEPT_QUEUE_SIZE 10
#define CHUNK 2048

int main(int argc, char** argv) {
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <port> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char* port = argv[1];
    char* file = argv[2];
    
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    
    int ret = getaddrinfo(NULL, port, &hints, &result);
    if (ret != 0) {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    int socket_fd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1) continue;
        if (bind(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(socket_fd);
    }
    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        return EXIT_FAILURE;
    }
    freeaddrinfo(result);
    
    ret = listen(socket_fd, ACCEPT_QUEUE_SIZE);
    if (ret != 0) {
        perror("listen");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    while (1) {
        int new_fd = accept(socket_fd, (struct sockaddr*) &cli_addr, &clilen);
        if (new_fd < 0) {
            perror("accept");
            continue;
        }
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            close(new_fd);
            close(socket_fd);
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            close(socket_fd);
            
            int file_fd = open(file, O_RDONLY);
            struct buf_t* buffer = buf_new(CHUNK);
            ssize_t read_bytes;
            
            while ((read_bytes = buf_fill(file_fd, buffer, CHUNK)) == CHUNK) {
                buf_flush(new_fd, buffer, read_bytes);   
            }
            if (read_bytes == -1) {
                perror("write");
            } else {
                buf_flush(new_fd, buffer, read_bytes);
            }

            buf_free(buffer);
            close(file_fd);
            close(new_fd);
            exit(EXIT_SUCCESS);
        } else {
            close(new_fd);
        }
    }
    return EXIT_SUCCESS;    
}
