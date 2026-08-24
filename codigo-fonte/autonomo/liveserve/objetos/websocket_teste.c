#include "../live_serve.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

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
    pthread_join(websocket_id, NULL);

    return 0;
}

