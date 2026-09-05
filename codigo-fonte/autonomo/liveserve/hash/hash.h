#ifndef  HASH_H
#define HASH_H
#define TAM_INICIAL 1009
#include <pthread.h>
struct node{
    int key;
    pthread_t value;
    struct node *colision;  
};

/*
struct debug{
    int colision;
    int normal;
};
*/
typedef struct{
    int size;
    struct node* no; 
}HASH;

int inithash(HASH *table);
//unsigned long calcularHash(char *string);
int insertHashValue(HASH *table, int key, pthread_t value);
int insertHash(HASH *table, int key, pthread_t value);
pthread_t searchHash(HASH *table, int key);
pthread_t searchHashValue(HASH *table, pthread_t value);
int deleteHash(HASH *table, int key);
int deleteHashValue(HASH *table, pthread_t value);
void freehash(HASH *table);
#endif
