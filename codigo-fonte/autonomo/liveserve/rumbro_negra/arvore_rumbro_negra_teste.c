#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "arvore_rumbro_negra.h"
#include "libtesetes/testes.h"
#define MAXTESTE 10

int values_teste[MAXTESTE] = {0};

void inoder_funcition(struct  nodo *no)
{
    if(no == NULL) return;
    printf("pai: %d, filho: %d, COLOR:%s \n", (no->dad != NULL) ? no->dad->fd : -1, no->fd , (no->color == BLACK) ? "BLACK" : "RUMBRO");
    if(no->left != NULL) inoder_funcition(no->left);
    if(no->right != NULL) inoder_funcition(no->right);
    return;
}

void listarpai(RBtree *tree)
{
    if(tree == NULL) return;
//    int *resultado = malloc(sizeof(int) * tree->size);
//    if(resultado == NULL) return ;
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
    }
    freeRB(&treeTeste);
    free(resultadoTeste);
    free(resultado);
    return data;
}

void testar_criacao(RBtree *arv)
{
    puts("\033[1;33mTESTANDO CRIAÇÃO...\033[0m");
    if(arv->size != 0 || arv->raiz != NULL )
    {
        puts("\033[1;31m VALORES INCORRETOS\033[0m");
        exit(1);

    }
    puts("\033[1;32mPASSOU\033[0m");
}

void testando_insercao(RBtree *arv)
{
    puts("\033[1;33mTESTANDO INSEÇÃO...\033[0m");
    int inseridos = 0;
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        if((insertRBtree(arv, values_teste[i])) == 0) inseridos++;
    }
    if(arv->size != inseridos){
        perror("VALORES INSERIDO DIFERENTE DO TAMANHO DA LISTA TAMANHO");
        exit(1);
    }
    printf("\033[1;32mRESULTADO: [%d/%lu]  \033[0m\n", inseridos, arv->size);
    puts("\033[1;32mPASSOU\033[0m");
}

void testando_percursos(RBtree *arv)
{
    puts("\033[1;33mTESTANDO PERCURSO INORDER...\033[0m");
    int* resultado = NULL;
    if((orderLista(inorder,  arv)) != 0)
    {
        perror("\033[1;31m NAO ESTA ORDENADA\033[0m");
        freeRB(arv);
        exit(1);
    }
    puts("\033[1;32mPASSOU\033[0m");

    puts("\033[1;33mTESTANDO PERCURSO PREORDER...\033[0m");
    compararArvore(arv);   
    free(resultado);
    resultado = NULL;
    puts("\033[1;32mPASSOU\033[0m");

    puts("\033[1;33mTESTANDO PERCURSO PORLEVEL...\033[0m");
    resultado =  testas_recurçao(porlevel, arv);
    free(resultado);
    resultado = NULL;
    puts("\033[1;32mPASSOU\033[0m");

    int acertos = 0;
    printf("\033[1;33mTESTE DE BUSCA\033[0m\n");
    for (int i = 0; i < MAXTESTE; i++) {
        if(binarySearch(arv, values_teste[i]) > 0) acertos++;
    }
    printf("\033[1;32mRESULTADO[%d/%d]\033[0m \n", acertos, MAXTESTE);
    puts("\033[1;32mPASSOU\033[0m");
}
void menor_maior(RBtree *arv)
{
    puts("\033[1;33mTESTANDO MAIOR/MENOR VALOR DA ARVORE...\033[0m");
    int maior_valor = values_teste[0];
    int menor_valor = values_teste[0];
    for (int i = 0; i < MAXTESTE ; i++) {
        if(values_teste[i] > maior_valor) maior_valor = values_teste[i];
        if(values_teste[i] < menor_valor) menor_valor = values_teste[i];
    }
    if(menor_valor != minValue(arv)){
        perror("\033[1;31m O MENOR VALOR DO ARRAY E DIFERENTE DO MENOR VALOR DA ARVORE\033[0m");
        exit(1);
    }
    if(menor_valor != minValue(arv)){
        perror("\033[1;31m O MAIOR VALOR DO ARRAY E DIFERENTE DO MAIOR VALOR DA ARVORE\033[0m");
        exit(1);
    }
    printf("\033[1;32mRESULTADO: [maior valor  array: %d, maior valor arvore: %d] , [menor valor  array: %d, menor valor arvore: %d]\033[0m\n", maior_valor, maxValue(arv) , menor_valor, minValue(arv));
    puts("\033[1;32mPASSOU\033[0m");
}
int checkarPropriedades(struct nodo *noAtual)
{
    if(noAtual == NULL) return 0;
    if(noAtual->color == RUMBRO)
    {
       if(
            noAtual->left != NULL && noAtual->left->color == RUMBRO ||
            noAtual->right != NULL && noAtual->right->color == RUMBRO
               )  return -1;
    }

    int left_black_heigth = checkarPropriedades(noAtual->left);
    int right_black_heigth = checkarPropriedades(noAtual->right);
    if( left_black_heigth == -1 ||
        right_black_heigth == -1 ||
        left_black_heigth != right_black_heigth
      ) return -1;
    if(noAtual->color == BLACK) return left_black_heigth + 1;
    else return left_black_heigth;
}

void validarArvoreRumbro(RBtree *arv){
    puts("\033[1;33mTESTANDO SE E UMA ARVORE RUMBRO NEGRA VALIDA...\033[0m");
    if(arv->raiz == NULL){
       return;
    }
    if (arv->raiz->color != BLACK) {
        perror("\033[1;31m A RAIZ DA ARVORE E RUMBRO\033[0m");
        exit(1);
    }
    if(checkarPropriedades(arv->raiz) == -1)
    {
        perror("\033[1;31m A ARVORE ESTA DESBALANCEADA\033[0m");
        exit(1);

    }
    puts("\033[1;32mPASSOU\033[0m");
}

int main(void){
    RBtree arv;
    newRBtree(&arv);
    srand(time(NULL));
    testar_criacao(&arv);
    testando_insercao(&arv);
    testando_percursos(&arv);
    menor_maior(&arv);
    validarArvoreRumbro(&arv);
    listarpai(&arv);
    removeRbtree(&arv, values_teste[1]);
    listarpai(&arv);
    validarArvoreRumbro(&arv);

    /*
    printf("%d\n",maxValue(&arv));
    printf("%d\n",minValue(&arv));    
    int *resultadoporlevel = porlevel(&arv);
    listarpai(&arv);
    int *resultadopreorder = preorder(&arv);
    
    for (int i = 0; i < arv.size; i++) {
        printf("%d ,", resultadoporlevel[i]);
    }
    printf("\n");
    for (int i = 0; i < arv.size; i++) {
        printf("%d ,", resultadopreorder[i]);
    }
    printf("\n");
    for (int i = 0; i < arv.size; i++) {
        printf("%d ,", values_teste[i]);
    }
    printf("\n");
    free(resultadopreorder);
    free(resultadoporlevel);
    resultadoporlevel = NULL;
    */
    freeRB(&arv);
    return 0;
}
