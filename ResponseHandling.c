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

// Concatenates two strings together
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
char *makeResponse(int code, char *content) {
    char codeStr[3] = {0};
    int contentLengthInt = (int) strlen(content); // Length of content, in bytes
    char *contentLength = malloc(sizeof(char) * 16);
    sprintf(contentLength, "%d", contentLengthInt);
    printf("%s", contentLength);
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
            "Connection: close\r\n\n"
    };

    // Concatenate all the parts of the message together
    finalMessage = concat(stuff[0], "200", 0);
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

    intermediateMessage = concat(finalMessage, content, 0);
    free(finalMessage);
    finalMessage = intermediateMessage;
    intermediateMessage = concat(finalMessage, "\r\n", 1);
    free(finalMessage);
    finalMessage = intermediateMessage;

    return finalMessage;
}

// Handles an incoming line and amends the describer accordingly
void interpretLine(char *line, struct MessageDescriber *describer, const int lineNo) {
    char *toMatch = " "; // We'll split it up by spaces
    char *subPtr = strtok(line, toMatch);
    int partNo = 0;

    while (subPtr != NULL) {
        if (lineNo == 0) {
            if (partNo == 0) {
                if (strcmp(subPtr, "GET") < 0) {
                    describer->requestType = 1;
                }
            }
        }

        // Find the next part
        subPtr = strtok(NULL, toMatch);

        partNo++;
    }
}

// Handles an incoming message and returns a describer
struct MessageDescriber *parseMessage(char *content) {
    // Create a new describer
    struct MessageDescriber *describer = (struct MessageDescriber*) (sizeof(struct MessageDescriber));
    int lineNo = 0;
    char *line;

    // Now, we need to parse the incoming message
    // We'll split the incoming message by \r\n and interpret it on-the-fly
    char *toMatch = "\r\n";
    char *ptr = strtok(content, toMatch);

    while (ptr != NULL) {
        // Copy the line over
        line = (char*) malloc(strlen(ptr));
        strcpy(line, ptr);
        // Interpret the line
        interpretLine(ptr, describer, lineNo);
        // Return the memory
        free(line);

        // find the next new line, or the end of the file
        ptr = strtok(NULL, "\r\n");
        lineNo++;
    }

    return NULL;
}