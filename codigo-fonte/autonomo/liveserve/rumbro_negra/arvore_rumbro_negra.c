#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "arvore_rumbro_negra.h"
#include "Queuen_rumbro.h"

int newRBtree(RBtree *tree){
    tree->raiz = NULL;
    tree->size = 0;
    return 0;
}

struct nodo *grandpa(struct nodo *no)
{
    if((no != NULL) && ( no->dad != NULL)) return no->dad->dad;
    return NULL;
}

struct nodo *uncle(struct nodo *no)
{
    struct nodo *aux = grandpa(no);
    if(aux == NULL) return NULL;
    if(no->dad == aux->left) return aux->right;
    return aux->left;
}

void RDD(struct nodo *no)
{
}

void RDS(struct nodo *no)
{
}


void isBalance(struct nodo *raiz, struct nodo *no)
{
    struct nodo *temp = no;
    while(temp->dad != NULL && temp->dad->color == RUMBRO)
    {
        struct nodo *grandp = grandpa(temp);
        struct nodo *u = uncle(temp);
        if(u != NULL && u->color == RUMBRO){
            printf("filho: %d , pai: %d\n", no->fd, no->dad->fd);
        
        }else{
            printf("tio e preto\n");
        }
        temp = grandp; 
    }
    raiz->color = BLACK;
}

struct nodo *newNodo(int value){
    struct nodo *newno = NULL;
    newno = malloc(sizeof(struct nodo));
    if(newno != NULL) *newno = (struct nodo){.color=RUMBRO, .fd=value, .left=NULL, .right=NULL, .dad=NULL};
    return newno;
 }

struct nodo *__binarySearch(struct nodo *aux,int value)
{
    if(aux == NULL) return NULL;
    while (aux != NULL) {
        if(aux->fd == value) return aux;
        if(value > aux->fd) aux = aux->right;
        else aux = aux->left;
    }
    return NULL;
}

int insertRBtree(RBtree *tree, int value){
    if(tree == NULL) return 1;
    struct nodo *newno = newNodo(value);
    if(newno == NULL) return 1;
    struct nodo *temp = tree->raiz;
    struct nodo *dad = NULL;
    int old = tree->size;
    while( temp != NULL)
    {
        if(newno->fd ==  temp->fd){
            free(newno);
            return 1;
        }
        dad = temp;
        if(newno->fd < temp->fd)
        {
            temp = temp->left;
            newno->dad = dad;
            continue;
        }
        temp = temp->right;
        newno->dad = dad;
    }
    tree->size++;
    if(dad == NULL)
    {
        tree->raiz = newno;
    }else if(newno->fd < dad->fd)
    {
        dad->left = newno;
    }else{
        dad->right = newno;
    }
    isBalance(tree->raiz, newno);
    return (old == tree->size);
}

int* porlevel(RBtree *tree){
    if(tree == NULL) return NULL;
    int *resultado = malloc(sizeof(int) * tree->size);
    QueuenRB fila;
    newQueueRB(&fila);
    EnqueueRB(&fila, tree->raiz);
    int i = 0;
    while (!empytQueueRB(&fila)) 
    {
        struct nodo aux = DequeueRB(&fila);
        if(aux.left != NULL) EnqueueRB(&fila, aux.left);
        if(aux.right != NULL) EnqueueRB(&fila, aux.right);
        if(aux.fd > 0){
            resultado[i] = aux.fd;
            i++;
        } 
    }
    freeQueuenRB(&fila);
    return resultado;
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

void _inorder(struct  nodo *no, int *i, int *resultado)
{
    if(no == NULL) return;
    if(no->left != NULL) _inorder(no->left, i, resultado);
    resultado[*i] = no->fd;
    *i = *i + 1;
    if(no->right != NULL) _inorder(no->right, i, resultado);
    return;
}

int* inorder(RBtree *tree)
{
    if(tree == NULL) return NULL;
    int *resultado = malloc(sizeof(int) * tree->size);
    if(resultado == NULL) return NULL;
    int i = 0;
    _inorder(tree->raiz, &i, resultado);
    return resultado;
}

void __preorder(struct nodo *no, int *i, int *resultado)
{
    if(no == NULL) return;
    resultado[*i] = no->fd;
    *i = *i + 1;
    if(no->left != NULL) __preorder(no->left, i, resultado);
    if(no->right != NULL) __preorder(no->right, i, resultado);
    return;
}

int* preorder(RBtree *tree)
{
    if(tree == NULL) return NULL;
    int *resultado = malloc(sizeof(int) * tree->size);
    if(resultado == NULL) return NULL;
    int i = 0;
    __preorder(tree->raiz, &i, resultado);
    return resultado;
}

int binarySearch(RBtree *tree, int value)
{
    if(value < 0) return -1;
    struct nodo *aux = __binarySearch(tree->raiz, value);
    if(aux == NULL) return -1;
    return aux->fd;
    
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
