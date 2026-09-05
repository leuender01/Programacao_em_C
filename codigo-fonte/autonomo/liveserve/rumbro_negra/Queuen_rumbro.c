#include <stdlib.h>
#include <stdint.h>
#include "arvore_rumbro_negra.h"
#include "Queuen_rumbro.h"

int newQueueRB(QueuenRB *p)
{
	p->tail = NULL;
	p->first = NULL;
	p->size = 0;
	return 0; 
}

int empytQueueRB(QueuenRB *p)
{
	return (int)(p->size == 0);
}

int EnqueueRB(QueuenRB *p, struct nodo* value)
{
    if(p == NULL) return 1;
    if(value == NULL) return 1;
    struct queuenRB *newnode = malloc(sizeof(struct queuenRB));
    if(newnode == NULL) return 1;
    *newnode = (struct queuenRB){value, NULL};
    if(p->size == 0 || p->first == NULL)
    {
        *p = (QueuenRB){.first=newnode, .tail=newnode};
        p->size++;
        return 0;
    }
    p->tail->prox = newnode;
    p->tail = newnode;
    p->size++;
    return 0;
}

struct nodo DequeueRB(QueuenRB *p)
{
    if(p == NULL || empytQueueRB(p)) return (struct nodo){.color=BLACK, .key=-1, .left=NULL, .right=NULL};
    struct nodo *result = p->first->fd;
    struct queuenRB  *freeno = p->first;
    p->first = p->first->prox;
    p->size--;
    free(freeno);
    return *result;

}
int freeQueuenRB(QueuenRB *p)
{
    struct queuenRB *temp = p->first;
    while (temp != NULL) 
    {
        struct queuenRB *aux = temp->prox;
        free(temp);
        temp = aux;
    }
    *p = (QueuenRB){.first=NULL, .tail=NULL, .size=0};
    return 0;
}

struct nodo peekQueueRB(QueuenRB *p)
{
    if(p->first == NULL) return (struct nodo){.color=BLACK, .key=-1, .left=NULL, .right=NULL};
    return *p->first->fd;
}
