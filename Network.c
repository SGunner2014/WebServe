//
// Created by samgunner on 07/09/2019.
//

#ifndef _STDIO_H
#include <stdio.h>
#endif

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8081
#define _NETWORK_C 1

// Creates and registers a socket
int createSocket() {
    int opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int valread;
    char buffer[1024] = {0};
    char *hello = "Hello";

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) <0) {
        perror("bind failed.");
    }

    listen(sockfd, 3);

    return sockfd;
}

// Listens for a new connection and returns the id of one, when it receives one
int getSocket(const int sock_fd) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int new_socket = accept(sock_fd,
                            (struct sockaddr*) &address,
                            (socklen_t*) &addrlen);

    return new_socket;
}

// Blocks and waits for a message from the client
char* readMessage(const int sock_fd) {
    int messageLength = 1024;
    char *buffer = malloc(sizeof(char) * messageLength);

    read(sock_fd, buffer, messageLength);
    return buffer;
}

// Sends a message back to the client
void sendMessage(int connection, char *message) {
    printf("%s%s", "Sending the following content:\n", message);
    send(connection, message, strlen(message), 0);
}