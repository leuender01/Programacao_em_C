#ifndef QUEUE_H
#define QUEUE_H

/* 
    com um monte de pesquisa no 
    google e algumas noites em 
    claro essa eu fiz, foi o 
    melhor que eu consegui fazer
    espero que tenha ficado bom
*/

#define TAM_MAX 1000 //so um limite minimo mais pode ser estendito para mais
#include <stdlib.h> 
#include <stdbool.h>
#include "../arvores/arvore-base.h" // essa e so uma adptação para usar em arvores binarias

typedef struct Element{
    NODE node;
    struct Element *prox;
}Element;

typedef struct{ 
    int size;
    Element *tail; 
    Element *front;
}Queue;

bool newQueue(Queue *p); //inicializa a filla
bool empytQueue(Queue *p); // verifica se esta vazia util para a parte interna mais pode ser tirado
bool isFullQueue(Queue *p); // verifica se esta cheia tambem pode ser descartada
bool Enqueue(Queue *p, NODE node); // insere um elemento na Fila
struct no* Dequeue(Queue *p); // devolve o elemento retirando o da fila
NODE peekQueue(Queue *p); // devolve o primeiro elemento sem retiralo da fila

#endif
