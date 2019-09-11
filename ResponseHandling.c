//
// Created by samgunner on 07/09/2019.
//

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _MESSAGEDESCRIBER_C
#include "MessageDescriber.c"
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _FILEUTILITIES_C
#include "FileUtilities.c"
#endif

#ifndef _NETWORK_C
#include "Network.c"
#endif

// Concatenates two strings together
// null char indicates if the string needs to be terminated with a null char
char *concat(const char *c1, const char *c2, const int nullChar) {
    unsigned long length;
    if (nullChar)
        length = strlen(c1) + strlen(c2) + 1;
    else
        length = strlen(c1) + strlen(c2);
    char *final = malloc(length);
    strcpy(final, c1);
    strcat(final, c2);
    return final;
}

// Makes a valid HTTP response from the supplied content and code
char *makeResponse(int code, char *content, int isHead) {
    int contentLengthInt;
    char *contentLength;
    contentLengthInt = (int) strlen(content); // Length of content, in bytes
    contentLength = malloc(sizeof(char) * 16);
    sprintf(contentLength, "%d", contentLengthInt);

    char codeStr[3] = {0};
    char *intermediateMessage;
    char *finalMessage;

    // Convert int code to string version
    sprintf(codeStr, "%d", code);

    // Beginning parts of the message
    char *stuff[] = {
            "HTTP/1.1 ",
            "Server: WebServe\r\n",
            "Content-Type: text/html\r\n",
            "Content-Length: ",
            "Connection: Closed\r\n\n"
    };

    // Concatenate all the parts of the message together
    finalMessage = concat(stuff[0], codeStr, 0);
    intermediateMessage = concat(finalMessage, "\r\n", 0);
    free(finalMessage);
    finalMessage = intermediateMessage;
    intermediateMessage = concat(stuff[3], contentLength, 0);
    free(contentLength);
    stuff[3] = intermediateMessage;
    intermediateMessage = concat(stuff[3], "\r\n", 0);
    stuff[3] = intermediateMessage;

    for (int i = 1; i < 5; i++) {
        intermediateMessage = concat(finalMessage, stuff[i], 0);
        free(finalMessage);
        finalMessage = intermediateMessage;
    }

    if (isHead == 0) {
        intermediateMessage = concat(finalMessage, content, 0);
        free(finalMessage);
        finalMessage = intermediateMessage;
        intermediateMessage = concat(finalMessage, "\r\n", 1);
        free(finalMessage);
    }
    finalMessage = intermediateMessage;

    return finalMessage;
}

// Handles an incoming line and amends the describer accordingly
struct MessageDescriber interpretLine(char *line, struct MessageDescriber describer, const int lineNo) {
    char *toMatch = " "; // We'll split it up by spaces
    char **subPtr = &line; // save where we got to when we match the string
    char *current = strtok_r(line, toMatch, subPtr);
    int partNo = 0;

    // Iterate through the different parts of the line and interpret
    while (current != NULL) {
        if (lineNo == 0) {
            if (partNo == 0) { // Type of request, se the request type
                if (strcmp(current, "GET") == 0) {
                    describer.requestType = GET;
                } else if (strcmp(current, "HEAD") == 0) {
                    describer.requestType = HEAD;
                }
            } else if (partNo == 1) { // Resource being requested
                describer.file = current;
            }
        }

        // Find the next part
        current = strtok_r(line, toMatch, subPtr);
        partNo++;
    }

    return describer;
}

// Handles an incoming message and returns a describer
struct MessageDescriber parseMessage(char *content) {
    // Create a new describer
    struct MessageDescriber messageDescriber;
    int lineNo = 0;
    char *line;

    // Now, we need to parse the incoming message
    // We'll split the incoming message by \r\n and interpret it on-the-fly
    char *toMatch = "\r\n";
    char *ptr = strtok(content, toMatch); // get the first match

    while (ptr != NULL) {
        // Copy the line over
        line = (char*) malloc(strlen(ptr));
        strcpy(line, ptr);
        // Interpret the line
        messageDescriber = interpretLine(line, messageDescriber, lineNo);
        // Return the memory

        // find the next new line, or the end of the file
        ptr = strtok(NULL, "\r\n");
        lineNo++;
    }

    return messageDescriber;
}

// Sends a specified error code to the client
void sendError(const int errorCode, const int sock_fd) {
    char *message;

    // Figure out the error code and determine an appropriate response
    switch(errorCode) {
        case 400:
            message = makeResponse(errorCode, "Invalid or malformed request.", 0);
            break;
        default:
            message = makeResponse(errorCode, "Invalid or malformed request.", 0);
            break;
    }

    // Send the message to the client
    sendMessage(sock_fd, message);
    free(message);
}

// Handles a GET request
void handleGet(const struct MessageDescriber describer, const int sock_fd) {
    // validity logic and file obtaining
    char *toSend;
    int statusCode, toFree = 0;

    if (lookupFile(describer.file)) {
        statusCode = 200;
        toSend = readFile(describer.file);
    } else {
        statusCode = 404;
        char *temp = "404. File not found: ";
        toSend = concat(temp, describer.file, 1);
        toFree = 1;
    }

    char *message = makeResponse(statusCode, toSend, 0);
    if (toFree)
        free(toSend);
    sendMessage(sock_fd, message);
}

// Handles a HEAD request
void handleHead(const struct MessageDescriber describer, const int sock_fd) {
    // same logic from get
    char *message = makeResponse(200, "Hello, world", 1);
    sendMessage(sock_fd, message);
}

// Handles a request after it has been parsed.
void handleRequest(const struct MessageDescriber describer, const int sock_fd) {
    switch(describer.requestType) {
        case GET:
            handleGet(describer, sock_fd);
            break;
        case HEAD:
            handleHead(describer, sock_fd);
            break;
        default:
            sendError(400, sock_fd);
            break;
    }
}