#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void freearray(ArrayS *array){
    for(int i = 0; i < array->length; i++){
        free(array->no[i].data);
    }
    free(array->no);
    return;
}

void add_data(ArrayS *array, Types type, void *data){
    if(array->no == NULL){
        array->max = 4;
        struct Node *newno = malloc(sizeof(struct Node) * array->max);
        if(newno == NULL) return;
        array->length = 0;
        array->no = newno;
    }
    if(array->length >= array->max){
        array->max *= 2;
        struct Node *temp = realloc(array->no, array->max *  sizeof(struct Node));
        if(temp == NULL) return;
        array->no = temp;
    }
    void *newdata;
    switch(type){
        case UINT16:
            newdata = (void *)malloc(sizeof(uint16_t));
            if(newdata == NULL) return;
            *(uint16_t *)newdata = *(uint16_t *)data;
            array->no[array->length].data = newdata;
            array->no[array->length].type = UINT16;
            break;
        case STRING:
            newdata = (void *)malloc(strlen((char *)data) + 1);
            if(newdata == NULL) return;
            strcpy((char *)newdata, (char *)data);
            array->no[array->length].data = newdata;
            array->no[array->length].type = STRING;
            break;
        case INTEIRO:
            newdata = (void *)malloc(sizeof(int));
            if(newdata == NULL) return;
            *(int *)newdata = *(int *)data;
            array->no[array->length].data = newdata;
            array->no[array->length].type = INTEIRO;
            break;
        case FLOAT:
            newdata = (void *)malloc(sizeof(float));
            if(newdata == NULL) return;
            *(float *)newdata = *(float *)data;
            array->no[array->length].data = newdata;
            array->no[array->length].type = FLOAT;
            break;
        default:
            return;
    }
    array->length++;
}

void *recuperar_dado(ArrayS *array, int index){
    if( index < 0 || index >= array->length) return NULL;
    switch(array->no[index].type){
        case STRING:
            return (char *)array->no[index].data;
        case UINT16: 
            return (uint16_t *)array->no[index].data;
        case INTEIRO:
            return (int *)array->no[index].data;
        case FLOAT:
            return (float *)array->no[index].data;
        default:
            return array->no[index].data;
    }
}
