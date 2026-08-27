#ifndef QUEUE_H
#define QUEUE_H

struct queuenRB{
    int fd;
    struct queuenRB *prox;
};

typedef struct {
    struct queuenRB *tail;
    struct queuenRB *first;
    unsigned long size;
} QueuenRB;

int newQueue(QueuenRB *p); 
int empytQueue(QueuenRB *p); 
int Enqueue(QueuenRB *p, int value); 
int Dequeue(QueuenRB *p); 
int peekQueue(QueuenRB *p); 

#endif
