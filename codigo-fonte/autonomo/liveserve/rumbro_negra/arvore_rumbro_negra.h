#ifndef ARVORE_RUMBRO
#define ARVORE_RUMBRO
typedef  enum {
    BLACK,
    RUMBRO
} COLOR;

struct nodo{
    COLOR color;
    int fd;
    struct nodo *left, *right, *dad;
};

typedef struct{
    struct nodo *raiz;
    unsigned long size;
} RBtree;

int newRBtree(RBtree *tree);
int insertRBtree(RBtree *tree, int value);
int freeRB(RBtree *tree);
int* porlevel(RBtree *tree);
int* inorder(RBtree *tree);
int* preorder(RBtree *tree);
int binarySearch(RBtree *tree, int value);
int maxValue(RBtree *tree);
int minValue(RBtree *tree);

#endif
