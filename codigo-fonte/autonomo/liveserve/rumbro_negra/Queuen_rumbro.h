#ifndef QUEUE_H
#define QUEUE_H
#include "arvore_rumbro_negra.h"

struct queuenRB{
    struct nodo fd;
    struct queuenRB *prox;
};

typedef struct {
    struct queuenRB *tail;
    struct queuenRB *first;
    unsigned long size;
} QueuenRB;

int newQueueRB(QueuenRB *p); 
int empytQueueRB(QueuenRB *p); 
int EnqueueRB(QueuenRB *p, struct nodo* value);
struct nodo DequeueRB(QueuenRB *p);
struct nodo peekQueueRB(QueuenRB *p);
int freeQueuenRB(QueuenRB *p);

#endif
