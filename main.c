#include <stdio.h>
#include "MessageDescriber.c"
#include "FileUtilities.c"
#include "Network.c"
#include "ResponseHandling.c"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main() {
    int sockfd = createSocket();
    int connection;
    char *message;

    while (1) {
        // Wait for a new connection
        connection = getSocket(sockfd);
        // Read HTTP request from client
        message = readMessage(connection);
        // Interpret the message
        struct MessageDescriber describer = parseMessage(message);
        handleRequest(describer, connection);
        free(message);
        printf("Sending response...");
        close(connection);
    }
    return 0;
}
#pragma clang diagnostic pop