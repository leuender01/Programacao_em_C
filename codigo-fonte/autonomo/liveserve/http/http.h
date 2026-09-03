#ifndef HTTP_H
#define HTTP_H

#include "websocket/capturar_signal.h"
#include <netinet/in.h>
#include <sys/socket.h>

struct  serverHTTP{
    int fd;
    socklen_t client_size;
    struct sockaddr_in myserver;
    struct sockaddr_in client;
};

struct listaArquivos{
    char **string;
    unsigned long legth;
};

void *http_server(void *);
#endif
