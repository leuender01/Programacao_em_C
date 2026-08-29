#include <stdlib.h>
#include "arvore_rumbro_negra.h"
#include "Queuen_rumbro.h"

int newRBtree(RBtree *tree){
    tree->raiz = NULL;
    tree->size = 0;
    return 0;
}

/*
    ## Regras:
    - Todo no e rumbro ou negrod
    - a raiz e sempre negra
    - todo no nulo e negro
    - o pai de um no rumbro e sempre negro
    - Qualquer caminho de um no ate um no nulo tem o mesmo numero de nos negros
    - cada novo no inserido e RUMBRO
    - insercao e igual a um ABB
    - Apos a insercao ver se as propridades da rumbro negra se mantem
    - Se o pai do novo no for negro todas as propridades se mantem
    - Se o pai do novo no for rumbro rotações alterações precisam se feitas

    ## Caso 1
    ### Seo pai e o tio do novo no sao Rumbros 
    - Pai e tio ficam negros
    - Avo fica Rumbro
    ### Se o pai do avo for Rumbro, inicia novamente o processo de verificacao das cores

    ```OBS
        atualize o novo no para o avô
    ```
    - O pai e o tio ficam Negros
    - Vo dica Rubro
    - novo no = avô

    ## Caso 2
    ### O pai e rubro e o tio e negro
    
    #### __Rotaçao Simples__direita
    ----Pai do novo no e filho esquerdo e o novo no e filho esquerdo-----
    - Pai fica negro 
    - Avo fica Rubro
    - Rotaciona o avo para direita

    ### __Rotaçao Dupla__direira_esquerda
    ----Pai do novo no e filho esquerdo e o novo no e filho direito-----
    - Rotaciona Pai para a esquerda
    - o novo no = filho esquerdo do novo no
    - Pai fica negro
    - Avo fica Rubro e Rotaciona o avo para a direita

 */
void RDD(struct nodo *no)
{
    struct nodo *swap = no->left;
    no->right = NULL;
    swap->right = no;
    no  = swap;
    
}

void RDS(struct nodo *no)
{
    struct nodo *swap = no->right;
    no->left = NULL;
    swap->left = no;
    no  = swap;
}

void isBalance(struct nodo *no)
{
    struct nodo *atualNo = no;
    while (atualNo->color == RUMBRO && atualNo->dad != NULL && atualNo->dad->color == RUMBRO) {
        struct nodo *grandp = atualNo->dad->dad;
        struct nodo *dad = atualNo->dad;
        if(grandp->left == atualNo)
        {
            struct nodo *uncle = grandp->right;
            if(uncle != NULL && uncle->color == RUMBRO)
            {
                uncle->color = BLACK; 
                dad->color =  uncle->color;
                grandp->color = RUMBRO;
                atualNo = grandp;
            }else{
                if(grandp->right == dad)
                {
                    RDS(dad);
                }
                RDD(grandp);
                grandp->color = RUMBRO;
                atualNo->dad->color = BLACK;
                atualNo = atualNo->dad;

            }
        
        }else{
            struct nodo *uncle = grandp->left;
            if(uncle != NULL && uncle->color == RUMBRO)
            {
                uncle->color = BLACK; 
                dad->color =  uncle->color;
                grandp->color = RUMBRO;
                atualNo = grandp;
            }else{
                if(grandp->left == dad)
                {
                    RDD(dad);
                }
                RDS(grandp);
                grandp->color = RUMBRO;
                atualNo->dad->color = BLACK;
                atualNo = atualNo->dad;
            }
        }
    
    }
    no->color = BLACK;
}

struct nodo *newNodo(int value, struct nodo *dad, COLOR cor){
    struct nodo *newno = NULL;
    newno = malloc(sizeof(struct nodo));
    if(newno != NULL) *newno = (struct nodo){.color=cor, .fd=value, .left=NULL, .right=NULL, .dad=dad};
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

struct nodo *__insertRB(struct nodo *temp, int value, RBtree *tree, struct nodo *dad, COLOR cor){
    if(temp != NULL && temp->fd == value) return temp;
    if(temp == NULL){
        temp = newNodo(value, dad, cor);
        isBalance(temp);
        tree->size++;
    }else if(value > temp->fd){
        temp->right = __insertRB(temp->right, value, tree, temp, RUMBRO);
        isBalance(temp);
    }else{
        temp->left = __insertRB(temp->left, value, tree, temp, RUMBRO);
        isBalance(temp);
    }
    return temp;
}

int insertRBtree(RBtree *tree, int value){
    if(tree == NULL) return 1;
    struct nodo *temp = tree->raiz;
    if(temp == NULL){
        temp = newNodo(value, NULL, BLACK);
        if(temp == NULL) return 1;
        tree->raiz = temp;
        tree->size++;
        return 0;
    }
    unsigned long old = tree->size;
    tree->raiz = __insertRB(temp, value, tree, NULL, RUMBRO);
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
