#include <stdio.h>
#include "StructureDate.h"

void newStack(Stack *p){
	p->top = -1;
};

int empytStack(Stack *p){
	return p->top == -1;
};

int isFullStack(Stack *p){
	return p->top == (TAM_MAX - 1);
};

void pushStack(Stack *p, int date){
	if(isFullStack(p)){
		printf("Erro: Stack is full!\n");
		return;
		};
	p->top++;
	p->items[p->top] = date;
	printf("Elemento %d inserido com sucesso!\n",date);
};

int popStack(Stack *p){
	if(empytStack(p)){
		printf("Stack is empyt\n");
		return -1;
	};
	int valor = p->items[p->top];
	p->top--;
	printf("remove is sucess\n");
	return valor;
};

int peekStack(Stack *p){
	if(empytStack(p)){
		printf("Erro: Stack is empyt!\n");
		return -1;
	};
	return p->items[p->top];
};

	
