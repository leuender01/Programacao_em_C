#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

/*
    isso e so um arquivo de cabeçalho contendo 
    prototipos de funções, usadas em um arquivo 
    separado, tava encontrando muitos problemas 
    em inserir coisas que eu ja fiz antes entao 
    decidi separar cada parte importante do codigo
    em um arquivo, que eu possa mecher um por vez.
*/

//--------------Bibliotecas_De_Codigos------------------

#include <stdbool.h> // biblioteca de booleanos
#include "arvore-base.h" // inclui a estrutura base para uma arvore
#include "../estrutura_de_dados/Queue.h" // inclui o cabeçalho do codigo da fila

//---------------Prototipos_de_funções------------------

// <<<<<<<<<< INICIALIZA >>>>>>>>>>>>

NODE init(NODE tree); // prototipo de função que inicializa a arvore

// <<<<<<<<<< INSERÇÃO >>>>>>>>>>>>

struct no* insertNode(NODE  node, int value);// prototipo de função principal que insere um no

// <<<<<<<<<<<<<<<<< BUSCA >>>>>>>>>>>>>>>>>>>

void inorder(NODE tree);// função recursiva
void preorderDFS(NODE  node);// percorre da raiz ate nao ter mais elementos sempre começando da esquerda para direita
void preorder(NODE tree);// função recursiva
void porlevel(NODE tree);// função que busca por nivel e necessario uma estrutura de dados semelhante a uma fila
bool BinarySearch(NODE  node,int arg);// a parte recursiva sa busca binaria
bool max(NODE tree); // mostra o maior elemento dentro de uma arvore binaria
bool min(NODE tree); // mostra o menor elemento dentro de uma arvore binaria
int height(struct no* node, int len);// FUnção principal que vai procurar o mair caminho dentro da arvore

// <<<<<<<<<< REMOVE UM NÓ DA ARVORE >>>>>>>>>>>>

NODE removeNode(NODE *node, int arg);// a parte recursiva da remoção
int _min(NODE  aux); // usada em conjunto com a função de remover pode ser substituida

//<<<<<<<<< LIBERA A MEMORIA ALOCADA >>>>>>>>>>>>>>

void freenode(NODE  node); // libera a memoria alocado nos nós

#endif
