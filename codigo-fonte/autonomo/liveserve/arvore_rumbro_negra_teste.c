#include "arvore_rumbro_negra.h"

int main(void){
    RBtree arv;
    newRBtree(&arv);
    insertRBtree(&arv, 1);
    freeRB(&arv);
}

