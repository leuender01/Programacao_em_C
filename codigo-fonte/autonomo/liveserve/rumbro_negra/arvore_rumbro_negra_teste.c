#include <stdio.h>
#include "../arvore_rumbro_negra.h"

int main(void){
    RBtree arv;
    printf("%d\n",newRBtree(&arv));
    printf("%d\n",insertRBtree(&arv, 1));
    printf("%d\n",porlevel(&arv));
    printf("%d\n",freeRB(&arv));
    return 0;
}

