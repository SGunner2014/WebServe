//
// Created by samgunner on 07/09/2019.
//

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#include <unistd.h>

#define ROOT_ADDRESS = "/home/samgunner/";

// Checks to see if a file exists
int lookupFile(char *address) {
    return access(address, F_OK) != -1;
}