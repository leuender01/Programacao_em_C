#include <stdio.h>
#include "Queuen_rumbro.h"
#include "./libtesetes/testes.h"
#include "arvore_rumbro_negra.h"

#define SIZE_  100
struct nodo valuesteste[SIZE_] = {0};

int main(void){
    QueuenRB fila;
    newQueueRB(&fila);
    for(int i = 0; i < SIZE_; i++){
        valuesteste[i] = (struct nodo){.fd = numerosAletaorios(100), .color=BLACK, .left=NULL, .right=NULL};
        EnqueueRB(&fila, &valuesteste[i]);
    }
    printf("%d : %d\n", peekQueueRB(&fila).fd, valuesteste[0].fd);
    for(int i = 0; i < SIZE_; i++){
        printf("valor[%d]  %d : [ DequeueRB : %d : size = %lu]\n", i, valuesteste[i].fd, DequeueRB(&fila).fd, fila.size);
    }
    return 0;
}

