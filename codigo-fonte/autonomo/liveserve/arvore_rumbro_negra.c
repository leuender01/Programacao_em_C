#include <stdlib.h>
#include "arvore_rumbro_negra.h"

int newRBtree(RBtree *tree){
    tree->Nill = NULL;
    tree->raiz = tree->Nill;
    tree->size = 0;
    return 0;
}

struct nodo *newNodo(int value, struct nodo *nodeFolha){
    struct nodo *newno = NULL;
    newno = malloc(sizeof(struct nodo));
    if(newno != NULL) *newno = (struct nodo){.color=BLACK, .fd=value, .left=nodeFolha, .right=nodeFolha};
    return newno;
 }

int insertRBtree(RBtree *tree, int value){
    if(tree == NULL) return 1;
    struct nodo *temp = tree->raiz;
    if(temp == NULL){
        temp = newNodo(value, tree->Nill);
        if(temp == NULL) return 1;
        tree->raiz = temp;
    }
    tree->size++;
    return 0;
}

void _freenode(struct nodo *no){
    if(no == NULL) return;
    if(no->left != NULL) _freenode(no->left);
    if(no->right != NULL) _freenode(no->left);
    free(no);
    return;
}

int freeRB(RBtree *tree){
    if(tree == NULL) return 1;
    struct nodo *raiz = tree->raiz;
    _freenode(raiz);
    return 0;
}

