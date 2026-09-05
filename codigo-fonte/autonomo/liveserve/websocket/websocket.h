#ifndef WEBSOCKET_H 
#define WEBSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "Queuen/Queue.h"
#include "capturar_signal.h"

typedef struct{
    int connection_fd;
    socklen_t client_size;
    struct sockaddr_in myserve;
    struct sockaddr_in client;
    char buffer[1024];
} ServerTransport;

typedef  struct {
    int connection_fd;
    int websocket_ative;
    Queue *input;
    char buffer[1024];
    
} CLientTranport;

int websocket_serve(void);

#endif
