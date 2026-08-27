#include <stdio.h>
#include <stdlib.h>
#include "arvore_rumbro_negra.h"
#include "Queuen_rumbro.h"

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

struct nodo *__insertRB(struct nodo *temp, int value, RBtree *tree){
    if(temp != NULL && temp->fd == value) return temp;
    if(temp == NULL){
        temp = newNodo(value);
        tree->size++;
    }else if(value > temp->fd){
        temp->right = __insertRB(temp->right, value, tree);
    }else{
        temp->left = __insertRB(temp->left, value, tree);
    }
    return temp;
}

int insertRBtree(RBtree *tree, int value){
    if(tree == NULL) return 1;
    struct nodo *temp = tree->raiz;
    if(temp == NULL){
        temp = newNodo(value);
        if(temp == NULL) return 1;
        tree->raiz = temp;
        tree->size++;
        return 0;
    }
    unsigned long old = tree->size;
    tree->raiz = __insertRB(temp, value, tree);
    return (old == tree->size);
}

int porlevel(RBtree *tree){
    if(tree == NULL) return 1;
    QueuenRB fila;
    newQueueRB(&fila);
    EnqueueRB(&fila, tree->raiz);
    while (!empytQueueRB(&fila)) 
    {
        struct nodo aux = DequeueRB(&fila);
        if(aux.left != NULL) EnqueueRB(&fila, aux.left);
        if(aux.right != NULL) EnqueueRB(&fila, aux.right);
        printf("%d\n", aux.fd);
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

void _inorder(struct  nodo *no)
{
    if(no == NULL) return;
    if(no->left != NULL) _inorder(no->left);
    printf("%d ", no->fd);
    if(no->right != NULL) _inorder(no->right);
    return;
}

int inorder(RBtree *tree)
{
    if(tree == NULL) return 1;
    _inorder(tree->raiz);
    printf("\n");
    return 0;
}

void __preorder(struct nodo *no)
{
    if(no == NULL) return;
    printf("%d ", no->fd);
    if(no->left != NULL) __preorder(no->left);
    if(no->right != NULL) __preorder(no->right);
    return;
}

int preorder(RBtree *tree)
{
    if(tree == NULL) return 1;
    __preorder(tree->raiz);
    printf("\n");
    return 0;
}

int binarySearch(RBtree *tree, int value)
{
    if(value < 0) return -1;
    struct nodo *aux = tree->raiz;
    if(aux == NULL) return -1;
    while (aux != NULL) {
        if(aux->fd == value) return value;
        if(value > aux->fd) aux = aux->right;
        else aux = aux->left;
    }
    return -1;
    
}

int maxValue(RBtree *tree)
{
    if(tree == NULL) return -1;
    struct nodo *aux = tree->raiz;
    if(aux == NULL) return -1;
    while (aux != NULL) {
        if(aux->right != NULL) aux = aux->right;
        else break;
    }
    return aux->fd;
}

int minValue(RBtree *tree)
{
    if(tree == NULL) return -1;
    struct nodo *aux = tree->raiz;
    if(aux == NULL) return -1;
    while (aux != NULL) {
        if(aux->left != NULL) aux = aux->left;
        else break;
    }
    return aux->fd;
}
