#include <stdio.h>
#include <stdlib.h>

struct aresta{
    struct aresta *prox;    
    struct no *pointer;
};

struct no{
    int id;
    int value;
    struct aresta *prox;
};

typedef struct no* NO;
typedef struct aresta ARESTA;

NO init(int id, int value){
    NO pai = malloc(sizeof(struct no));
    pai->prox = NULL;
    pai->id = id;
    pai->value = value;
    return pai;
};

void conect(NO v1,NO v2){
    if(v1 == NULL || v2 == NULL) return;
    ARESTA *no_prox = malloc(sizeof(ARESTA));
    no_prox->pointer = v2;
    no_prox->prox = v1->prox;
    v1->prox = no_prox;
};

void printgrafo(NO v1){
   ARESTA *aresta = v1->prox;
   while (aresta != NULL)
   {
    printf("%d\n",aresta->pointer->id);
    aresta = aresta->prox;
   };
};

int main(void){
    NO v1, v2, v3,v4,v5;
    v1 = init(1,20);
    v2 = init(2,30);
    v3 = init(60,40);
    v4 = init(3,100);
    v5 = init(5,30);
    
    conect(v1,v2);
    conect(v2,v3);
    conect(v1,v4);
    conect(v1,v5);
    printgrafo(v2);
    printf("%d : %d",v1->prox->pointer->id,v3->id);
    //printf("%d, %d, %p, %p\n", v1->id,v1->value,v1->prox->pointer,v1);
    //printf("%d, %d, %p, %p\n", v2->id,v2->value,v2->prox->pointer,v2);
    //printf("%d, %d, %p, %p\n", v3->id,v3->value,,v3);
    
    free(v1);
    free(v2);
    free(v3);

    return 0;
};