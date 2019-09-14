//
// Created by samgunner on 13/09/2019.
//

#ifndef _PROPERTIES_C
#include "Properties.c"
#endif

#ifndef _FILEUTILITIES_C
#include "FileUtilities.c"
#endif

// Parses a properties file.
struct ServerProperties *parseProperties(char *contents) {
    printf("contents:%s\n", contents);
    fflush(stdout);

    struct ServerProperties *properties = malloc(sizeof(struct ServerProperties));
    char **firstPointer = &contents;
    char **secondPointer;
    char *firstCurrent = strtok_r(contents, "\r\n", firstPointer);
    char *secondCurrent;
    int i;
    enum mode {
        SERVING_DIRECTORY,
        SERVER_IDENTIFIER,
        SERVER_VERSION,
        SERVER_INDEX
    } mode;

    while (firstCurrent != NULL) {
        secondPointer = &firstCurrent;
        secondCurrent = strtok_r(firstCurrent, " ", secondPointer);

        for (i = 0; i < 2; i++) {
            if (i == 0) {
                if (strcmp(secondCurrent, "SERVING_DIRECTORY") == 0)
                    mode = SERVING_DIRECTORY;
                else if (strcmp(secondCurrent, "SERVER_IDENTIFIER") == 0)
                    mode = SERVER_IDENTIFIER;
                else if (strcmp(secondCurrent, "SERVER_VERSION") == 0)
                    mode = SERVER_VERSION;
                else if (strcmp(secondCurrent, "SERVER_INDEX") == 0)
                    mode = SERVER_INDEX;
            } else {
                switch(mode) {
                    case SERVING_DIRECTORY:
                        properties->servingDirectory = secondCurrent;
                        break;
                    case SERVER_IDENTIFIER:
                        properties->serverIdentifier = secondCurrent;
                        break;
                    case SERVER_VERSION:
                        properties->serverVersion = secondCurrent;
                        break;
                    case SERVER_INDEX:
                        properties->serverIndex = secondCurrent;
                        break;
                }
            }
            secondCurrent = strtok_r(firstCurrent, " ", secondPointer);
        }

        firstCurrent = strtok_r(contents, "\r\n", firstPointer);
    }

    return properties;
}

// Reads in a config file from a specified path
struct ServerProperties *readProperties(char *path) {
    if (lookupFile(path)) {
        printf("path:%s\n", path);
        char *contents = readFile(path);
        struct ServerProperties *props = parseProperties(contents);
        free(contents);
        return props;
    } else {
        printf("%s", "The specified config file could not be found.");
        exit(-1);
    }
}