#include <stdlib.h>
#include <sys/types.h>
#include "arvore_rumbro_negra.h"
#include "Queuen_rumbro.h"

int newRBtree(RBtree *tree){
    tree->raiz = NULL;
    tree->size = 0;
    return 0;
}

static struct nodo *grandpa(struct nodo *no)
{
    if((no != NULL) && ( no->dad != NULL)) return no->dad->dad;
    return NULL;
}

static struct nodo *uncle(struct nodo *no)
{
    struct nodo *aux = grandpa(no);
    if(aux == NULL) return NULL;
    if(no->dad == aux->left) return aux->right;
    return aux->left;
}

static void RDD(RBtree *raiz , struct nodo *no)
{
    if (no == NULL || no->left == NULL) return;
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

static void RDS(RBtree *raiz , struct nodo *no)
{
    if (no == NULL || no->right == NULL) return;
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


static void isBalance(RBtree *raiz, struct nodo *no)
{
    struct nodo *temp = no;
    while(temp->dad != NULL && temp->dad->color == RUMBRO)
    {
        struct nodo *grandp = grandpa(temp);
        struct nodo *dad = temp->dad;
        if(grandp != NULL && grandp->right == dad)
        {
            struct nodo *u = uncle(temp);
            if(u != NULL && u->color == RUMBRO) 
            {
                u->color = BLACK;
                dad->color = BLACK;
                grandp->color = RUMBRO;
                temp = grandp;
            }else {
                if(dad->left == temp)
                {
                    temp = dad;
                    RDD(raiz, temp);
                    dad = temp->dad;
                    grandp = grandpa(temp);
                }
                dad->color = BLACK;
                grandp->color = RUMBRO;
                RDS(raiz, grandp);
            }
        }else if(grandp != NULL){
            struct nodo *u = uncle(temp);
            if(u != NULL &&  u->color == RUMBRO) 
            {
                u->color = BLACK;
                dad->color = BLACK;
                grandp->color = RUMBRO;
                temp = grandp;
            }else {
                if( dad->right == temp)
                {
                    temp = dad;
                    RDS(raiz, temp);
                    dad = temp->dad;
                    grandp = grandpa(temp);
                }
                dad->color = BLACK;
                grandp->color = RUMBRO;
                RDD(raiz, grandp);
            }
        }
        if(temp == raiz->raiz) break;
    }
    raiz->raiz->color = BLACK;
}

static struct nodo *newNodo(int value){
    struct nodo *newno = NULL;
    newno = malloc(sizeof(struct nodo));
    if(newno != NULL) *newno = (struct nodo){.color=RUMBRO, .key=value, .left=NULL, .right=NULL, .dad=NULL};
    return newno;
 }

static struct nodo *__binarySearch(struct nodo *aux,int value)
{
    if(aux == NULL) return NULL;
    while (aux != NULL) {
        if(aux->key == value) return aux;
        if(value > aux->key) aux = aux->right;
        else aux = aux->left;
    }
    return NULL;
}

int insertRBtree(RBtree *tree, int key){
    if(tree == NULL) return 1;
    struct nodo *newno = newNodo(key);
    if(newno == NULL) return 1;
    struct nodo *temp = tree->raiz;
    struct nodo *dad = NULL;
    int old = tree->size;
    while( temp != NULL)
    {
        if(newno->key ==  temp->key){
            free(newno);
            return 1;
        }
        dad = temp;
        if(newno->key < temp->key)
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
    }else if(newno->key < dad->key)
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
        if(aux.key > 0){
            resultado[i] = aux.key;
            i++;
        } 
    }
    freeQueuenRB(&fila);
    return resultado;
}

static void _freenode(struct nodo *no){
    if(no == NULL) return;
    if(no->left != NULL) _freenode(no->left);
    if(no->right != NULL) _freenode(no->right);
    free(no);
    return;
}

int freeRB(RBtree *tree){
    if(tree == NULL) return 1;
    struct nodo *raiz = tree->raiz;
    tree->size = 0;
    _freenode(raiz);
    return 0;
}

static void _inorder(struct  nodo *no, int *i, int *resultado)
{
    if(no == NULL) return;
    if(no->left != NULL) _inorder(no->left, i, resultado);
    resultado[*i] = no->key;
    *i = *i + 1;
    if(no->right != NULL) _inorder(no->right, i, resultado);
    return;
}

int* inorder(RBtree *tree)
{
    int *resultado = NULL;
    if(tree == NULL) return NULL;
    if(tree->size > 0)resultado = malloc(sizeof(int) * tree->size);
    if(resultado == NULL) return NULL;
    int i = 0;
    _inorder(tree->raiz, &i, resultado);
    return resultado;
}

static void __preorder(struct nodo *no, int *i, int *resultado)
{
    if(no == NULL) return;
    resultado[*i] = no->key;
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

int binarySearch(RBtree *tree, int key)
{
    if(key < 0) return -1;
    struct nodo *aux = __binarySearch(tree->raiz, key);
    if(aux == NULL) return -1;
    return aux->key;
    
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
    return aux->key;
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
    return aux->key;
}

static struct nodo *_minValue(struct nodo *tree)
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

static struct nodo NIL_NODE = {.color = BLACK, .key = -1, .left = NULL, .right = NULL, .dad = NULL};
#define NIL (&NIL_NODE)

static char colorOf(struct nodo *n)
{
    return (n == NULL) ? BLACK : n->color;
}

static void remove_fixup(RBtree *arv, struct nodo *x)
{
    while (x != arv->raiz && colorOf(x) == BLACK) {
        struct nodo *dad = x->dad;
        if (x == dad->left) {
            struct nodo *brother = dad->right;
 
            if (colorOf(brother) == RUMBRO) {
                brother->color = BLACK;
                dad->color = RUMBRO;
                RDS(arv, dad);
                brother = dad->right;
            }
 
            if (colorOf(brother->left) == BLACK && colorOf(brother->right) == BLACK) {
                brother->color = RUMBRO;
                x = dad;
            } else {
                if (colorOf(brother->right) == BLACK) {
                    if (brother->left != NULL) brother->left->color = BLACK;
                    brother->color = RUMBRO;
                    RDD(arv, brother);
                    brother = dad->right;
                }
                brother->color = dad->color;
                dad->color = BLACK;
                if (brother->right != NULL) brother->right->color = BLACK;
                RDS(arv, dad);
                x = arv->raiz;
            }
        } else {
            struct nodo *brother = dad->left;
 
            if (colorOf(brother) == RUMBRO) {
                brother->color = BLACK;
                dad->color = RUMBRO;
                RDD(arv, dad);
                brother = dad->left;
            }
 
            if (colorOf(brother->right) == BLACK && colorOf(brother->left) == BLACK) {
                brother->color = RUMBRO;
                x = dad;
            } else {
                if (colorOf(brother->left) == BLACK) {
                    if (brother->right != NULL) brother->right->color = BLACK;
                    brother->color = RUMBRO;
                    RDS(arv, brother);
                    brother = dad->left;
                }
                brother->color = dad->color;
                dad->color = BLACK;
                if (brother->left != NULL) brother->left->color = BLACK;
                RDD(arv, dad);
                x = arv->raiz;
            }
        }
    }
    x->color = BLACK;
}
static void _remove(RBtree *raiz, struct nodo *no, int key)
{
    struct nodo *temp = no;
    while (temp != NULL) {
        if(key == temp->key) break;
        else if(key < temp->key) temp = temp->left;
        else temp = temp->right;
        
    }
    if(temp == NULL) return;
    if(temp->left != NULL && temp->right != NULL)
    {
        struct nodo *sucessor = _minValue(temp->right);
        temp->key = sucessor->key;
        temp = sucessor;
    }
    char oldcolor = temp->color;
    struct nodo *filho = (temp->left != NULL) ?  temp->left : temp->right;
    int userNil=  0;
    if(filho == NULL)
    {
        filho = NIL;
        userNil = 1;
    }

    filho->dad = temp->dad;
    
    if (temp->dad == NULL) {
        raiz->raiz = filho;
    } else if (temp == temp->dad->left) {
        temp->dad->left = filho;
    } else {
        temp->dad->right = filho;
    }
    
    if(oldcolor == BLACK)
    {
        remove_fixup(raiz, filho);
    }

    if(userNil){
         if (filho->dad == NULL) {
            raiz->raiz = NULL;
        } else if (filho->dad->left == filho) {
            filho->dad->left = NULL;
        } else if (filho->dad->right == filho) {
            filho->dad->right = NULL;
        }

        NIL->dad = NULL;
        NIL->left = NULL;
        NIL->right = NULL;
        NIL->color = BLACK;
    }
    free(temp);
    raiz->size--;
}

 int removeRbtree(RBtree *tree, int key)
{
    if(tree->raiz == NULL) return 0;
    int old = tree->size;
    _remove(tree, tree->raiz, key);
    return (old != tree->size);
}
