#include <stdio.h>
#include "MessageDescriber.c"
#include "FileUtilities.c"
#include "Network.c"
#include "ResponseHandling.c"
#include "FileStore.c"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main() {
    int sockfd = createSocket();
    int connection;
    char *message;
    char *response1, *response2;

    char *content = makeResponse(200, "Hello, world!");

    while (1) {
        // Wait for a new connection
        connection = getSocket(sockfd);
        // Read HTTP request from client
        message = readMessage(connection);
        // Interpret the message
        struct MessageDescriber *describer = parseMessage(message);

        free(message);
        printf("Sending response...");
        response1 = content;
        sendMessage(connection, response1);
        close(connection);
    }
    return 0;
}
#pragma clang diagnostic pop