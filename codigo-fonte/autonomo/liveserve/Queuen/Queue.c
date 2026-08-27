#include <stdlib.h>
#include <string.h>
#include "Queue.h"

int newQueue(Queue *p)
{
    *p = (Queue){NULL, NULL, 0};
    return 0;
}

int empytQueue(Queue *p)
{
    return (int)(p->size == 0);
}

int Enqueue(Queue *p, const char *string, int socket_fd)
{
    unsigned long size = strlen(string);
    if(p == NULL || string == NULL) return 1;
    struct Queuedata *new = malloc(sizeof(struct Queuedata));
    if(new == NULL) return 1;
    char *newstring = malloc(size + 1);
    if(newstring == NULL) return 1;

    strcpy(newstring, string);
    if(strcmp(string, newstring)){ 
        free(new);
        free(newstring);
        return 1;
    }
    *new = (struct Queuedata){socket_fd, newstring, NULL};
    if(p->size == 0 || p->first == NULL){
        *p = (Queue){.first=new, .tail=new};
        p->size++;
        return 0;
    }
    p->tail->prox = new;
    p->tail = new;
    p->size++;
    return 0;
}
struct Queuedata Dequeue(Queue *p)
{
    if(p->size == 0){
         return (struct Queuedata){0, NULL, NULL};
    }
    struct Queuedata new = {p->first->socket_fd, p->first->strig, NULL};
    struct Queuedata *freeno = p->first;
    p->first = p->first->prox; 
    free(freeno);
    p->size--;
    return new;
};

struct Queuedata peekQueue(Queue *p)
{
    struct Queuedata new = {p->first->socket_fd, p->first->strig, NULL};
    return new;
}

int freeQueuen(Queue *p)
{
    struct Queuedata *temp = p->first;
    while (temp != NULL) {
        free(temp->strig);
        free(temp);
        temp = temp->prox;
    }
    *p = (Queue){NULL, NULL, 0};
    return 0;    
}
