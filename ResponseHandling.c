//
// Created by samgunner on 07/09/2019.
//

// include the necessary files
#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _MESSAGEDESCRIBER_C
#include "ClientMessage.c"
#endif

#ifndef _SERVER_MESSAGE_C
#include "ServerMessage.c"
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _FILEUTILITIES_C
#include "FileUtilities.c"
#endif

#ifndef _NETWORK_C
#include "Network.c"
#endif

#include <math.h>

// Concatenates two strings together
// null char indicates if the string needs to be terminated with a null char
char *concat(const char *c1, const char *c2) {
  size_t length = strlen(c1) + strlen(c2) + 1;
  char *final = malloc(length);
  snprintf(final, strlen(c1) + strlen(c2), "%s%s", c1, c2);
  return final;
}

// Forms a textual response that can be sent, given a server message
// Value must be freed after use.
char *makeResponse(const struct ServerMessage toSend) {
  char *otherLines[] = {"Server: WebServe\r\n", "Content-Type: text/html\r\n",
                        "Content-Length: ", "Connection: Closed\r\n", "\r\n\n"};

  size_t totalLength = 0;
  // loop through the content lines and figure out how long the message needs to
  // be
  for (int i = 0; i < 5; i++) {
    totalLength += strlen(otherLines[i]); // Account for null char, remove
  }

  char *firstLine = "HTTP/1.1 ";

  totalLength += toSend.contentLength;
  totalLength += strlen(firstLine) + 4 + strlen(toSend.message) + 4 + 7 +
                 4; // Account for the length needed to represent the content
                    // length as well as carriage return and newline
  totalLength++;    // account for null char

  char *toSendStr = (char *)malloc(sizeof(char) * totalLength + 1);
  snprintf(toSendStr, totalLength, "%s%d%s%s%s", firstLine, toSend.responseCode,
           " ", toSend.message, "\r\n");
  for (int i = 0; i < 5; i++) {
    if (i != 2) {
      snprintf(toSendStr + strlen(toSendStr), totalLength - strlen(toSendStr),
               "%s", otherLines[i]);
    } else {
      snprintf(toSendStr + strlen(toSendStr), totalLength - strlen(toSendStr),
               "%s%lu%s", otherLines[i], toSend.contentLength, "\r\n");
    }
  }

  if (toSend.content)
    snprintf(toSendStr + strlen(toSendStr), totalLength - strlen(toSendStr),
             "%s", toSend.content);

  return toSendStr;
}

// Handles an incoming line and amends the describer accordingly
void interpretLine(char *line, struct ClientMessage *describer,
                   const int lineNo) {
  char *toMatch = " ";   // We'll split it up by spaces
  char **subPtr = &line; // save where we got to when we match the string
  char *current = strtok_r(line, toMatch, subPtr);
  int partNo = 0;

  // Iterate through the different parts of the line and interpret
  while (current != NULL) {
    if (lineNo == 0) {
      if (partNo == 0) { // Type of request, se the request type
        if (strcmp(current, "GET") == 0) {
          describer->requestType = GET;
        } else if (strcmp(current, "HEAD") == 0) {
          describer->requestType = HEAD;
        } else {
          describer->requestType = INVALID;
        }
      } else if (partNo == 1) { // Resource being requested
        switch (describer->requestType) {
        case HEAD:
          describer->head.file = current;
          break;
        case GET:
          describer->head.file = current;
          break;
        case PUT:
          describer->head.file = current;
          break;
        }
      }
    }

    // Find the next part
    current = strtok_r(line, toMatch, subPtr);
    partNo++;
  }
}

// Handles an incoming message and returns a describer
struct ClientMessage *parseMessage(char *content) {
  // Create a new describer
  struct ClientMessage *messageDescriber =
      (struct ClientMessage *)malloc(sizeof(struct ClientMessage));
  char **subPtr = &content;
  int lineNo = 0;

  // Now, we need to parse the incoming message
  // We'll split the incoming message by \r\n and interpret it on-the-fly
  char *ptr = strtok_r(content, "\r\n", subPtr);

  while (ptr != NULL) {
    // Interpret the line
    interpretLine(ptr, messageDescriber, lineNo);
    // Return the memory

    // find the next new line, or the end of the file
    ptr = strtok_r(content, "\r\n", subPtr);
    lineNo++;
  }

  return messageDescriber;
}

// Sends a specified error code to the client
void sendError(struct ServerMessage message, int sock_fd) {
  // Figure out the appropriate response code
  switch (message.responseCode) {
  case 404:
    message.message = "Resource cannot be located.";
    break;
  case 403:
    message.message =
        "You do not have permission to view the specified resource.";
    break;
  case 400:
    message.message = "Invalid or malformed request.";
    break;
  default:
    message.message = "Invalid or malformed request.";
    break;
  }

  message.content = message.message;
  message.contentLength = strlen(message.content);
  char *response = makeResponse(message);
  sendMessage(sock_fd, response);
  free(response);
}

// Handles a GET request
void handleGet(const struct ClientMessage *describer, const int sock_fd) {
  // validity logic and file obtaining
  char *toSend;
  struct ServerMessage sMessage;

  if (lookupFile(describer->get.file)) {
    sMessage.responseCode = 200;
    sMessage.message = "OK";
    toSend = readFile(describer->get.file);
    sMessage.content = toSend;
    sMessage.contentLength = strlen(toSend);
  } else {
    printf("%s%s", "Couldn't find file: ", describer->get.file);
    sMessage.responseCode = 404;
    sendError(sMessage, sock_fd);
    return;
  }

  char *message = makeResponse(sMessage);
  sendMessage(sock_fd, message);
  free(toSend);
  free(message);
}

// Handles a HEAD request
void handleHead(const struct ClientMessage *describer, const int sock_fd) {
  // same logic from get
  struct ServerMessage message;
  message.responseCode = 200;
  message.content = "Hello, world";
  message.contentLength = strlen(message.content);
  message.message = "OK";
  char *messageStr = makeResponse(message);
  sendMessage(sock_fd, messageStr);
  free(messageStr);
}

// handles an invalid request from the client
void handleInvalidRequest(const struct ClientMessage *describer,
                          const int sock_fd) {
  struct ServerMessage message;
  message.responseCode = 400;
  sendError(message, sock_fd);
}

// Handles a request after it has been parsed.
void handleRequest(struct ClientMessage *describer, const int sock_fd,
                   const struct ServerProperties *properties) {
  int response = 1;

  if (describer->requestType == HEAD || describer->requestType == GET ||
      describer->requestType == PUT) {
    response = makeSafe(describer, properties);
  }

  // Check it's a valid request :)
  if (response != 1) {
    struct ServerMessage message;
    message.responseCode = response;
    sendError(message, sock_fd);
  } else {
    switch (describer->requestType) {
    case GET:
      handleGet(describer, sock_fd);
      break;
    case HEAD:
      handleHead(describer, sock_fd);
      break;
    default:
      handleInvalidRequest(describer, sock_fd);
      break;
    }
  }
}