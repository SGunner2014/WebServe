//
// Created by samgunner on 07/09/2019.
//

#define _MESSAGEDESCRIBER_C 1

struct MessageDescriber {
    enum RequestType {
        GET,
        HEAD,
        PUT
    };

    // The file the client is trying to request, if any
    char *file;
};