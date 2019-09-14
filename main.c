#include <stdio.h>
#include "ClientMessage.c"
#include "FileUtilities.c"
#include "Network.c"
#include "ResponseHandling.c"
#include "Config.c"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(int argc, char *argv[]) {
    char *propertiesPath;

    if (argc == 2) {
        propertiesPath = argv[1];
    } else {
        printf("%s", "Invalid arguments. A config file must be specified as the first argument.");
        exit(-1);
    }

    int sockfd = createSocket();
    int connection;
    char *message;

    struct ServerProperties *properties = readProperties(propertiesPath);
    printf("Loaded a config file with the following properties:\nIdentifier:%s\nVersion:%s\nServing Root:%s\nServer Index:%s\n", properties->serverIdentifier, properties->serverVersion, properties->servingDirectory, properties->serverIndex);

    while (1) {

        // Wait for a new connection
        connection = getSocket(sockfd);
        // Read HTTP request from client
        message = readMessage(connection);
        // Interpret the message
        struct ClientMessage *describer = parseMessage(message);
        handleRequest(describer, connection, properties);

        switch(describer->requestType) {
            case HEAD:
                free(describer->head.file);
                break;
            case GET:
                free(describer->get.file);
                break;
            case PUT:
                free(describer->put.file);
                break;
        }
        free(message);
        free(describer);
        close(connection);
    }
    return 0;
}
#pragma clang diagnostic pop