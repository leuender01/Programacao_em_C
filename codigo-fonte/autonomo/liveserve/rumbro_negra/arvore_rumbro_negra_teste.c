#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "arvore_rumbro_negra.h"
#include "libtesetes/testes.h"
#define MAXTESTE 12

int values_teste[MAXTESTE] = {0};

void inoder_funcition(struct  nodo *no)
{
    if(no == NULL) return;
    if(no->left != NULL) inoder_funcition(no->left);
    printf("pai: %d, filho: %d, COLOR:%s \n", (no->dad != NULL) ? no->dad->fd : -1, no->fd , (no->color == BLACK) ? "BLACK" : "RUMBRO");
    if(no->right != NULL) inoder_funcition(no->right);
    return;
}

void listarpai(RBtree *tree)
{
    if(tree == NULL) return;
    int *resultado = malloc(sizeof(int) * tree->size);
    if(resultado == NULL) return ;
    inoder_funcition(tree->raiz);
    return;
}

int* testas_recurçao(int *func(RBtree *arv), RBtree* arv){
    puts("\033[1;33mTESTANDO A RECURÇÃO ...\033[0m");
    int *arrayResultado = NULL;
    arrayResultado =  func(arv);
    if(arrayResultado == NULL){
        perror("\033[1;31m FUNCAO RETORNOU NULL\033[0m");
        printf("%d\n",freeRB(arv));
        exit(1);
    }
    puts("\033[1;32mPASSOU\033[0m");
    return arrayResultado;
}

int orderLista(int *func(RBtree *arv), RBtree *arv)
{
    if(arv->size < 2) return 0;
    int testes = 0;
    int *arrayResultado = testas_recurçao(func, arv);
    if(arrayResultado == NULL)
    {
        perror("\033[1;31m FUNCAO RETORNOU NULL\033[0m");
        exit(1);
    }
    for (int i = 0; i < (arv->size - 1); i++) 
    {
        testes = (arrayResultado[i] < arrayResultado[i + 1]);
    }
    printf("\n");
    free(arrayResultado);
    
    return !testes;
}

int compararArvore(RBtree *arv)
{
    RBtree treeTeste;
    newRBtree(&treeTeste);
    int *resultadoTeste = NULL;
    int *resultado = preorder(arv);
    if(resultado == NULL)
    {
        perror("\033[1;31m FUNCAO RETORNOU NULL\033[0m");
        printf("%d\n",freeRB(arv));
        exit(1);
    }
    for (int i = 0; i < arv->size ; i++) 
    {
        insertRBtree(&treeTeste, resultado[i]);
    }
    resultadoTeste = preorder(&treeTeste);
    if(resultadoTeste == NULL)
    {
        perror("\033[1;31m FUNCAO RETORNOU NULL\033[0m");
        printf("%d\n",freeRB(arv));
        printf("%d\n",freeRB(&treeTeste));
        free(resultado);
        exit(1);
    }
    if(arv->size != treeTeste.size){
        printf("\033[1;31m ARVORES COM TAMANHOS DIFERENTES [%lu/%lu]\033[0m\n", treeTeste.size, arv->size);
        printf("%d\n",freeRB(arv));
        printf("%d\n",freeRB(&treeTeste));
        free(resultadoTeste);
        free(resultado);
        exit(1);
    }
    int data = 0;
    for(int i = 0; i < arv->size; i++){
        if(resultado[i] != resultadoTeste[i]) data = 1;
//        printf("%d : %d\n", resultado[i], resultadoTeste[i]);
    }
    freeRB(&treeTeste);
    free(resultadoTeste);
    free(resultado);
    return data;
}

int main(void){
    RBtree arv;
    int* resultado = NULL;
    srand(time(NULL));
    puts("\033[1;33mTESTANDO CRIAÇÃO...\033[0m");
    if(arv.size != 0 || arv.raiz != NULL )
    {
        puts("\033[1;31m VALORES INCORRETOS\033[0m");
        exit(1);

    }
    printf("%d\n",maxValue(&arv));
    printf("%d\n",minValue(&arv));

    puts("\033[1;33mTESTANDO INSEÇÃO...\033[0m");
    int inseridos = 0;
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        if((insertRBtree(&arv, values_teste[i])) == 0) inseridos++;
    }
    if(arv.size != inseridos){
        perror("VALORES INSERIDO DIFERENTE DO TAMANHO DA LISTA TAMANHO");
        exit(1);
    }
    printf("[%d/%lu] \033[1;32m RESULTADO\033[0m\n", inseridos, arv.size);
    puts("\033[1;32mPASSOU\033[0m");

    puts("\033[1;33mTESTANDO PERCURSO INORDER...\033[0m");
    if((orderLista(inorder,  &arv)) != 0)
    {
        perror("\033[1;31m NAO ESTA ORDENADA\033[0m");
        freeRB(&arv);
        exit(1);
    }
    puts("\033[1;32mPASSOU\033[0m");

    puts("\033[1;33mTESTANDO PERCURSO PREORDER...\033[0m");
    compararArvore(&arv);   
    free(resultado);
    resultado = NULL;
    puts("\033[1;32mPASSOU\033[0m");

    puts("\033[1;33mTESTANDO PERCURSO PORLEVEL...\033[0m");
    resultado =  testas_recurçao(porlevel, &arv);
    puts("\033[1;32mPASSOU\033[0m");

    int acertos = 0;
    printf("\033[1;32mTESTE DE BUSCA\033[0m\n");
    for (int i = 0; i < MAXTESTE; i++) {
        if(binarySearch(&arv, values_teste[i]) > 0) acertos++;
    }
    printf("[%d/%d] \033[1;32m RESULTADO\033[0m\n", acertos, MAXTESTE);
    puts("\033[1;32mPASSOU\033[0m");
    printf("%d\n",maxValue(&arv));
    printf("%d\n",minValue(&arv));
    listarpai(&arv);
    for (int i = 0; i < arv.size; i++) {
        printf("%d ,", resultado[i]);
    }
    free(resultado);
    resultado = NULL;

    printf("%d\n",freeRB(&arv));
    return 0;
}
