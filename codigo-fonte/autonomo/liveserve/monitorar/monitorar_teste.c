#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libtesetes/testes.h"
#include "Queuen/Queue.h"
#include "monitorar.h"
#include "websocket/capturar_signal.h"
Queue output;
volatile int rodando = 1;
pthread_mutex_t block;

void *ler_fila(void *arg)
{
    sleep(2);
    while (rodando) {
        pthread_mutex_lock(&block);
        if(!empytQueue(&output))
        {
            struct Queuedata data = Dequeue(&output);
            printf("%s\n",data.strig);
            if(data.strig != NULL) free(data.strig);
        }
        pthread_mutex_unlock(&block);
        sleep(1);
    }
    return  (void *)(intptr_t) 0;
}

TEST_CASE(teste_monitorar)
{
    signal( SIGINT, captura_signal);
    MESSAGE("TESTAR MODIFICAÇÃO DE ARQUIVOS")    
    newQueue(&output);
    pthread_t monitor_id, ler_F;
    pthread_create(&monitor_id, NULL, monitorar , NULL);
    pthread_create(&ler_F, NULL, ler_fila , NULL);
    void *result;
    pthread_join(monitor_id, result);
    EXPECTED_EQ("ESPERASSE QUE RETORNE SUCESSO", (intptr_t)result, 0);
    pthread_join(ler_F, result);
    EXPECTED_EQ("ESPERASSE QUE RETORNE SUCESSO", (intptr_t)result, 0);
    freeQueuen(&output);
    TESTE_PASS()
}

TEST_SUITE(teste_monitorar)
