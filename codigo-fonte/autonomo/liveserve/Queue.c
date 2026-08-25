#include <stdlib.h>
#include <string.h>
#define QUEUEN_H 
#include "Queue.h"

int newQueue(Queue *p)
{
    *p = (Queue){NULL, NULL, 0};
    return 0;
}

int empytQueue(Queue *p)
{
    return (int)(p->size == 0);
};

int Enqueue(Queue *p, const char *string, int socket_fd)
{
    unsigned long size = strlen(string);
    if(p == NULL || string == NULL) return 1;
    struct node *new = malloc(sizeof(struct node));
    if(new == NULL) return 1;
    char *newstring = malloc(size + 1);
    if(newstring == NULL) return 1;

    strcpy(newstring, string);
    if(strcmp(string, newstring)){ 
        free(new);
        free(newstring);
        return 1;
    }
    new->strig = newstring;
    new->socket_fd = socket_fd;
    new->prox = NULL;
    if(p->size == 0 || p->first == NULL){
        p->first = new;
        p->tail = new;
        p->size++;
        return 0;
    }
    p->tail->prox = new;
    p->tail = new;
    p->size++;
    return 0;
}
struct node Dequeue(Queue *p)
{
    if(p->size == 0){
         return (struct node){0, NULL, NULL};
    }
    struct node new = {p->first->socket_fd, p->first->strig, NULL};
    struct node *freeno = p->first;
    p->first = p->first->prox; 
    free(freeno);
    p->size--;
    return new;
};

struct node peekQueue(Queue *p)
{
    struct node new = {p->first->socket_fd, p->first->strig, NULL};
    return new;
};

int freeQueuen(Queue *p)
{
    struct node *temp = p->first;
    while (temp != NULL) {
        free(temp->strig);
        free(temp);
        temp = temp->prox;
    }
    p->first = NULL;
    p->tail =NULL;
    p->size = 0;
    return 0;    
}
