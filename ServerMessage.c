//
// Created by samgunner on 11/09/2019.
//

#define _SERVER_MESSAGE_C 1

// Represents the response from the server to the client
struct ServerMessage {
    int responseCode;
    unsigned long contentLength; // The length of the content to be sent
    char *message; // 200 "OK", 404 "File not found", etc... (can be null)
    char *content; // Content to send to the user, can be null in case of a head response
};