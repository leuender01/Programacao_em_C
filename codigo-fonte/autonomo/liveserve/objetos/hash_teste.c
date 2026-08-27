#include <stdlib.h>
#include <stdio.h>
#define TESTES
#include "./libtesetes/testes.h"
#include "../hash.h"
#include <string.h>
#include <pthread.h>
#define SIZE 100

void testar_numeros(void);
unsigned int tabela_valores[SIZE] = {0};
HASH table;

int main(void){
    srand(time(NULL));
    
    inithash(&table);
    for(int i = 0; i < SIZE; i++){
        tabela_valores[i] = numerosAletaorios();
    }
    int j = 0;
    for(int i = 0; i < SIZE; i++){
        printf("| %u ", tabela_valores[i]);
        if(j < 15){ 
            printf("|");
            j++;
        }
        else{ 
            printf("|\n");
            j = 0;
        }
    }
    printf("|\n");
    testar_numeros();
    freehash(&table);

    return 0;
}

void testar_numeros(void){
    printf("\033[1;34m[testando inserção]\033[0m\n");
    //teste a inserçao na tabela hash inserindo todos os elementos dentro da variavel tabela_letras e o valor pode ser qualquer um
    //printf("%s\n", tabela_letras[1]);
    for(int i = 0; i < SIZE; i++){
        if(insertHash(&table, tabela_valores[i], i)){ 
            printf("\033[32m[FALHOU] ao inserir a chave %ud\033[0m\n", tabela_valores[i]);
            
        }
    }
    printf("\033[1;32m[OK]\033[0m passou ao inserir o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);

    printf("\033[1;34m[testando busca]\033[0m\n");

    for(int i = 0; i < SIZE; i++){
        int a = searchHash(&table, tabela_valores[i]);
        if(a < 0){
            printf("[%d] error\n", a);
            
        }
    }

    printf("\033[1;32m[OK]\033[0m passou na busca, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);

    printf("\033[1;34m[testando deleção]\033[0m\n");

    for(int i = 0; i < SIZE; i++){
        int a = deleteHash(&table, tabela_valores[i]);
        if(a == -1){
            printf("[%d] error\n", a);
        }
    }   
    
    printf("\033[1;32m[OK]\033[0m passou na deleção, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);

    printf("\033[1;34m[testando busca]\033[0m\n");

    for(int i = 0; i < SIZE; i++){
        int a = searchHash(&table, tabela_valores[i]);
        if(a < 0){
            printf("[%d] error\n", a);
        }
    }

    printf("\033[1;32m[OK]\033[0m passou na busca, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);

    printf("\033[1;34m[testando inserção]\033[0m\n");
    //teste a inserçao na tabela hash inserindo todos os elementos dentro da variavel tabela_letras e o valor pode ser qualquer um
    //printf("%s\n", tabela_letras[1]);
    for(int i = 0; i < SIZE; i++){
        if(insertHash(&table, tabela_valores[i], i)){ 
            printf("\033[32m[FALHOU] ao inserir a chave %ud\033[0m\n", tabela_valores[i]);
            
        }
    }
    printf("\033[1;32m[OK]\033[0m passou ao inserir o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);

    printf("\033[1;34m[testando deleção]\033[0m\n");

    for(int i = 0; i < SIZE/2; i++){
        int a = deleteHash(&table, tabela_valores[i]);
        if(a == -1){
            printf("[%d] error\n", a);
        }
    }   
    
    printf("\033[1;32m[OK]\033[0m passou na deleção, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
}

//void testar_value(){
//    printf("\033[1;34m[testando inserção]\033[0m\n");
//    //teste a inserçao na tabela hash inserindo todos os elementos dentro da variavel tabela_letras e o valor pode ser qualquer um
//    //printf("%s\n", tabela_letras[1]);
//    for(int i = 0; i < SIZE; i++){
//        if(insertHashValue(&table, tabela_valores[i], i)){ 
//            printf("\033[32m[FALHOU] ao inserir a chave %ud\033[0m\n", tabela_valores[i]);
//            
//        }
//    }
//    printf("\033[1;32m[OK]\033[0m passou ao inserir o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
//
//    printf("\033[1;34m[testando busca]\033[0m\n");
//
//    for(int i = 0; i < SIZE; i++){
//        int a = searchHashValue(&table, i);
//        if(a > 0){
//            printf("[NUMBER] error\n");
//            
//        }
//    }
//
//    printf("\033[1;32m[OK]\033[0m passou na busca, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
//
//    printf("\033[1;34m[testando deleção]\033[0m\n");
//    for(int i = 0; i < SIZE; i++){
//        int a = deleteHashValue(&table, i);
//        if(a == -1){
//            printf("[%d] error\n", a);
//        }
//    }   
//    
//    printf("\033[1;32m[OK]\033[0m passou na deleção, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
//
//    printf("\033[1;34m[testando busca]\033[0m\n");
//
//    for(int i = 0; i < SIZE; i++){
//        int a = searchHashValue(&table, i);
//        if(a > 0){
//            printf("[NUMBER] error\n");
//            
//        }
//    }
//
//    printf("\033[1;32m[OK]\033[0m passou na busca, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
//
//    printf("\033[1;34m[testando inserção]\033[0m\n");
//    //teste a inserçao na tabela hash inserindo todos os elementos dentro da variavel tabela_letras e o valor pode ser qualquer um
//    //printf("%s\n", tabela_letras[1]);
//    for(int i = 0; i < SIZE; i++){
//        if(insertHashValue(&table, tabela_valores[i], i)){ 
//            printf("\033[32m[FALHOU] ao inserir a chave %ud\033[0m\n", tabela_valores[i]);
//            
//        }
//    }
//    printf("\033[1;32m[OK]\033[0m passou ao inserir o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
//
//    printf("\033[1;34m[testando deleção]\033[0m\n");
//
//    for(int i = 0; i < SIZE/2; i++){
//        int a = deleteHashValue(&table, i);
//        if(a == -1){
//            printf("[%d] error\n", a);
//        }
//    }   
//    
//    printf("\033[1;32m[OK]\033[0m passou na deleção, o tamanho atual e %d colisoes %d normal %d\n", table.size, table.debug.colision, table.debug.normal);
//}
