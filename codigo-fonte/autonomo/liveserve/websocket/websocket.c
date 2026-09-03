#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include "websocket.h"
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "hash/hash.h"
#include "Queuen/Queue.h"
#include "rumbro_negra/arvore_rumbro_negra.h"

static const char erro404[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static char resposta101[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
const int time_aguardar = 20000;
volatile int flag = -1;
volatile int rodando = 1;
pthread_mutex_t block = PTHREAD_MUTEX_INITIALIZER;

extern int ws_port;
HASH clientes_threads;
Queue output;
Queue system_message;
RBtree situacao_atual;

struct cancel_thread{
    int fd;
    pthread_t fd_thread;
};

static void *liberaThread(void *arg)
{
    struct cancel_thread *cancel = (struct cancel_thread *)arg;
    pthread_mutex_lock(&block);
    if(cancel->fd >= 0){ 
        removeRbtree(&situacao_atual, cancel->fd);
        deleteHash(&clientes_threads, cancel->fd);
    }
    else pthread_join(cancel->fd_thread, NULL);
    free(cancel);
    pthread_mutex_unlock(&block);
    pthread_exit(NULL);
}

static char *string_gerada(char *key_start)
{
        key_start += 19;
        char client_key[64];
        int k = 0;
        memset(client_key, 0, sizeof(client_key));
        while(key_start[k] != '\r' && key_start[k] != '\n' && key_start[k] != ' ' && k < 32)
        {
            client_key[k] = key_start[k];
            k++;
        }
        client_key[k] = '\0';
        char chave_aceita[128];
        memset(chave_aceita, 0, sizeof(chave_aceita));
        calcular_chave_websocket(client_key, chave_aceita);
        char *resposta = malloc(strlen(resposta101) + strlen(chave_aceita) + 5);
        strcpy(resposta, resposta101);
        strcat(resposta, chave_aceita);
        strcat(resposta, "\r\n\r\n");
        return resposta;
}

static void enviar_mensagem_websocket(int client_fd, const char *mensagem) {
    if (client_fd <= 0) {
        return;
    }

    size_t len = strlen(mensagem);
    unsigned char frame[1024]; 
    int frame_len = 0;

    frame[0] = 0x81; 

    if (len <= 125) {
        frame[1] = len;
        frame_len = 2;
    } else if (len >= 126 && len <= 65535) {
        frame[1] = 126;
        frame[2] = (len >> 8) & 0xFF;
        frame[3] = len & 0xFF;
        frame_len = 4;;
    } else {
        return;
    }

    memcpy(frame + frame_len, mensagem, len);
    frame_len += len;
    write(client_fd, frame, frame_len);
}

void *websocket_write(void *arg){
    while(rodando){
        while(!empytQueue(&output)){
            struct Queuedata data = Dequeue(&output);
            if(data.socket_fd == 0){
                pthread_mutex_lock(&block);
                int *sockets_atual = inorder(&situacao_atual);
                unsigned long total = situacao_atual.size;
                pthread_mutex_unlock(&block);

                if(sockets_atual != NULL){
                    for(unsigned long i = 0; i < total; i++){
                        enviar_mensagem_websocket(sockets_atual[i], data.strig);
                    }
                    free(sockets_atual);
                }
            }else{
                enviar_mensagem_websocket(data.socket_fd, data.strig);
            }
         free(data.strig);
        }
        usleep(20000);
    }
    return NULL;
}

static void opcodesData(Transport *websocket, int opcode){
    int playload_len = 0;
    int mask_index =0;
    int data_index = 0;
    unsigned char mask[4];
    switch (opcode) {
        case 0x01:
            playload_len = websocket->buffer[1] & 0x7F;
            mask_index = 2;
            data_index = mask_index + 4;
            unsigned char mask[4];
            for(int i = 0; i < 4; i++) mask[i] = websocket->buffer[mask_index + i];
            char mensagem[1024];
            memset(mensagem, 0, sizeof(mensagem));
            for(int i = 0; i < playload_len; i++) mensagem[i] = websocket->buffer[data_index + i] ^ mask[i % 4];
            pthread_mutex_lock(&block);
            Enqueue(websocket->input, mensagem, websocket->connection_fd);
            pthread_mutex_unlock(&block);
        break;
    case 0x08:
        websocket->websocket_ative = 0;
        break;
    case 0x09:
        playload_len = websocket->buffer[1] & 0x7F;
        mask_index = 2;
        for(int i = 0; i < 4; i++) mask[i] = websocket->buffer[mask_index + i];
        int data_index = mask_index + 4;
        unsigned char payload[128]; 
        memset(payload, 0, sizeof(payload));

        for(int i = 0; i < playload_len; i++) payload[i] = websocket->buffer[data_index + i] ^ mask[i % 4];
        unsigned char pong_frame[130];
        pong_frame[0] = 0x8A; 
        pong_frame[1] = playload_len;                                     
        for(int i = 0; i < playload_len; i++) {
            pong_frame[2 + i] = payload[i];
        }

        write(websocket->connection_fd, pong_frame, 2 + playload_len);
        break;
        default:
            printf("mensagem\n");
        break;
}
}

static void *websocket_read(void *arg)
{
    Transport *websocket = (Transport *)arg;
    websocket->input = NULL;
    websocket->input = malloc(sizeof(Queue));
    if(websocket->input == NULL){
        websocket->websocket_ative = 0;
    }
    while (websocket->websocket_ative) 
    {
        newQueue(websocket->input);
        size_t bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer) - 1);
        if(bytes_lidos <= 0)
    {
        websocket->websocket_ative = 0;
        break;
    }
    char *key_start = strstr(websocket->buffer,"Sec-WebSocket-Key: ");
    if(key_start == NULL)
    {
        write(websocket->connection_fd, erro404, strlen(erro404));
//        printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Cliente encerrou a conexao na porta %d.\n", ws_port);
        websocket->websocket_ative = 0;
        break;
    }
    char *resposta = string_gerada(key_start);
    write(websocket->connection_fd, resposta, strlen(resposta));
    free(resposta);

    pthread_mutex_lock(&block);
    flag = websocket->connection_fd;
    pthread_mutex_unlock(&block);

//    printf("\033[32m[\033[1mWS\033[0m\033[32m]\033[0m: Conexao estabelecida com sucesso\n\n");
    while(websocket->websocket_ative){
        memset(websocket->buffer, 0,sizeof(websocket->buffer));
        int pronto_msg = aguardar_fd(websocket->connection_fd, time_aguardar);
        if(!empytQueue(websocket->input)){
            struct Queuedata saida = Dequeue(websocket->input);
            printf("mensagem: [ %s ], WS[ %d ]\n", saida.strig, saida.socket_fd);
            free(saida.strig);
        }
        if(rodando == 0){ 
            websocket->websocket_ative = 0;
            break;
        }
        if(pronto_msg < 0 && errno != EINTR){
            websocket->websocket_ative = 0;
            break;
        }else if(pronto_msg > 0){
            size_t bytes_lidos;
            if((bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer))) <= 0)
            {
                websocket->websocket_ative = 0;
                break;
            }
            int opcode = websocket->buffer[0] & 0x0F;
            opcodesData(websocket, opcode);
        }
    }
    break;
    }
    if(websocket->input != NULL ) {
        freeQueuen(websocket->input);
        free(websocket->input);
    }
    if(rodando){
        pthread_t delete_id;
        struct cancel_thread *cancel = malloc(sizeof(struct cancel_thread));
        if(cancel == NULL) {
            close(websocket->connection_fd);
            free(websocket);
            return NULL;
        }
        *cancel = (struct cancel_thread){0 ,0};
        *cancel = (struct cancel_thread){websocket->connection_fd, searchHash(&clientes_threads, websocket->connection_fd)};
        pthread_create(&delete_id, NULL, liberaThread,  (void *)cancel);
        pthread_detach(delete_id);
        
    }
//    printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Conexao perdida com o navegador websocket:[%d].\n", websocket->connection_fd);
//    removeRbtree(&situacao_atual, websocket->connection_fd);
    Enqueue(&system_message, "\033[1;31m Desconectado \033[0m", websocket->connection_fd);
    close(websocket->connection_fd);
    websocket->connection_fd = -1;
    free(websocket);
    return NULL;
}

static void *listar_conexoes(void *arg)
{
    unsigned long atividade = situacao_atual.size;
    while (rodando) {
        if(situacao_atual.size != atividade || !empytQueue(&system_message)){ 
            printf("\033[H\033[J");
            int *resultado = NULL;
            resultado = inorder(&situacao_atual);
            printf("\r\033[1;32m CONEXOES ATIVAS NO MOMENTO\033[0m\n");
            while (!empytQueue(&system_message)) {
                struct Queuedata  data = Dequeue(&system_message);
                printf("[WS : %d] : %s\n", data.socket_fd, data.strig);
                free(data.strig);
            }
            if(resultado != NULL) free(resultado);
            atividade = situacao_atual.size;
        }
        usleep(200000);
    }
    return NULL;
}

void *websocket_serve(void *arg)
{
    Transport *websocket = (Transport *)arg;
    pthread_t listar_id, output_mensage;

    inithash(&clientes_threads);
    newQueue(&output);
    newQueue(&system_message);
    newRBtree(&situacao_atual);
    
    pthread_create(&output_mensage, NULL, websocket_write , NULL);
    pthread_create(&listar_id, NULL, listar_conexoes , NULL);
    printf("Servidor Websocket Iniciado");
    if(websocket == NULL) return NULL;
    Transport *cliente = NULL;
    printf("iniciado com sucesso esperando um conexao na porta no enderço http://localhost:%d\n", ws_port);
    while (rodando) {
        int pronto = aguardar_fd(websocket->socket_fd , time_aguardar );
        if(pronto == 0) continue;
        if(pronto < 0 && errno != EINTR) continue;

        cliente = malloc(sizeof(Transport));
        if(cliente == NULL) continue;
        memcpy(cliente, websocket, sizeof(Transport));
        cliente->client_size = sizeof(cliente->client);

        cliente->connection_fd = accept(websocket->socket_fd, (struct sockaddr*)&cliente->client, &cliente->client_size);
        if(cliente->connection_fd < 0){
            free(cliente);
            continue;
        }
        int *alocar = malloc(sizeof(int));
        if(alocar != NULL){
            *alocar = cliente->connection_fd;
            insertRBtree(&situacao_atual, cliente->connection_fd, (void *)alocar);
            alocar = NULL;
        }
        Enqueue(&system_message, "\033[1;32m conectado \033[0m", cliente->connection_fd);
        memset(cliente->buffer, 0, sizeof(cliente->buffer));
        pthread_t websocket_id;
        cliente->websocket_ative = 1;
        pthread_mutex_lock(&block);
        
        if((pthread_create(&websocket_id, NULL, websocket_read, (void *)cliente)) != 0){
            close(cliente->connection_fd);
            cliente->connection_fd = -1;
            free(cliente);
            continue;
        }
        if(insertHash(&clientes_threads, cliente->connection_fd, websocket_id)){
            close(cliente->connection_fd);
            cliente->connection_fd = -1;
            cliente->websocket_ative = 0;
        }
        pthread_mutex_unlock(&block);
//        Enqueue(&output, "reload", 0);
    }

    printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Servidor Encerrando... %d.\n", ws_port);
    pthread_mutex_lock(&block);
    freehash(&clientes_threads);
    flag = -1;
    freeQueuen(&output);
    close(websocket->socket_fd);
    websocket->socket_fd = -1;
    freeRB(&situacao_atual);
    pthread_mutex_unlock(&block);
    pthread_join(listar_id, NULL);
    pthread_join(output_mensage, NULL);
    return NULL;
}

void calcular_chave_websocket(const char *key, char *output)
{
     char GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";     
     char buffer[256];                                         
     unsigned char sha1_result[SHA_DIGEST_LENGTH];             
                                                               
     memset(buffer, 0, sizeof(buffer));                        
     memset(sha1_result, 0, sizeof(sha1_result));              
     sprintf(buffer, "%s%s", key, GUID);                       
                                                               
     SHA1((unsigned char*)buffer, strlen(buffer), sha1_result);
     BIO *bio, *b64;                                           
     BUF_MEM *buffer_ptr;                                      
     b64 = BIO_new(BIO_f_base64());                            
     BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);               
     bio = BIO_new(BIO_s_mem());                               
     bio = BIO_push(b64, bio);                                 
     BIO_write(bio, sha1_result, SHA_DIGEST_LENGTH);           
     BIO_flush(bio);                                    
     bio = BIO_pop(b64);                                       
     BIO_get_mem_ptr(bio, &buffer_ptr);                        
     memcpy(output, buffer_ptr->data, buffer_ptr->length);     
     output[buffer_ptr->length] = '\0';                               
     BIO_free_all(bio);                                        
     BIO_free(b64);                                            
    }

static int aguardar_fd(int fd, long timeout_usec){
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    struct timeval timeout = {0, timeout_usec};
    return select(fd + 1, &readfds, NULL, NULL, &timeout);
}
