#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include "live_serve.h"
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#define HASH_H
#include "hash.h"

const char erro404[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
const char resposta101[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
extern volatile int rodando;
extern int ws_port;
extern int time_aguardar;
extern int flag;
extern pthread_mutex_t block;

HASH clientes_threads;
static pthread_mutex_t clientes_mutex = PTHREAD_MUTEX_INITIALIZER;

char *string_gerada(char *key_start)
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
        printf("\033[32m[\033[1mWS\033[0m\033[32m]\033[0m Chave recebida do Navegador: '%s'\n", client_key);

        char chave_aceita[128];
        memset(chave_aceita, 0, sizeof(chave_aceita));
        calcular_chave_websocket(client_key, chave_aceita);

        printf("\033[32m[\033[1mWS\033[0m\033[32m]\033[0m: Chave Base64 gerada: '%s'\n", chave_aceita);
        char *resposta = malloc(strlen(resposta101) + strlen(chave_aceita) + 5);
        strcpy(resposta, resposta101);
        strcat(resposta, chave_aceita);
        strcat(resposta, "\r\n\r\n");
        return resposta;
}

void enviar_mensagem_websocket(int client_fd, const char *mensagem) {
    if (client_fd <= 0) {
        printf("Nenhum cliente conectado para enviar mensagem.\n");
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
        frame_len = 4;
    } else {
        printf("Mensagem grande demais para este exemplo simplificado.\n");
        return;
    }

    memcpy(frame + frame_len, mensagem, len);
    frame_len += len;
    write(client_fd, frame, frame_len);
}

void *websocket_write(void *arg){
    Transport *websocket = (Transport *)arg;
    char chat[1024];
    while (websocket->websocket_ative){
        int resultado = aguardar_fd(STDERR_FILENO, time_aguardar);
        if(resultado > 0) fgets(chat, sizeof(chat), stdin);
        else continue;
        enviar_mensagem_websocket(websocket->connection_fd, chat);
    }
    return NULL;
}

void opcodesData(Transport *websocket, int opcode){
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
            printf("mensagem %s\n", mensagem);
            break;
        case 0x08:
            printf("Navegador solicitou fechamento\n");
            pthread_mutex_lock(&block);
            deleteHash(&clientes_threads ,websocket->connection_fd);
            pthread_mutex_unlock(&block);
            websocket->websocket_ative = 0;
            close(websocket->connection_fd);
            break;
        case 0x09:
            playload_len = websocket->buffer[1] & 0x7F;
            mask_index = 2;
            for(int i = 0; i < 4; i++) mask[i] = websocket->buffer[mask_index + i];
            int data_index = mask_index + 4;
            unsigned char payload[128]; 
            memset(payload, 0, sizeof(payload));

            for(int i = 0; i < playload_len; i++) payload[i] = websocket->buffer[data_index + i] ^ mask[i % 4];

            printf("\033[33m[\033[1mWS\033[0m\033[33m]\033[0m: Ping recebido, enviando Pong...\n");

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

void *websocket_read(void *arg)
{
    Transport *websocket = (Transport *)arg;
    while (websocket->websocket_ative) 
    {
        size_t bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer) - 1);
        if(bytes_lidos <= 0)
        {
            printf("erro na conexao\n");
            close(websocket->connection_fd);
            continue;
        }
        char *key_start = strstr(websocket->buffer,"Sec-WebSocket-Key: ");
        if(key_start == NULL)
        {
            write(websocket->connection_fd, erro404, strlen(erro404));
            printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Cliente encerrou a conexao na porta %d.\n", ws_port);
            pthread_mutex_lock(&block);
            deleteHash(&clientes_threads ,websocket->connection_fd);
            pthread_mutex_unlock(&block);
            close(websocket->connection_fd);
            free(websocket);
            return NULL;
        }
        char *resposta = string_gerada(key_start);
        write(websocket->connection_fd, resposta, strlen(resposta));
        free(resposta);

        pthread_mutex_lock(&block);
        flag = websocket->connection_fd;
        pthread_mutex_unlock(&block);

        printf("\033[32m[\033[1mWS\033[0m\033[32m]\033[0m: Conexao estabelecida com sucesso\n\n");
        while(websocket->websocket_ative){
            memset(websocket->buffer, 0,sizeof(websocket->buffer));
            int pronto_msg = aguardar_fd(websocket->connection_fd, time_aguardar);
            if(rodando == 0){ 
                websocket->websocket_ative = 0;
                break;
            }
            if(pronto_msg < 0 && errno != EINTR){
                printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Conexao perdida com o navegador.\n");
                pthread_mutex_lock(&block);
                if(flag == websocket->connection_fd) flag = -1;
                deleteHash(&clientes_threads ,websocket->connection_fd);
                pthread_mutex_unlock(&block);
                close(websocket->connection_fd);
            }else if(pronto_msg > 0){
                size_t bytes_lidos;
                if((bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer))) <= 0)
                {
                    printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Conexao perdida com o navegador.\n");
                    pthread_mutex_lock(&block);
                    if(flag == websocket->connection_fd) flag = -1;
                    deleteHash(&clientes_threads ,websocket->connection_fd);
                    pthread_mutex_unlock(&block);
                    websocket->websocket_ative = 0;
                    close(websocket->connection_fd);
                    continue;
                }
                int opcode = websocket->buffer[0] & 0x0F;
                opcodesData(websocket, opcode);
            }
        }
        pthread_mutex_lock(&block);
        deleteHash(&clientes_threads ,websocket->connection_fd);
        pthread_mutex_unlock(&block);
        close(websocket->connection_fd);
        free(websocket);
        return NULL;
    }
    pthread_mutex_lock(&block);
    deleteHash(&clientes_threads ,websocket->connection_fd);
    pthread_mutex_unlock(&block);
    close(websocket->connection_fd);
    free(websocket);
    return NULL;
}

void *websocket_serve(void *arg)
{
    Transport *websocket = (Transport *)arg;
    inithash(&clientes_threads);
    printf("Iniciando Websocket\n");
    if(websocket == NULL) exit(EXIT_FAILURE);
    Transport *cliente = NULL;
    while (rodando) {
        printf("iniciado com sucesso esperando um conexao na porta no enderço http://localhost:%d\n", ws_port);
        int pronto = aguardar_fd(websocket->socket_fd , time_aguardar );
        if(pronto == 0) continue;
        if(pronto < 0 && errno != EINTR) continue;
        printf("atividade no servidor\n");

        cliente = malloc(sizeof(Transport));
        if(cliente == NULL) continue;
        memcpy(cliente, websocket, sizeof(Transport));
        cliente->client_size = sizeof(cliente->client);

        cliente->connection_fd = accept(websocket->socket_fd, (struct sockaddr*)&websocket->client, &websocket->client_size);
        if(cliente->connection_fd < 0){
            free(cliente);
            continue;
        }
        memset(cliente->buffer, 0, sizeof(cliente->buffer));
        pthread_t websocket_id;
        cliente->websocket_ative = 1;
        pthread_mutex_lock(&clientes_mutex);
        
        if((pthread_create(&websocket_id, NULL, websocket_read, (void *)cliente)) == -1){
            close(cliente->connection_fd);
            free(cliente);
            continue;
        };
        if(insertHash(&clientes_threads, cliente->connection_fd, websocket_id)){
            close(cliente->connection_fd);
            cliente->websocket_ative = 0;
        };
        pthread_mutex_unlock(&clientes_mutex);
    }

    printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Cliente encerrou a conexao na porta %d.\n", ws_port);
    pthread_mutex_lock(&block);
    flag = -1;
    pthread_mutex_unlock(&block);

    pthread_mutex_lock(&clientes_mutex);
    close(websocket->socket_fd);
    freehash(&clientes_threads);
    pthread_mutex_unlock(&clientes_mutex);
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
