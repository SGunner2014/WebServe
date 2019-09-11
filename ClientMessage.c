//
// Created by samgunner on 07/09/2019.
//

#define _MESSAGEDESCRIBER_C 1

enum REQUEST_TYPE {
    INVALID,
    GET,
    HEAD,
    PUT
};

// Represents a HEAD request
struct HeadRequest {
    char *file;
};

// Represents a GET request
struct GetRequest {
    char *file;
};

// Represents a PUT request
struct PutRequest {
    char *file;
    char *content;
};

// Represents a general message from the client
struct ClientMessage {
    enum REQUEST_TYPE requestType;

    union {
        struct HeadRequest head;
        struct GetRequest get;
        struct PutRequest put;
    };
};