#ifndef ARVORE_RUMBRO
#define ARVORE_RUMBRO
typedef  enum {
    BLACK,
    RUMBRO
} COLOR;

struct nodo{
    COLOR color;
    int fd;
    struct nodo *left;
    struct nodo *right;
};

typedef struct{
    struct nodo *raiz;
    struct nodo *Nill;
    unsigned long size;
} RBtree;

int newRBtree(RBtree *tree);
struct nodo *newNodo(int value, struct nodo *nodeFolha);
int insertRBtree(RBtree *tree, int value);
int freeRB(RBtree *tree);
#endif
