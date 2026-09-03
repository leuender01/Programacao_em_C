#include <pthread.h>
#include <stdio.h>
#include "capturar_signal.h"

//extern pthread_mutex_t block;

extern int rodando;
void captura_signal(int sinal){
    printf("\033[36;1m[SISTEMA \033[0m\033[36m]\033[0m: Encerrando suavemente\n");
//    pthread_mutex_lock(&block);
    rodando = 0;
//    pthread_mutex_unlock(&block);
}
