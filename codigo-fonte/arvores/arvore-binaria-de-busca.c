#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "arvore-binaria-de-busca.h"
#include "arvore-de-busca-AVL.h"// caso queira incluir rotaçoes na inserção da arvore binaria tem que incluir isso

/*
    A Parte do codigo em si nao tem utilidade real apenas eu 
    aprendendo a fazer uma.
*/

struct no * insertNode(NODE node, int value){
    
    /*
        A o mesmo tempo que cria um novo nó ele realizar uma travercia 
        usando a busca binaria mantendo a arvore com os elementos com
        maior valor a direito e os menores ao lado esquerdo.
    */

    if(node == NULL){
        node = malloc(sizeof(struct no));
        if(node == NULL) return node;
        node->value = value;
        node->left = NULL;
        node->right = NULL;
        node->height = 0; 
        return node;
    };

    if(value < node->value){
        node->left = insertNode(node->left, value);
        updateheigth(node->left);
    }else if(value > node->value){
        node->right = insertNode(node->right, value);
        updateheigth(node->right);
    }; 
    updateheigth(node);
    //printf("a altura do node %d, altura %d\n", node->value, node->height);
    node = balancear(node);
    return node;
}


void inorder(NODE node){

    /*
        A parte recursiva para p
        ercorrer a arvore em ordem 
        da esquerda para direita.
    */

    if(node->left != NULL) inorder(node->left);
    printf(", %d",node->value);
    if(node->right != NULL) inorder(node->right);
    return;
}



void preorder(NODE node){

    /*
        A parte recursiva da função 
        que devolve a raiz e vai ate 
        nao ter mais elementos a serem
        imperessos.
    */

    printf(", %d",node->value);
    if(node->left != NULL) preorder(node->left);
    if(node->right != NULL) preorder(node->right);
    return;
}

bool BinarySearch(NODE node,int arg){

    /*
        Essa função faz a busca binaria e devolve
        um inteiro, sendo a parte recursiva da função,
        podendo adaptar para qualquer coisa.
    */

    if(node->value == arg) return true;
    if(arg < node->value && node->left != NULL) return BinarySearch(node->left,arg);
    else if(arg > node->value && node->right != NULL) return BinarySearch(node->right,arg);
    return false;
}


void freenode(NODE node){

    /*
        Libera a memoria alocada para os nós da arvore.
    */
   /*
    Queue fila;
    newQueue(&fila);
    Enqueue(&fila, node);
    while (fila.size)
    {
        node = Dequeue(&fila);
        if(node->left != NULL) Enqueue(&fila, node->left);
        if(node->right != NULL) Enqueue(&fila, node->right);       
        free(node);
    }
        */
    if(node == NULL) return;

    freenode(node->left);
    freenode(node->right);   
    free(node);

}

bool max(NODE tree){

    /*
        Procura pelo maior elemento na arvore.
    */

    if(tree == NULL) return false;
    NODE aux = tree;
    while(aux->right != NULL) aux = aux->right;
    if(aux == NULL) return false;
    printf("%d\n", aux->value);    
    return true;    
}

bool min(NODE tree){

    /*
        Procura pelo menor elemento na arvore.
    */

    if(tree == NULL) return false;
    NODE aux = tree;
    while(aux->left != NULL) aux = aux->left;
    if(aux == NULL) return false;
    printf("%d\n", aux->value);    
    return true;    
}

int _min(NODE aux){

    /*
        Usando em conjunto com a função de remoção, 
        sendo necessaria para balancear a arvore.
        Nao precisa necessariamente criar um função separada, 
        da para juntar com função de cima, mais prefiro fazer separada.
        A o final ela retorna o no com o menor elemento da arvore.
    */

    if(aux == NULL) return -1;
    while(aux->left != NULL) aux = aux->left;   
    return aux->value;    
}

NODE removeNode(NODE *node_prt, int arg){

    /*
        Função recursiva que vai remover o no em tres casos no primeiro se o 
        no existir e nao tiver filhos ele vai sobrescrever com o valor NULL e 
        depois liberar a meoria, caso tenha apenas um filhos esse filho vira o
        novo no, e caso ele tenha os dois ele vai pegar o menor no a direita da
        raiz e vai virar  novo no da arvore apagando a sua posição anterior, 
        e claro usando a Busca binaria para percorrer a arvore.
    */

    NODE node = *node_prt;
    if(*node_prt == NULL) return NULL;
    if(arg < (*node_prt)->value){
        (*node_prt)->left = removeNode(&(*node_prt)->left,arg);
    }else if(arg > node->value){
        (*node_prt)->right = removeNode(&(*node_prt)->right,arg);
    }else if(node->value == arg){
        NODE temp = *node_prt;
        if((*node_prt)->left == NULL || (*node_prt)->right == NULL){
            
            NODE filho = ((*node_prt)->left)? (*node_prt)->left : (*node_prt)->right;
            free(temp);
            *node_prt = filho;
            return *node_prt;

        }else{
            int aux = _min((*node_prt)->right);
            (*node_prt)->value = aux;
            (*node_prt)->right = removeNode(&((*node_prt)->right), aux);
        };
    };
    updateheigth(*node_prt);
    *node_prt = balancear(*node_prt);
    return *node_prt;
}

void porlevel(NODE tree){

    /*
        Busca por nivel ela
        vai mostrar os elementos da arvore 
        por nivel, sendo dependente de outra 
        estrutura de dados chamado Fila.
    */

    if(tree == NULL) return;
    Queue fila;
    NODE aux = tree;
    newQueue(&fila);
    Enqueue(&fila, aux);
    printf("Porlevel>>>\n");
    while (fila.size)
    {
        aux = Dequeue(&fila);
        if(aux->left) Enqueue(&fila, aux->left);
        if(aux->right) Enqueue(&fila, aux->right);       
        printf("%d, ",aux->value);
    }
    
    printf("\n");
}

int height(NODE node, int len){

    /*
        Função para calcular a altura de uma arvore,
        pretendo usar futuramente para colar arvores AvL.
    */

    return ( node == NULL)? -1 : node->height;
}
