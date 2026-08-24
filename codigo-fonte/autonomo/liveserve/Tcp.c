#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "live_serve.h"

Transport Tcp(const char *tipo, int port)
{
    Transport new;
    memset(&new, 0, sizeof(Transport));
    
    new.client_size = sizeof(new.client);                                                         
    new.socket_fd = socket(AF_INET, SOCK_STREAM, 0);                                
                                                                                 
    if(new.socket_fd > 0) printf("\033[1;32m[WS]\033[0m Socket aberto:\n");                                
    else exit(EXIT_FAILURE);                                                                             

    int optar = 1;
    if (setsockopt(new.socket_fd, SOL_SOCKET, SO_REUSEADDR, &optar, sizeof(optar)) < 0) {
        perror("Erro ao definir SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    new.myserve.sin_family = AF_INET;                                               
    new.myserve.sin_port = htons(port);                                             
    new.myserve.sin_addr.s_addr = INADDR_ANY;                                       
    if(!strcmp(tipo, "server")) inet_aton(LOCAL_HOST, &(new.myserve.sin_addr));                                 
                                                                                
    printf("tentando abrira porta %i\n",port);                                  
                                                                                
    if(bind(new.socket_fd, (struct sockaddr*)&new.myserve, sizeof(new.myserve)) != 0)       
    {                                                                           
        printf("houve um problema em abrir a porta\n");                         
        exit(EXIT_FAILURE);                                                               
    }                                                                           
    printf("tipo:%s porta %i aberta\n",tipo, port);                                       
    listen(new.socket_fd, 3);                                                       
    memset(new.buffer, 0, sizeof(new.buffer));

    return new;
}
