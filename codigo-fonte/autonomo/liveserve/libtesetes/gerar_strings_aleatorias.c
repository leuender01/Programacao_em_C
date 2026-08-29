#include <time.h>
#include <string.h>
#include <stdlib.h>
#define TESTES
#include "testes.h"

char* gerarStringAletaria(void){
    srand(time(NULL));
    int temp_size = (rand() % 100) + 1;
    char *resultado = malloc(temp_size + 5);
    if(resultado == NULL) return NULL;
    const char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int len_alfabet = strlen(alfabeto);
    for(int i = 0; i < temp_size ; i++){
        resultado[i] = alfabeto[rand() % len_alfabet];
    }
    resultado[temp_size ] = '\0';
    return resultado;
}

int numerosAletaorios(int seed){
    int resultado = (rand() % seed) + 1;
    return resultado;
}
