#define HASH_H
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int inithash(HASH *table){
    struct node* no = malloc(sizeof(struct node) * TAM_INICIAL);
    if(no == NULL) return -1;
    memset(no, 0, sizeof(struct node) * TAM_INICIAL);
    for (int i = 0; i < TAM_INICIAL; i++ ){ 
        no[i].key = -1;
        no[i].value = -1;
    }
    table->size = 0;
    table->no = no;
    return 0;
}

/*
unsigned long calcularHash(char *string){
    unsigned long hash = 5381;
    int c;
    while((c = *string++)){
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TAM_INICIAL;
}
*/

int insertHash(HASH *table, int key, pthread_t value){
    if(key < 0) return 1;
//    unsigned long hash = calcularHash((char *)key);
    unsigned long hash = value % TAM_INICIAL;
    if(table->no[hash].key >=  0){
        struct node *no = malloc(sizeof(struct node));
        if(no == NULL){
            return 1;
        }
        no->key = key;
        no->value = value;
        no->colision = table->no[hash].colision;
        table->no[hash].colision = no;
        table->size++;
        table->debug.colision++;
        return 0;
    }
    table->no[hash].key = key;
    table->no[hash].value = value;
    table->no[hash].colision = NULL;
    table->size++;
    table->debug.normal++;
    return 0;
}

int insertHashValue(HASH *table, int key, pthread_t value){
    if(key < 0 ) return 1;
    unsigned long hash = value % TAM_INICIAL;
    if(table->no[hash].key > 0){
        struct node *no = malloc(sizeof(struct node));
        if(no == NULL){
            return 1;
        }
        no->key = key;
        no->value = value;
        no->colision = table->no[hash].colision;
        table->no[hash].colision = no;
        table->size++;
        table->debug.colision++;
        return 0;
    }
    table->no[hash].key = key;
    table->no[hash].value = value;
    table->no[hash].colision = NULL;
    table->size++;
    table->debug.normal++;
    return 0;
}

int searchHash(HASH *table, int key){
    if(key < 0) return 1;
//    unsigned long hash = calcularHash((char *)key);
    unsigned long hash = key % TAM_INICIAL;
    if(table->no[hash].key >= 0 && table->no[hash].key == key){
        return table->no[hash].value;
    }else if(table->no[hash].colision != NULL){
        struct node *aux = table->no[hash].colision;
        while(aux != NULL){
            if(aux->key >= 0 && aux->key == key){
                return aux->value;
            }
            aux = aux->colision;
        }
    }
    return 1;
}

int searchHashValue(HASH *table, pthread_t value){
    if(value < 0) return 1;
    unsigned long hash = value % TAM_INICIAL;
    if(table->no[hash].value >= 0 && table->no[hash].value == value){
        return table->no[hash].key;
    }else if(table->no[hash].colision != NULL){
        struct node *aux = table->no[hash].colision;
        while(aux != NULL){
            if(aux->value > 0 && aux->value == value){
                return aux->key;
            }
            aux = aux->colision;
        }
    }
    return 1;
}

int deleteHashValue(HASH *table, pthread_t value){
    if(value < 0 || table == NULL) return 1;
    unsigned long hash = value % TAM_INICIAL;
    if(table->no[hash].key >= 0 && table->no[hash].value == value){
        table->no[hash].key = -1;
        if(table->no[hash].colision != NULL){
            struct node *aux = table->no[hash].colision;
            table->no[hash].key = aux->key;
            table->no[hash].value = aux->value;
            table->no[hash].colision = aux->colision;
            free(aux);
            table->debug.colision--;
        } else{
            table->no[hash].value = -1;
            table->debug.normal--;
        }
        table->size--;
        return 0;
    }
    if( table->no[hash].colision != NULL){
        struct node *aux = table->no[hash].colision;
        struct node *prev = NULL;
        while(aux != NULL){
            if(aux->key >= 0 && aux->value == value){
                if(prev == NULL){
                    table->no[hash].colision = aux->colision;
                } else {
                    prev->colision = aux->colision;
                }
                aux->key= -1;
                free(aux);
                table->size--;
                table->debug.colision--;
                return 0;
            }
            prev = aux;
            aux = aux->colision;
        }
    }
    return 1;
}

int deleteHash(HASH *table, int key){
    if(key < 0 || table == NULL) return 1;
//    unsigned long hash = calcularHash((char *)key);
    unsigned long hash = key % TAM_INICIAL;
    if(table->no[hash].key >= 0 && table->no[hash].key ==  key){
        table->no[hash].key = -1;
        if(table->no[hash].colision != NULL){
            struct node *aux = table->no[hash].colision;
            table->no[hash].key = aux->key;
            table->no[hash].value = aux->value;
            table->no[hash].colision = aux->colision;
            free(aux);
            table->debug.colision--;
        } else{
            table->no[hash].value = -1;
            table->debug.normal--;
        }
        table->size--;
        return 0;
    }
    if( table->no[hash].colision != NULL){
        struct node *aux = table->no[hash].colision;
        struct node *prev = NULL;
        while(aux != NULL){
            if(aux->key >= 0 && aux->key == key){
                if(prev == NULL){

                    table->no[hash].colision = aux->colision;
                } else {
                    prev->colision = aux->colision;
                }
                free(aux);
                table->size--;
                table->debug.colision--;
                return 0;
            }
            prev = aux;
            aux = aux->colision;
        }
    }
    return 1;
}

void freehash(HASH *table){
    if(table == NULL || table->no == NULL) return;
    for(int i = 0; i < TAM_INICIAL; i++){
        if(table->no[i].key >= 0){
            close(table->no[i].key);
            pthread_join(table->no[i].value, NULL);
        }
        struct node *aux = table->no[i].colision;
        
        while(aux != NULL){
            struct node *temp = aux;
            aux = aux->colision;
            close(temp->key);
            pthread_join(temp->value,NULL);
            free(temp);
        }
    }
    free(table->no);
    table->no = NULL;
}
