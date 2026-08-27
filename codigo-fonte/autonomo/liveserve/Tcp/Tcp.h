#ifndef TCP_H
#define TCP_H

#define LOCAL_HOST "0.0.0.0"

#include <sys/socket.h>
#include <netinet/in.h>
#include "Queuen/Queue.h"

typedef struct{
    int socket_fd, connection_fd;  
    socklen_t client_size;
    struct sockaddr_in myserve;
    struct sockaddr_in client;
    char buffer[1024];
    int websocket_ative;
    Queue *input;
} Transport;             
Transport Tcp(const char *tipo, int port);

#endif
