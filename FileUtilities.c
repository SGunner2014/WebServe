//
// Created by samgunner on 07/09/2019.
//

#define _FILEUTILITIES_C 1

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#include "Properties.c"

// Reads a string from a file
char* readFile(const char *address) {
    FILE *fp = fopen(address, "r"); // Open file for reading

    // Find the number of bytes in the file
    fseek(fp, 0, SEEK_END);
    long numBytes = ftell(fp);
    char *contents = (char*) calloc(numBytes, sizeof(char)); // Allocate space for the contents

    // Reset the pointer to the beginning of the file
    fseek(fp, 0L, SEEK_SET);
    fread(contents, sizeof(char), numBytes, fp);
    fclose(fp);

    return contents;
}

// Makes sure the client is accessing a valid file and that they have permission to access it
// Returns an error message if it cannot be made safe, otherwise 0
int makeSafe(struct ClientMessage *message, const struct ServerProperties *properties) {
//    printf("got here");
    size_t rootLength = strlen(properties->servingDirectory);
    int toReturn;
    char *requestedFile;
    char *combinedPath;
    char *evaluatedPath;

    switch(message->requestType) {
        case HEAD:
            requestedFile = message->head.file;
            break;
        case GET:
            requestedFile = message->get.file;
            break;
        case PUT:
            requestedFile = message->put.file;
            break;
        default:
            return 400;
    }

    

    size_t totalLength = strlen(requestedFile) + rootLength + 1;
    combinedPath = (char*) calloc((strlen(requestedFile) + rootLength + 1), sizeof(char));
    snprintf(combinedPath, totalLength, "%s%s", properties->servingDirectory, requestedFile);
//    strcpy(combinedPath, properties->servingDirectory);
//    strcat(combinedPath, requestedFile);
//    printf("\n\ncombined|%s|\n\n", combinedPath);
    evaluatedPath = realpath(combinedPath, NULL);

    if (evaluatedPath == NULL)
        toReturn = 404;
    else if (strncmp(properties->servingDirectory, evaluatedPath, rootLength) == 0) {
        toReturn = 1;
    } else {
        toReturn = 403;
    }

//    printf("evaluated:%s\ncombined:%s\n", evaluatedPath, combinedPath);

    switch (message->requestType) {
        case HEAD:
            message->head.file = evaluatedPath;
            break;
        case GET:
            message->get.file = evaluatedPath;
            break;
        case PUT:
            message->put.file = evaluatedPath;
            break;
    }

    free(combinedPath);
    return toReturn;
}

// Writes a string to a file.
void writeFile(const char *address, const char *contents) {
    FILE *fp = fopen(address, "w+"); // Open the file and clear the contents
    fputs(contents, fp);
    fclose(fp);
}

// Checks to see if a file exists
int lookupFile(char *address) {
    struct stat buffer;
    return stat(address, &buffer) == 0 ? 1 : 0;
}