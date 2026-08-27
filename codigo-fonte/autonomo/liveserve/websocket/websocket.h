#ifndef WEBSOCKET_H 
#define WEBSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "Queuen/Queue.h"
#include "Tcp/Tcp.h"

static int aguardar_fd(int fd, long timeout_usec);
void calcular_chave_websocket(const char *key, char *output);
void *websocket_serve(void *arg);
char *ler_arquivo(const char* path, const char *type);
void captura_signal(int sinal);

#endif
