#include <stdio.h>
#include "arvore_rumbro_negra.h"
#include "libtesetes/testes.h"
#define MAXTESTE 100

int values_teste[MAXTESTE] = {0};
int main(void){
    RBtree arv;
    printf("%d\n",newRBtree(&arv));
    printf("%d\n",maxValue(&arv));
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i]);
    }
    printf("%d\n",inorder(&arv));
    printf("%d\n",preorder(&arv));
    printf("%d\n",porlevel(&arv));
    for (int i = 0; i < MAXTESTE; i++) {
        printf("resultado %d, valor priocurado %d\n", binarySearch(&arv, values_teste[i]), values_teste[i]);
    }
    printf("%d\n",maxValue(&arv));
    printf("%d\n",minValue(&arv));
    printf("%d\n",freeRB(&arv));
    return 0;
}

