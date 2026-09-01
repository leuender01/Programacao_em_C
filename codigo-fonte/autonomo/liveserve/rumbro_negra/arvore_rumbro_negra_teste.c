#include <stdio.h>
//#include <time.h>
#include <stdlib.h>
#include "arvore_rumbro_negra.h"
#include "libtesetes/testes.h"
#define MAXTESTE 10


void inoder_funcition(struct  nodo *no)
{
    if(no == NULL) return;
    printf("pai: %d, filho: %d, COLOR:%s \n", (no->dad != NULL) ? no->dad->key : -1, no->key , (no->color == BLACK) ? "BLACK" : "RUMBRO");
    if(no->left != NULL) inoder_funcition(no->left);
    if(no->right != NULL) inoder_funcition(no->right);
    return;
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

void listarpai(RBtree *tree)
{
    if(tree == NULL) return;
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
    MESSAGE("COMPARANDO DUAS ARVORES");
    RBtree treeTeste;
    newRBtree(&treeTeste);
    int *resultadoTeste = NULL;
    int *resultado = preorder(arv);
    EXPECT_COND(resultado != NULL);
    for (int i = 0; i < arv->size ; i++) 
    {
        insertRBtree(&treeTeste, resultado[i],  (void *)&resultado[i]);
    }
    resultadoTeste = preorder(&treeTeste);
    EXPECT_COND(resultadoTeste != NULL);
    EXPECTED_EQ("TESTANDO SE O TAMANHO DAS ARVORES SAO IGUAIS", arv->size, treeTeste.size);
    int data = 0;
    for(int i = 0; i < arv->size; i++){
        if(resultado[i] != resultadoTeste[i]) data = 1;
    }
    freeRB(&treeTeste);
    free(resultadoTeste);
    free(resultado);
    return data;
}

TEST_CASE(testar_criacao)
{
    MESSAGE("TESTANDO A CRIAÇÃO DE UMA NOVA ARVORE");
    RBtree arv;
    newRBtree(&arv);
    EXPECT_COND(arv.size == 0 && arv.raiz == NULL )
    TESTE_PASS();
}

TEST_CASE(testando_insercao)
{
    int values_teste[MAXTESTE] = {0};
    RBtree arv;
    newRBtree( &arv);
    
    int inseridos = 0;
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        if((insertRBtree(&arv, values_teste[i], (void *)&values_teste[i])) == 0) inseridos++;
    }
    EXPECTED_EQ("TESTANDO INSEÇÃO...", inseridos,arv.size);
    printf("\033[1;32mRESULTADO: [%d/%lu]  \033[0m\n", inseridos, arv.size);
    freeRB(&arv);
    TESTE_PASS();
}


TEST_CASE(testando_preorder)
{
    MESSAGE("TESTANDO PERCURSO PREORDER...");
    int values_teste[MAXTESTE] = {0};   
    RBtree arv;
    newRBtree( &arv);
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i], (void *)&values_teste[i]);
    }
    int* resultado = NULL;
    EXPECT_COND(compararArvore(&arv) > 0);   
    free(resultado);
    resultado = NULL;
    puts("\033[1;32mPASSOU\033[0m");
    freeRB(&arv);
    TESTE_PASS();
}

TEST_CASE(testando_inorder)
{
    int values_teste[MAXTESTE] = {0};   
    RBtree arv;
    newRBtree( &arv);
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i], (void *)&values_teste[i]);
    }
    MESSAGE("TESTANDO PERCURSO INORDER...");
    EXPECT_COND(orderLista(inorder,  &arv) == 0);
    freeRB(&arv);
    TESTE_PASS();



}

TEST_CASE(testando_busca)
{
    int values_teste[MAXTESTE] = {0};   
    RBtree arv;
    newRBtree( &arv);
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i], (void *)&values_teste[i]);
    }
    int acertos = 0;
    printf("\033[1;33mTESTE DE BUSCA\033[0m\n");
    for (int i = 0; i < MAXTESTE; i++) {
        if((int *)binarySearch(&arv, values_teste[i]) != NULL) acertos++;
    }
    EXPECTED_EQ("TESTANDO SE A BUSCA NA ARVORE RUMBRO NEGRA", acertos, 0);
    freeRB(&arv);
    TESTE_PASS();
}



TEST_CASE(testando_porlevel)
{
    int values_teste[MAXTESTE] = {0};   
    RBtree arv;
    newRBtree( &arv);
    int * resultado = NULL;
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i], (void *)&values_teste[i]);
    }
    MESSAGE("TESTANDO PERCURSO PORLEVEL...");
    resultado =  testas_recurçao(porlevel, &arv);
    EXPECT_COND(resultado != NULL);
    free(resultado);
    resultado = NULL;
    freeRB(&arv);
    TESTE_PASS();
}

TEST_CASE(menor_maior)
{
    int values_teste[MAXTESTE] = {0};
    RBtree arv;
    newRBtree( &arv);
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i], (void *)&values_teste[i]);
    }
    
    int maior_valor = values_teste[0];
    int menor_valor = values_teste[0];
    for (int i = 0; i < MAXTESTE ; i++) {
        if(values_teste[i] > maior_valor) maior_valor = values_teste[i];
        if(values_teste[i] < menor_valor) menor_valor = values_teste[i];
    }
    EXPECTED_EQ("TESTANDO SE SE A ARVORE ENCONTROU O MENOR VALOR", menor_valor, minValue(&arv));
    EXPECTED_EQ("TESTANDO SE SE A ARVORE ENCONTROU O MAIOR VALOR", maior_valor, maxValue(&arv));
    freeRB(&arv);
    TESTE_PASS();
    return 0;
}


TEST_CASE(validarArvoreRumbro)
{
    int values_teste[MAXTESTE] = {0};
    RBtree arv;
    newRBtree( &arv);
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i], (void *)&values_teste[i]);
    }
    puts("\033[1;33mTESTANDO SE E UMA ARVORE RUMBRO NEGRA VALIDA...\033[0m");
    if(arv.raiz == NULL){
       return 0;
    } 
    
    EXPECTED_EQ("TESTANDO SE A ARVORE A RAIZ DA ARVORE E RUMBRO", arv.raiz->color,  BLACK);
    MESSAGE("CHECANDO SE A ARVORE E UMA ARVORE RUMBRO NEGRA VALIDA");
    EXPECT_COND(checkarPropriedades(arv.raiz) != -1);
    freeRB(&arv);
    TESTE_PASS();
}

void valores_porlevel(RBtree *arv)
{

    int *resultadopreorder = porlevel(arv);
    for (int i = 0; i < arv->size; i++) {
        printf("%d ,", resultadopreorder[i]);
    }
    free(resultadopreorder);
    printf("\n");
}

TEST_SUITE(menor_maior, testando_insercao, testar_criacao, testando_preorder, testando_inorder, testando_porlevel,  testando_busca, validarArvoreRumbro); 


//int main(void){
//    RBtree arv;
//    newRBtree(&arv);
//    srand(time(NULL));
//    testar_criacao(&arv);
//    testando_insercao(&arv);
//    menor_maior(&arv);
//    listarpai(&arv);
//    validarArvoreRumbro(&arv);
//    removeRbtree(&arv, 916);
//    validarArvoreRumbro(&arv);
//    removeRbtree(&arv, 887);
//    validarArvoreRumbro(&arv);
//    removeRbtree(&arv, 493);
//    validarArvoreRumbro(&arv);
//    valores_porlevel(&arv);
//
    /*
//    testando_percursos(&arv);
//    listarpai(&arv);
//    listarpai(&arv);
//    valores_porlevel(&arv);
//    listarpai(&arv);
//    valores_porlevel(&arv);
//    listarpai(&arv);
//    validarArvoreRumbro(&arv);
//    valores_porlevel(&arv);
    printf("%d\n",maxValue(&arv));
    printf("%d\n",minValue(&arv));    
    int *resultadoporlevel = porlevel(&arv);
    listarpai(&arv);
    
    for (int i = 0; i < arv.size; i++) {
        printf("%d ,", resultadoporlevel[i]);
    }
    printf("\n");
    printf("\n");
    for (int i = 0; i < arv.size; i++) {
        printf("%d ,", values_teste[i]);
    }
    printf("\n");
    free(resultadoporlevel);
    resultadoporlevel = NULL;
    */
//    freeRB(&arv);
//    return 0;
//}
