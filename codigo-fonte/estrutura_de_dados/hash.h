#ifndef HASH_H
#define HASH_H
#define TAM_TABLE_HASH 100 

#include <stdbool.h>

struct node{
    char *key;
    char *value;
    struct node *colision;  
};

typedef struct node* HASH[TAM_TABLE_HASH];

bool init(HASH table);
int calcularHash(char *string);
bool insertHash(HASH table, char *key, char *value);
bool SearchHash(HASH table, char *key);
bool removeNode(HASH table, char *key);
struct node* _removeNODE(struct node *node, char *key, bool *match);


#endif