#include <stdio.h>
#include <stdlib.h>
#define QUEUEN_H
#include "../Queue.h"

int main(void){

    Queue fila;
    newQueue(&fila);
    Enqueue(&fila, "ola", 0);
    Enqueue(&fila, "gato", 0);
    Enqueue(&fila, "bala", 0);
    Enqueue(&fila, "fala", 0);
    Enqueue(&fila, "sala", 0);
    printf("%s, %d\n", peekQueue(&fila).strig, peekQueue(&fila).socket_fd);
    
    while (fila.size > 0){
        struct node teste = Dequeue(&fila);
        printf("%s, %ld\n", teste.strig, fila.size);
        free(teste.strig);
    }
    
//    freeQueuen(&fila);

    return 0;
}

