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

// Reads a string from a file
char* readFile(const char *address) {
    FILE *fp = fopen(address, "r"); // Open file for reading

    // Find the number of bytes in the file
    fseek(fp, 0, SEEK_END);
    long numBytes = ftell(fp);
    char *contents = (char*) malloc(sizeof(char) * numBytes); // Allocate space for the contents

    // Reset the pointer to the beginning of the file
    fseek(fp, 0L, SEEK_SET);
    fread(contents, sizeof(char), numBytes, fp);
    fclose(fp);

    return contents;
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