#include "../live_serve.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define QUEUEN_H
#include "../Queue.h"

extern Queue input;
int ws_port = 8081;
volatile int rodando = 1;
volatile int flag = -1;
int time_aguardar = 200000;
pthread_mutex_t block;

int main(void){
    signal(SIGINT, captura_signal);
    Transport websocket = Tcp("client", ws_port);
    pthread_t websocket_id;
    pthread_create(&websocket_id, NULL, websocket_serve, (void *)&websocket);
    while (rodando) {
        pthread_mutex_lock(&block);
        if(!empytQueue(&input)){
            struct Queuedata data = Dequeue(&input);
            printf("[websocket_id : %d mensage enviada:[ %s ]\n", data.socket_fd, data.strig);
            free(data.strig);
        }
        pthread_mutex_unlock(&block);
        
        usleep(100);
    }
    pthread_join(websocket_id,NULL);
    return 0;
}

