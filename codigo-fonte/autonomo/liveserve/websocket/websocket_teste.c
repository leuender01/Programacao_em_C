#include "libtesetes/testes.h"
#include "websocket.h"
#include "websocket_client.h"
#include "rumbro_negra/arvore_rumbro_negra.h"
#include "capturar_signal.h"
#include "hash/hash.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

extern Queue output;
volatile int rodando = 1;
extern RBtree situacao_atual;
Queue system_message_cliete;
const int simultaneas = 500;
pthread_t* simultaneas_id;

void* enviar_reload(void *arg)
{
    sleep(7);
    for(int i = 0; i < 3; i++) {
        if(rodando == 0) break;
        if(situacao_atual.size != 0) Enqueue(&output, "reload", 0);
        sleep(15);
    }
    if(rodando == 1) rodando = 0;
    return NULL;
}

TEST_CASE(websocket_init)
{
    MESSAGE("INCIANDO SERVIDOR WEBSOCKET");
    signal(SIGINT, captura_signal);
    pthread_t teste_id;
    pthread_create(&teste_id, NULL, enviar_reload, NULL);
    pthread_detach(teste_id);
    EXPECTED_EQ("ESPERASSE QUE SAI COM SUCESSO", websocket_serve(), 0);
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
TEST_CASE(websocket_serveA){
    signal(SIGINT, captura_signal);
    websocket_serve();
    TESTE_PASS();
}

TEST_CASE(tamanho_estruturas){
    printf("Tamnho clientreWS: %ld\n", sizeof(CLientTranport));
    printf("Tamnho serverWS: %ld\n", sizeof(ServerTransport));
    printf("Tamnho Queen: %ld\n", sizeof(Queue));
    printf("Tamnho struct Queuedata: %ld\n", sizeof(struct Queuedata));
    printf("Tamnho Rbtree: %ld\n", sizeof(RBtree));
    printf("Tamnho struct nodo: %ld\n", sizeof(struct nodo));
    printf("Tamnho Hash: %ld\n", sizeof(HASH));
    printf("Tamnho struct node: %ld\n", sizeof(struct node));
    TESTE_PASS()
}


TEST_SUITE_ISOLATE(websocket_init, websocket_cliente_teste)
//TEST_SUITE(tamanho_estruturas)
