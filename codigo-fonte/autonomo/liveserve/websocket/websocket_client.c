#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include "websocket_client.h"
#include "Queuen/Queue.h"

#define SERVE_IP "127.0.0.1"
#define PORT 8081
#define BUFFER_SIZE 1024

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static void base64_encode_16bytes(const unsigned char *input, char *output);
static void gerar_bytes_aleatorios(unsigned char *buffer);
static pthread_mutex_t block = PTHREAD_MUTEX_INITIALIZER;
//static void enviar_mensagem_ws(int fd, const char *texto);
//
//void enviar_mensagem_ws(int fd, const char *texto)
//{
//    
//
//
//}

extern Queue system_message_cliete;

static void base64_encode_16bytes(const unsigned char *input, char *output) {
    int i = 0, j = 0;

    while (i < 15) {
        unsigned int triple = (input[i] << 16) + (input[i+1] << 8) + input[i+2];
        i += 3;
        output[j++] = b64_table[(triple >> 18) & 0x3F];
        output[j++] = b64_table[(triple >> 12) & 0x3F];
        output[j++] = b64_table[(triple >> 6) & 0x3F];
        output[j++] = b64_table[triple & 0x3F];
    }

    unsigned int triple = (input[i] << 16);
    output[j++] = b64_table[(triple >> 18) & 0x3F];
    output[j++] = b64_table[(triple >> 12) & 0x3F];
    output[j++] = '=';
    output[j++] = '=';
    output[j] = '\0';
}


static void gerar_bytes_aleatorios(unsigned char *buffer) {
    for (int i = 0; i < 16; i++) {
        buffer[i] = rand() % 256; 
    }
}

static void opcodesData(int websocket, char buffer[1024] , int opcode, int temp){
    int playload_len = 0;
    int mask_index =0;
    int data_index = 0;
    unsigned char mask[4];
    switch (opcode) {
        case 0x01:{
            int mascarado = (buffer[1] & 0x80) != 0;
            playload_len = buffer[1] & 0x7F;
            data_index = 2;
            if(mascarado){
                mask_index = 2;
            for(int i = 0; i < 4; i++) mask[i] = buffer[mask_index + i];
                data_index = mask_index + 4;
                }
            char mensagem[1024];
            memset(mensagem, 0, sizeof(mensagem));
            for(int i = 0; i < playload_len; i++){
                mensagem[i] = mascarado ? (buffer[data_index + i] ^ mask[i % 4]) : buffer[data_index + i];
            }
            pthread_mutex_lock(&block);
            Enqueue(&system_message_cliete, mensagem, temp);
            pthread_mutex_unlock(&block);
            break;
              }
    case 0x08:{
        return;
        break;
        }
    case 0x09:{
        playload_len = buffer[1] & 0x7F;
        mask_index = 2;
        for(int i = 0; i < 4; i++) mask[i] = buffer[mask_index + i];
        int data_index = mask_index + 4;
        unsigned char payload[128]; 
        memset(payload, 0, sizeof(payload));

        for(int i = 0; i < playload_len; i++) payload[i] = buffer[data_index + i] ^ mask[i % 4];
        unsigned char pong_frame[130];
        pong_frame[0] = 0x8A; 
        pong_frame[1] = playload_len;                                     
        for(int i = 0; i < playload_len; i++) {
            pong_frame[2 + i] = payload[i];
        }

        write(websocket, pong_frame, 2 + playload_len);
        break;
        default:
            printf("mensagem\n");
        break;
          }
    }
}
            
void* websocket_cliente(void *arg)
{
    int soocket;
    struct sockaddr_in serve;
    char buffer[BUFFER_SIZE] = {0};
    srand(time(NULL));
    unsigned int esperar = 1000000;
    
    //##CAMADA DE TRANSPORTE
    
    if((soocket = socket(AF_INET, SOCK_STREAM, 0)) <0)
    {
//        perror("Erro ao criar socket");
        return (void *)(intptr_t) -1;
    };  
    
    serve.sin_family = AF_INET;
    serve.sin_port = htons(PORT);

    if(inet_pton(AF_INET, SERVE_IP, &serve.sin_addr) <= 0)
    {
//        perror("Endereço invalido");
        return (void *)(intptr_t) -1;
    }
    
    if(connect(soocket, (struct sockaddr *)&serve, sizeof(serve)) < 0)
    {
//        perror("Falha na conexão");
        return (void *)(intptr_t) -1;
    }

//    printf("Conectado iniciando o Handshake\n");

    //##CAMANDA DE APLICAÇÃO
    unsigned char bytes_aleatorios[16];
    char chave_base64[25];
    
    gerar_bytes_aleatorios(bytes_aleatorios);
    base64_encode_16bytes(bytes_aleatorios, chave_base64);
    
//    printf("[Cliente] Chave aleatória gerada para esta sessão: %s\n", chave_base64);    

    char handS[1024];
    sprintf(handS,    
            "GET / HTTP/1.1\r\n"
            "Host: %s:%d\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Key: %s\r\n"
            "Sec-WebSocket-Version: 13\r\n\r\n", SERVE_IP, PORT, chave_base64
            );
    write(soocket, handS, strlen(handS));
    
    read(soocket, buffer, BUFFER_SIZE);
//    printf("%s\n", buffer);   

    if(strstr(buffer, "101 Switching Protocols") == NULL)
    {
//        printf("[Cliente] Falha no Handshake do WebSocket. Resposta do servidor:\n%s\n", buffer);
        close(soocket);
        return (void *)(intptr_t) -1;
    }
    memset(buffer, 0, BUFFER_SIZE);
    clock_t inicio = clock();

    read(soocket, buffer, BUFFER_SIZE);
    clock_t fim = clock();
    int opcode = buffer[0] & 0x0F;
    double tempo_segundos = (double)(fim - inicio) / CLOCKS_PER_SEC;
    int temp = (int)(tempo_segundos * 1000);
    opcodesData(soocket, buffer, opcode, temp);
    int kill=  0x80;
    write(soocket, &kill, 1);
    usleep((unsigned int )(rand() % esperar));

    close(soocket);
//    printf("[Cliente] Conexao encerrada.\n");
    return (void *)(intptr_t) 0;
}
