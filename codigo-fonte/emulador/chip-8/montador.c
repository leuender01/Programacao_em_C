#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "array.h"

const char hexadecimal[] = {'a','b','c','d','e','f'};

int main(int argc, char *argv[]){
    ArrayS array;
    memset(&array, 0, sizeof(ArrayS));
    if(argc != 2){ 
        printf("De uma entrada valida\n");
        return 0;
    } 
    char linha[200];
    memset(linha, 0, sizeof(linha));
    FILE *fd = fopen(argv[1], "r");
    while((fgets(linha, sizeof(linha), fd) != NULL)){
        linha[strcspn(linha, "\n")] = '\0';
        if((strstr(linha, ";")) == NULL){
            add_data(&array, STRING, linha);
        }
    }
    fclose(fd);
    for(int i = 0; i < array.length; i++){
        if(array.no[i].type == STRING){
            char *resultado = recuperar_dado(&array, i);
            int size = strlen(resultado);
            uint16_t opcode = 0x0000;
            for(int a = 0; a < size; a++){
                char letra = tolower(resultado[a]);
                bool troca = true;
                uint16_t montar = 0x0000;
                for(int b = 0; b < sizeof(hexadecimal); b++){
                    if(letra == hexadecimal[b]){
                        montar = 0x0000;
                        montar = b + 10;
                        montar = montar << (((size - 1) - a) * 4);
                        opcode |= montar;
                        troca = false;
                        break;
                    }else{
                        continue;
                    }
                }
                if(troca && letra != ' ' && letra != '0'){ 
                    montar = 0;
                    montar = (int)(letra - '0');
                    montar = montar << (((size - 1) - a) * 4);
                    opcode |= montar;
                    troca = false;
            }
            }
            add_data(&array, UINT16, &opcode);
        }
    }
    printf("%i %i\n", array.max, array.length);
    FILE *file = fopen("binario.ch8", "wb");
    for(int i = 0; i < array.length; i++){
        if(array.no[i].type == STRING) printf("strings: %s\n", recuperar_dado(&array, i));
        if(array.no[i].type == UINT16){
            uint16_t dado = *(uint16_t *)(recuperar_dado(&array, i));
            printf("inteiro 16: %0x\n", dado);
            uint8_t dx = (dado & 0xFF00) >> 8;
            uint8_t dy = (dado & 0x00FF);
            fwrite(&dx, sizeof(uint8_t), 1, fd);
            fwrite(&dy, sizeof(uint8_t), 1, fd);
        }
    }
    fclose(file);
    freearray(&array);
    return 0;
}
