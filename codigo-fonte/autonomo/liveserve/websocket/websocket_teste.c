#include "libtesetes/testes.h"
#include "websocket.h"
#include "websocket_client.h"
#include "rumbro_negra/arvore_rumbro_negra.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int ws_port = 8081;
extern Queue output;
extern volatile int rodando;
extern RBtree situacao_atual;
Queue system_message_cliete;
const int simultaneas = 350;
pthread_t* simultaneas_id;

TEST_CASE(websocket_init)
{
    MESSAGE("INCIANDO SERVIDOR WEBSOCKET");
    signal(SIGINT, captura_signal);
    Transport websocket = Tcp("client", ws_port);
    pthread_t websocket_id;
    pthread_create(&websocket_id, NULL, websocket_serve, (void *)&websocket);
    for(int i = 0; i < 3; i++) {
        if(rodando == 0) break;
        if(situacao_atual.size != 0) Enqueue(&output, "reload", 0);
        sleep(15);
    }
    if(rodando == 1) rodando = 0;
    pthread_join(websocket_id,NULL);
    TESTE_PASS();
}

TEST_CASE(websocket_cliente_teste)
{
    MESSAGE("TESTANDO VARIAS CONEXÕES AO MESMO TEMPO");
    sleep(3);
    newQueue(&system_message_cliete);
    simultaneas_id = malloc(sizeof(pthread_t) * simultaneas); 
    int resultado = 0;
    for(int i = 0; i < simultaneas; i++) {
        pthread_create(&simultaneas_id[i], NULL, websocket_cliente, NULL);
    }
    for(int i = 0; i < simultaneas; i++) {
        void *resultadot;
        pthread_join(simultaneas_id[i], &resultadot);
        if((intptr_t)resultadot == 0) resultado++;
    }
    while (!empytQueue(&system_message_cliete)) {
        struct Queuedata data =  Dequeue(&system_message_cliete);
        printf("Tempo de resposta em ms : %s , [WS: %d]\n", data.strig, data.socket_fd);
        free(data.strig);
    }
    freeQueuen(&system_message_cliete);
    free(simultaneas_id);
    if(rodando == 1) rodando = 0;
    SUMARY(resultado, simultaneas);
    TESTE_PASS();
}


TEST_SUITE_ISOLATE(websocket_init, websocket_cliente_teste)
