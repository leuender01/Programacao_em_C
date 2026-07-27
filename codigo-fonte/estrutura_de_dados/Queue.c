#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Queue.h"

/*
	Nao e nada muito rebuscado mais ate agora ta funcionando.
*/

bool newQueue(Queue *p){
	
	/*
		Inicializa a Fila.
	*/

	p->tail = NULL;
	p->front = NULL;
	p->size = 0;
	return true; // retorna true porque eu quis
};

bool empytQueue(Queue *p){

	/*
		Verifica se esta vazia, pode ser descartada.
	*/

	return (p->size == 0);
};
bool isFullQueue(Queue *p){

	/*
		Verifica se esta cheia, pode ser descartada.
	*/

	return (p->size == TAM_MAX);
};

bool Enqueue(Queue *p, NODE node){

	/*
		Insere um elemento e como so estivesse em
	*/
	if(isFullQueue(p)){
		printf("Queue is Full\n");
		return false;
	};
	Element *novo = (Element*)malloc(sizeof(Element));
	if(novo){
		novo->node = node;
		novo->prox = NULL;
		if(p->front == NULL){ 
			p->front = novo;
		 }else{
			p->tail->prox = novo;
		};
		p->tail = novo;
		p->size++;
		}else{
			return false;
		};
		return true;
};
struct no* Dequeue(Queue *p){
	if(empytQueue(p)) return NULL;
	Element *temp = p->front;
	NODE node = temp->node;
	p->front = p->front->prox;
	if(p->front == NULL) p->tail = NULL;
	free(temp);
	p->size--;
	return node;

};
NODE peekQueue(Queue *p){
	if(p->front == NULL) return NULL;
	return p->front->node;
};
