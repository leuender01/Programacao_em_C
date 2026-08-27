#include <stdio.h>
#include <stdlib.h>
#include "arvore_rumbro_negra.h"
#define QUEUEN_RUMBRO_H
#include "Queue.h"

int newRBtree(RBtree *tree){
    tree->raiz = NULL;
    tree->size = 0;
    return 0;
}

struct nodo *newNodo(int value){
    struct nodo *newno = NULL;
    newno = malloc(sizeof(struct nodo));
    if(newno != NULL) *newno = (struct nodo){.color=BLACK, .fd=value, .left=NULL, .right=NULL};
    return newno;
 }

int insertRBtree(RBtree *tree, int value){
    if(tree == NULL) return 1;
    struct nodo *temp = tree->raiz;
    if(temp == NULL){
        temp = newNodo(value);
        if(temp == NULL) return 1;
        tree->raiz = temp;
    }
    tree->size++;
    return 0;
}

int porlevel(RBtree *tree){
    if(tree == NULL) return 1;
    Queue fila;
    newQueue(&fila);
    struct nodo *aux = tree->raiz;
    Enqueue(&fila, tree->raiz);
    while (!empytQueue(&fila)) 
    {
        struct Queuedata data = Dequeue(&fila);
        aux = data.no;
        if(aux->left != NULL) Enqueue(&fila, aux->left);
        if(aux->right != NULL) Enqueue(&fila, aux->right);
        printf("%d\n", aux->fd);
    }
    printf("\n");
    return 0;
}

void _freenode(struct nodo *no){
    if(no == NULL) return;
    if(no->left != NULL) _freenode(no->left);
    if(no->right != NULL) _freenode(no->right);
    free(no);
    return;
}

int freeRB(RBtree *tree){
    if(tree == NULL) return 1;
    struct nodo *raiz = tree->raiz;
    _freenode(raiz);
    return 0;
}
