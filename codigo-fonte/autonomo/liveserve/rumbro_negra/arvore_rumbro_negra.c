//#include <stdio.h>
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

void RDD(RBtree *raiz , struct nodo *no)
{
    /*
        y = x.left
        x.left = y.right
        if y.right != self.NIL:
            y.right.parent = x
        y.parent = x.parent
        if x.parent is None:
            self.root = y
        elif x == x.parent.right:
            x.parent.right = y
        else:
            x.parent.left = y
        y.right = x
        x.parent = y
        */
    if (no == NULL || no->left == NULL) return;
//    puts("Rotacionar para direita\n");
//    if(no != NULL ) printf("no atual %d , pai: %d\n",(no != NULL) ?  no->fd : -1, (no->dad != NULL) ?  no->dad->fd : -1);
    struct nodo *swap = no->left;
    no->left = swap->right;

    if (swap->right != NULL) swap->right->dad = no;
    swap->dad = no->dad;
    if (no->dad == NULL) raiz->raiz = swap; 
    else if (no == no->dad->right) no->dad->right = swap;
    else no->dad->left = swap;

    swap->right = no;
    no->dad = swap;
}

void RDS(RBtree *raiz , struct nodo *no)
{
    /*
         y = x.right
        x.right = y.left
        if y.left != self.NIL:
            y.left.parent = x
        y.parent = x.parent
        if x.parent is None:
            self.root = y
        elif x == x.parent.left:
            x.parent.left = y
        else:
            x.parent.right = y
        y.left = x
        x.parent = y
    */
    if (no == NULL || no->right == NULL) return;
//    puts("Rotacionar para esquerda\n");
//    if(no != NULL ) printf("no atual %d , pai: %d\n",(no != NULL) ?  no->fd : -1, (no->dad != NULL) ?  no->dad->fd : -1);
    struct nodo *swap = no->right;
    no->right = swap->left;
    if(swap->left != NULL) swap->left->dad = no;
    swap->dad = no->dad;
    if(no->dad == NULL) raiz->raiz = swap;
    else if( no == no->dad->left) no->dad->left = swap;
    else no->dad->right = swap;
    swap->left = no;
    no->dad = swap;
}


void isBalance(RBtree *raiz, struct nodo *no)
{
    struct nodo *temp = no;
    while(temp->dad != NULL && temp->dad->color == RUMBRO)
    {
        struct nodo *grandp = grandpa(temp);
        struct nodo *dad = temp->dad;
        if(grandp != NULL && grandp->right == dad)
        {
//            puts("O pai e filho direito");
            struct nodo *u = uncle(temp);
            if(u != NULL && u->color == RUMBRO) 
            {
                u->color = BLACK;
                dad->color = BLACK;
                grandp->color = RUMBRO;
                temp = grandp;
            }else {
//                puts("Precisa de rotaco ");
                if(dad->left == temp)
                {
//                    printf("noAtual e filho esquerdo no atual: %d, pai: %d\n", temp->fd, dad->fd);
                    temp = dad;
                    RDD(raiz, temp);
                    dad = temp->dad;
                    grandp = grandpa(temp);
                }
                dad->color = BLACK;
                grandp->color = RUMBRO;
                RDS(raiz, grandp);
            }
            /*
            if(dad != NULL && dad->left == temp)
            {
                puts("no atual e filho esquerdo");
            }else if(dad != NULL && dad->right == temp){
                puts("no atual e filho direito");
            }
            */
        }else if(grandp != NULL){
//            puts("O pai e filho esquerdo");
            struct nodo *u = uncle(temp);
            if(u != NULL &&  u->color == RUMBRO) 
            {
                u->color = BLACK;
                dad->color = BLACK;
                grandp->color = RUMBRO;
                temp = grandp;
            }else {
//                puts("Precisa de rotaco ");
                if( dad->right == temp)
                {
//                    puts("noAtual e filho direito");
                    temp = dad;
                    RDS(raiz, temp);
                    dad = temp->dad;
                    grandp = grandpa(temp);
                }
                dad->color = BLACK;
                grandp->color = RUMBRO;
                RDD(raiz, grandp);
            }
            /*
            if(dad != NULL && dad->left == temp)
            {
                puts("no atual e filho esquerdo");
                RDD(no->dad);
            }else if(dad != NULL && dad->right == temp){
                puts("no atual e filho direito");
            }
            */
        }
        if(temp == raiz->raiz) break;
    }
    raiz->raiz->color = BLACK;
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
    isBalance(tree, newno);
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

struct nodo *_minValue(struct nodo *tree)
{
    if(tree == NULL) return NULL;
    struct nodo *aux = tree;
    if(aux == NULL) return NULL;
    while (aux != NULL) {
        if(aux->left != NULL) aux = aux->left;
        else break;
    }
    return aux;
}

struct nodo *_remove(RBtree *raiz, struct nodo *no, int value)
{
    if(no == NULL) return NULL;
    if(value < no->fd)
    {
        no->left = _remove(raiz, no->left, value);
        if(no->left != NULL) no->left->dad = no;
    }else if(value > no->fd)
    {
        no->right = _remove(raiz, no->right, value);
        if(no->right != NULL) no->right->dad = no;
    }else{
        if(no->left == NULL || no->right == NULL)
        {
            struct nodo *filho = (no->left != NULL) ? no->left : no->right;  
            if(filho != NULL) filho->dad = no->dad;
            free(no);
            raiz->size--;
            return filho;
        }else{
            struct nodo *aux = _minValue(no->right);
            no->fd = aux->fd;
            no->right = _remove(raiz, no->right, aux->fd);
            if(no->right != NULL) no->right->dad = no;
        }
    }
    return no;
}

 int removeRbtree(RBtree *tree, int value)
{
    if(tree->raiz == NULL) return 0;
    int old = tree->size;
    tree->raiz = _remove(tree, tree->raiz, value);
    return (old != tree->size);
}
