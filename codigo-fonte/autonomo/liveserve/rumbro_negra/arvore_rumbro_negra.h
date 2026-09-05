#ifndef ARVORE_RUMBRO
#define ARVORE_RUMBRO

/*
typedef  enum {
    BLACK,
    RUMBRO
} COLOR;
*/
#define BLACK 'B'
#define RUMBRO 'R'

#pragma pack(1)
struct nodo{
    char color;
    int key;
    struct nodo *left, *right, *dad;
};
#pragma pack()

typedef struct{
    struct nodo *raiz;
    unsigned long size;
} RBtree;

int newRBtree(RBtree *tree);
int insertRBtree(RBtree *tree, int key);
int freeRB(RBtree *tree);
int* porlevel(RBtree *tree);
int* inorder(RBtree *tree);
int* preorder(RBtree *tree);
int binarySearch(RBtree *tree, int key);
int maxValue(RBtree *tree);
int minValue(RBtree *tree);
int removeRbtree(RBtree *tree, int key);
#endif
