#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hash.h"


int main(void){
    HASH table;
    init(table);
    insertHash(table, "ola", "leuzim do rodo ola");
    insertHash(table, "carniça", "leuzim do rodo carniça");
    insertHash(table, "sabado", "leuzim do rodo sabado");
    insertHash(table, "valadao", "leuzim do rodo valadao");
    insertHash(table, "servo", "leuzim do rodo servo");
    insertHash(table, "cachorro", "leuzim do rodo cachorro");
    insertHash(table, "nofa", "leuzim do rodo nofa");
    insertHash(table, "serafim", "leuzim do rodo serafim");
    insertHash(table, "pouvora", "leuzim do rodo pouvora");
    
    for(int i = 0; i < TAM_TABLE_HASH - 1; i++){
        if(table[i]) printf("Hash de %p\n", table[i]);
    }

    removeNode(table, "carniça");
    SearchHash(table, "ola");
    SearchHash(table, "carniça");
    SearchHash(table, "sabado");
    SearchHash(table, "servo");
    SearchHash(table, "cachorro");
    SearchHash(table, "valadao");
    SearchHash(table, "nofa");
    SearchHash(table, "serafim");
    SearchHash(table, "pouvora");

    for(int i = 0; i < TAM_TABLE_HASH - 1; i++){
        if(table[i]) printf("Hash de %p\n", table[i]);
    }

    return 0;
}

bool init(HASH table){
    for(int i = 0; i < TAM_TABLE_HASH - 1; i++){
        table[i] = NULL;
    }
    return true;
}

bool SearchHash(HASH table, char *key){
    int hash = calcularHash(key);
    if(table[hash] == NULL) return false;
    if(!strcmp(table[hash]->key, key))
    {
        printf("%s\n", table[hash]->value);
        return true;
    }else{
        struct node *aux = table[hash];
        while(aux){
            if(!strcmp(aux->key, key))
            {
                printf("%s\n", aux->value);
                return true;
            }
            aux = aux->colision;
        }
        return false;
    }
    return true;

}

bool insertHash(HASH table, char *key, char *value){
    int hash = calcularHash(key);
    printf("hash insert %d\n", hash);
    if(table[hash] == NULL){
        struct node* novoNode = malloc(sizeof(struct node));
        if(novoNode == NULL) return false;
        novoNode->key = key;
        novoNode->value = value;
        novoNode->colision = NULL;
        table[hash] = novoNode;
        return true;
    }else{
        if(!strcmp(table[hash]->key, key)) return false;
        struct node* novoNode = malloc(sizeof(struct node));
        if(novoNode == NULL) return false;
        novoNode->key = key;
        novoNode->value = value;
        novoNode->colision = table[hash];
        table[hash] = novoNode; 
        return true;
    }
}

int calcularHash(char *string){
    int calc = 0;
    for(int i = 0; i < strlen(string); i++){
        calc = calc + *(string + i);
        calc = calc << 1;
    }
    return calc % TAM_TABLE_HASH;
}

bool removeNode(HASH table, char *key){
    int hash = calcularHash(key);
    printf("hash remove %d\n", hash);
    if(table[hash] == NULL) return false;
    if(!strcmp(table[hash]->key, key)){
        table[hash] = table[hash]->colision;
        return true;
    }else{
        bool match = false;
        table[hash]->colision = _removeNODE(table[hash]->colision, key, &match);
        return match;
    }
    
}

struct node* _removeNODE(struct node *node, char *key, bool *match){
    if(node == NULL) return node;
    if(!strcmp(node->key, key))
    {
        *match = true;
        struct node* temp = node->colision;
        free(node);
        return temp;
    }
    node->colision = _removeNODE(node->colision, key, match);
    return node;
}