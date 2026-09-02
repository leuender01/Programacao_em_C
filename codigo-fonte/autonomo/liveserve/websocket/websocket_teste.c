#include "websocket.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

int ws_port = 8081;
extern Queue output;
extern volatile int rodando;

int main(void){
    signal(SIGINT, captura_signal);
    Transport websocket = Tcp("client", ws_port);
    pthread_t websocket_id;
    pthread_create(&websocket_id, NULL, websocket_serve, (void *)&websocket);
    sleep(3);
    while (rodando) {
        Enqueue(&output, "reload", 0);
        sleep(1);
    }
    pthread_join(websocket_id,NULL);
    return 0;
}

