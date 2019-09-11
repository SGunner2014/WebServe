//
// Created by samgunner on 07/09/2019.
//

#define _MESSAGEDESCRIBER_C 1

enum REQUEST_TYPE {
    GET,
    HEAD,
    PUT
};

struct MessageDescriber {
    enum REQUEST_TYPE requestType;

    // The file the client is trying to request, if any
    char *file;
};