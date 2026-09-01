#ifndef ARVORE_RUMBRO
#define ARVORE_RUMBRO

typedef  enum {
    BLACK,
    RUMBRO
} COLOR;

struct nodo{
    COLOR color;
    int key;
    void *info;
    struct nodo *left, *right, *dad;
};

typedef struct{
    struct nodo *raiz;
    unsigned long size;
} RBtree;

int newRBtree(RBtree *tree);
int insertRBtree(RBtree *tree, int key, void *info);
int freeRB(RBtree *tree);
int* porlevel(RBtree *tree);
int* inorder(RBtree *tree);
int* preorder(RBtree *tree);
void* binarySearch(RBtree *tree, int key);
int maxValue(RBtree *tree);
int minValue(RBtree *tree);
int removeRbtree(RBtree *tree, int key);
#endif
