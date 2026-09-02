//#include <stdio.h>
//#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include "Queuen_rumbro.h"
#include "./libtesetes/testes.h"
#include "arvore_rumbro_negra.h"

#define SIZE_ 1000

TEST_CASE(testado_criarUmafila)
{
    MESSAGE("TESTANDO SE A CRIAÇÃO DA FILA E VALIDA");
    QueuenRB fila;
    newQueueRB(&fila);
    EXPECT_COND(fila.size == 0 && fila.first == NULL && fila.tail == NULL);
    EXPECTED_EQ("TESTANDO A LIBARAR MEMORIA", freeQueuenRB(&fila), 0);
    TESTE_PASS();
}

TEST_CASE(testando_Enqueuen)
{
    MESSAGE("TESTANDO SE A INSERÇÃO NA FILA");
    QueuenRB fila;
    newQueueRB(&fila);
    EXPECT_COND(fila.size == 0 && fila.first == NULL && fila.tail == NULL);
    struct nodo valuesteste[SIZE_] = {0};
    int inseriodos = 0;
    srand(time(NULL));
    for(int i = 0; i < SIZE_; i++){
        valuesteste[i] = (struct nodo){.key = numerosAletaorios(100), .color=BLACK, .left=NULL, .right=NULL, .info=(void *)&valuesteste[i]};
        if(EnqueueRB(&fila, &valuesteste[i]) == 0) inseriodos++;
    }
    EXPECT_COND(fila.size == inseriodos);
    SUMARY(inseriodos, fila.size);
    EXPECTED_EQ("TESTANDO A LIBARAR MEMORIA", freeQueuenRB(&fila), 0);
    TESTE_PASS();
}

TEST_CASE(testando_Dequeue)
{
    MESSAGE("DESCARREGANDO A FILA");
    QueuenRB fila;
    newQueueRB(&fila);
    srand(time(NULL));
    EXPECT_COND(fila.size == 0 && fila.first == NULL && fila.tail == NULL);
    struct nodo valuesteste[SIZE_] = {0};
    int inseriodos = 0;
    for(int i = 0; i < SIZE_; i++){
        valuesteste[i] = (struct nodo){.key = numerosAletaorios(100), .color=BLACK, .left=NULL, .right=NULL, .info=(void *)&valuesteste[i]};
        if(EnqueueRB(&fila, &valuesteste[i]) == 0) inseriodos++;
    }
    EXPECT_COND(fila.size == inseriodos);
    SUMARY(inseriodos, fila.size);
    int old_value = fila.size;
    while (!empytQueueRB(&fila)) {
        DequeueRB(&fila);
    }
    MESSAGE("TESTANDO SE A FILA ESTA VAZIA");
    SUMARY(fila.size, old_value);
    EXPECTED_EQ("TESTANDO SE A FILA ESTA VAZIA ", fila.size, 0);
    TESTE_PASS();
}

TEST_SUITE(testado_criarUmafila, testando_Dequeue, testando_Enqueuen)

