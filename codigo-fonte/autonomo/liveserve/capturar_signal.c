#include <pthread.h>
#include <stdio.h>
#include "live_serve.h"

extern int rodando;
extern pthread_mutex_t block;

void captura_signal(int sinal){
    printf("\033[36;1m[SISTEMA \033[0m\033[36m]\033[0m: Encerrando suavemente\n");
    pthread_mutex_lock(&block);
    rodando = 0;
    pthread_mutex_unlock(&block);
}
