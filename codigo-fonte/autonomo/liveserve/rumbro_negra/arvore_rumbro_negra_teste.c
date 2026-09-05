#include <stdlib.h>
#include <time.h>
#include "arvore_rumbro_negra.h"
#include "libtesetes/testes.h"

#define MAXTESTE 100
const int rodadas = 100;


void valores_porlevel(RBtree *arv)
{
    int *resultadopreorder = porlevel(arv);
    for (int i = 0; i < arv->size; i++) {
        printf("%d ,", resultadopreorder[i]);
    }
    free(resultadopreorder);
    printf("\n");
}

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
    int *arrayResultado = NULL;
    arrayResultado =  func(arv);
    if(arrayResultado == NULL){
        freeRB(arv);
        exit(1);
    }
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
//    EXPECT_COND(resultado != NULL);
    if(resultado == NULL) return -1;
    for (int i = 0; i < arv->size ; i++) 
    {

        insertRBtree(&treeTeste, resultado[i]);
    }
    resultadoTeste = preorder(&treeTeste);
//    EXPECT_COND(resultadoTeste != NULL); 
    if(resultadoTeste == NULL) return -1;
//    EXPECTED_EQ("TESTANDO SE O TAMANHO DAS ARVORES SAO IGUAIS", arv->size, treeTeste.size);
    if(arv->size != treeTeste.size) return -1;
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
    MESSAGE("TESTANDO A INSEÇÃO NA ARVORE");
    srand(time(NULL));
    for(int j = 0; j < rodadas; j++){
        int values_teste[MAXTESTE] = {0};
        RBtree arv;
        newRBtree( &arv);
        
        int inseridos = 0;
        for (int i = 0; i < MAXTESTE; i++) {
            values_teste[i] = numerosAletaorios(1000);
            if((insertRBtree(&arv, values_teste[i])) == 0) inseridos++;
            EXPECT_COND(checkarPropriedades(arv.raiz) > 0);
        }
        EXPECTED_EQ("TESTANDO INSEÇÃO...", inseridos,arv.size);
        freeRB(&arv);
    }
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
        insertRBtree(&arv, values_teste[i]);
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
    MESSAGE("TESTANDO PERCURSO INORDER...");
    int values_teste[MAXTESTE] = {0};   
    for (int j = 0; j < rodadas; j++) {
        RBtree arv;
        newRBtree( &arv);
        for (int i = 0; i < MAXTESTE; i++) {
            values_teste[i] = numerosAletaorios(1000);
            insertRBtree(&arv, values_teste[i]);
        }
        EXPECT_COND(orderLista(inorder,  &arv) == 0);
        freeRB(&arv);
    }
    TESTE_PASS();

}

TEST_CASE(testando_busca)
{
    MESSAGE("TESTE DE BUSCA");
    int values_teste[MAXTESTE] = {0};   
    srand(time(NULL));
    int teste = 0;
    for (int j = 0 ; j < rodadas; j++) {
        RBtree arv;
        newRBtree( &arv);
        for (int i = 0; i < MAXTESTE; i++) {
            values_teste[i] = numerosAletaorios(1000);
            insertRBtree(&arv, values_teste[i]);
        }
        int acertos = 0;
        for (int i = 0; i < MAXTESTE; i++) {
            if(binarySearch(&arv, values_teste[i]) == values_teste[i]) acertos++;
        }
        if(acertos == MAXTESTE) teste++;
        freeRB(&arv);
    }
    SUMARY(teste, rodadas);
    TESTE_PASS();
}

TEST_CASE(testando_porlevel)
{
    MESSAGE("TESTANDO PERCURSO PORLEVEL...");
    int values_teste[MAXTESTE] = {0};   
    RBtree arv;
    newRBtree( &arv);
    int * resultado = NULL;
    for (int i = 0; i < MAXTESTE; i++) {
        values_teste[i] = numerosAletaorios(1000);
        insertRBtree(&arv, values_teste[i]);
    }
    resultado =  testas_recurçao(porlevel, &arv);
    EXPECT_COND(resultado != NULL);
    free(resultado);
    freeRB(&arv);
    TESTE_PASS();
}

TEST_CASE(menor_maior)
{
    MESSAGE("TESTANDO SE ENCONTROU MAIOR E MENOR VALOR");
    int values_teste[MAXTESTE] = {0};
    for(int j = 0; j < rodadas; j++){
        RBtree arv;
        newRBtree( &arv);
        for (int i = 0; i < MAXTESTE; i++) {
            values_teste[i] = numerosAletaorios(1000);
            insertRBtree(&arv, values_teste[i]);
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
    }
    TESTE_PASS();
    return 0;
}


TEST_CASE(validarArvoreRumbro)
{
    MESSAGE("TESTANDO SE E UMA ARVORE RUMBRO NEGRA VALIDA...");
    int values_teste[MAXTESTE] = {0};
    int relatorio = 0;
    for(int j = 0; j < rodadas; j++){
        RBtree arv;
        newRBtree( &arv);
        for (int i = 0; i < MAXTESTE; i++) {
            values_teste[i] = numerosAletaorios(1000);
            insertRBtree(&arv, values_teste[i]);
        }
        if(arv.raiz == NULL){
           return 0;
        } 
        
        EXPECTED_EQ("CHECANDO SE A ARVORE E UMA ARVORE RUMBRO NEGRA VALIDA", arv.raiz->color,  BLACK);
        int result = checkarPropriedades(arv.raiz);
        EXPECT_COND(result != -1);
        relatorio++;
        freeRB(&arv);
    }
    SUMARY(relatorio, rodadas);
    TESTE_PASS();
}


TEST_CASE(remocao_arvore)
{
    int values_teste[MAXTESTE] = {0};
    MESSAGE("CHECANDO SE E UMA ARVORE RUMBRO NEGRA VALIDA E REMOVENDO INTEIRA");
    for(int j = 0; j < rodadas; j++){
        RBtree arv;
        newRBtree( &arv);
        EXPECT_COND(arv.raiz == NULL && arv.size == 0);
        for (int i = 0; i < MAXTESTE; i++) {
            values_teste[i] = numerosAletaorios(1000);
            insertRBtree(&arv, values_teste[i]);
        }
        int result = removeRbtree(&arv, values_teste[0]);
        EXPECTED_EQ("TESTANDO SE A REMOÇÃO FOI VALIDA", result, 1);
        result = removeRbtree(&arv, values_teste[0]);
        EXPECTED_EQ("TESTANDO SE A REMOÇÃO DE UM ELEMENTO JA REMOVIDO", result, 0);
        int value_raiz_old = arv.raiz->key;
        EXPECTED_EQ("REMOVENDO A RAIZ",  removeRbtree(&arv, arv.raiz->key), 1);
        EXPECT_COND(value_raiz_old != arv.raiz->key);
        EXPECTED_EQ("TESTANDO SE A ARVORE A RAIZ DA ARVORE E BLACK", arv.raiz->color,  BLACK);
        EXPECT_COND(checkarPropriedades(arv.raiz) > 0);
        for (int i = 0; i < MAXTESTE; i++) {
            removeRbtree(&arv, values_teste[i]);
        }
        EXPECT_COND(arv.raiz == NULL && arv.size == 0);
        EXPECTED_EQ("TESTANDO LIBERAR MEMORIA ", freeRB(&arv), 0) ;
    }
    TESTE_PASS();
}


TEST_SUITE(
        remocao_arvore, 
        testando_busca,
        testando_inorder,
        testando_insercao,
        testando_porlevel,
        testando_preorder,
        menor_maior,
        validarArvoreRumbro,
        testar_criacao
        )
