//
// Created by samgunner on 07/09/2019.
//

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _MESSAGEDESCRIBER_C
#include "MessageDescriber.c"
#endif

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
    char *finalMessage = concat(stuff[0], "200", 0);
    finalMessage = concat(finalMessage, "\r\n", 0);
    stuff[3] = concat(stuff[3], contentLength, 0);
    stuff[3] = concat(stuff[3], "\r\n", 0);

    for (int i = 1; i < 5; i++) {
        finalMessage = concat(finalMessage, stuff[i], 0);
    }

    finalMessage = concat(finalMessage, content, 0);
    finalMessage = concat(finalMessage, "\r\n", 1);

    return finalMessage;
}

// Handles an incoming message and returns a describer
struct MessageDescriber *handleMessage(char *content) {
    struct MessageDescriber describer;


}