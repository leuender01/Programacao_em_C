#include <stdlib.h>
#include <stdint.h>
#include "Queuen_rumbro.h"

int newQueueRB(QueuenRB *p){
	p->tail = NULL;
	p->first = NULL;
	p->size = 0;
	return 0; 
}

int empytQueueRB(QueuenRB *p){
	return (int)(p->size == 0);
}

int EnqueueRB(QueuenRB *p, int value){
    return 0;
}

int DequeueRB(QueuenRB *p){
    return 0;

}
int peekQueueRB(QueuenRB *p){
    return 0;
}
