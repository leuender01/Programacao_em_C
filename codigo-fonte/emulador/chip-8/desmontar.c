#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "array.h"
#include <stdint.h>

int main(int argc, char *argv[]){
    if(argc > 2 || argc < 2) return 0;
    ArrayS array;
    FILE *fd = fopen(argv[1], "rb");
    while(1){
        uint16_t opcode = 0;
        uint16_t montar = 0;
        uint8_t dx = 0;
        uint8_t dy = 0;
        if((fread(&dx, sizeof(uint8_t), 1, fd)) <= 0) break;
        if((fread(&dy, sizeof(uint8_t), 1, fd)) <= 0) break;
        opcode = dy;
        montar = dx << 8;
        opcode |= montar;
        add_data(&array, UINT16, &opcode);
    }
    fclose(fd);

    for(int i = 0; i < array.length; i++){
        if(array.no[i].type == UINT16) {
            uint16_t opcode = *(uint16_t *)recuperar_dado(&array, i);
            uint8_t byte[4] = {0};
            char string[4];
            byte[3] = opcode & 0x000F;
            byte[2] = (opcode & 0x00F0) >> 4;
            byte[1] = (opcode & 0x0F00) >> 8;
            byte[0] = (opcode & 0xF000) >> 12;

            for(int j = 0; j < 4; j++){
                 if(byte[j] > 9){
                    string[j] = 'A' + (byte[j] - 10);
                 }else{
                    string[j] = '0' + byte[j];
                 }

            }
            add_data(&array, STRING, string);
        }
    }
    FILE *fc = fopen("desmontador.s", "w");
    for(int i = 0; i < array.length; i++){
        if(array.no[i].type == STRING) {
            printf("letra: %s\n", (char *)recuperar_dado(&array, i));
            char *frase = recuperar_dado(&array, i);
            fprintf(fc,"%s\n" ,frase);
        }
        if(array.no[i].type == UINT16){
            printf("hexadecimal: %0x\n", *(uint16_t *)recuperar_dado(&array, i));
        }
    }
    fclose(fc);
    freearray(&array);
    return 0;
}

