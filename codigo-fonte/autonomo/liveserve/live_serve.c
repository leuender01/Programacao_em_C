#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <pthread.h>
#include <errno.h>
#include "live_serve.h"

const char erro404[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
const char resposta101[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
const char script_injection[] = "<script>const ws = new WebSocket(\'ws:127.0.0.1:8082\');ws.onopen=()=>{console.log(\"Conectado ao WebSocket!\");};ws.onmessage = (event) => {if( event.data === \"reload\"){location.reload();};};ws.onerror=(error)=>{console.log(\"Erro no WebSocket\", error);}; </script>";
const int ws_port = 8082;
const int http_port = 8081;
const char ws_frame[8] = {0x81, 0x06, 'r','e','l','o','a','d'};
volatile int rodando = 1;
volatile int flag = -1;
const int time_aguardar = 20000;

pthread_mutex_t block;

pthread_t id_websocket;
pthread_t id_monitor;

int main(void)
{   
    signal(SIGINT, captura_signal);
    Transport websocket;
    Transport servidor;

    pthread_mutex_init(&block, NULL);
    servidor = Tcp("server", http_port);
    websocket = Tcp("client", ws_port);
    
    pthread_create(&id_monitor, NULL, notificacao, NULL); 
    pthread_create(&id_websocket, NULL, websocket_serve, &websocket);
    
    usleep(500000);

    while(rodando){
        int pronto = aguardar_fd(servidor.socket_fd, time_aguardar );
        if(pronto < 0){
            if(errno == EINTR) continue; 
            break;
        }
        if(pronto == 0) continue;

        printf("\033[1;37m[HTTP]\033[0m: Esperando uma conexao na porta %i...\n", http_port);
        servidor.connection_fd = accept(servidor.socket_fd, (struct sockaddr *)&servidor.client, &servidor.client_size);
        if (rodando <= 0){
            close(servidor.connection_fd);
            break;
       }
        printf("\033[1;37m[HTTP]\033[0m: houve uma requisição...\n");
        memset(servidor.buffer, 0, sizeof(servidor.buffer));
        read(servidor.connection_fd, servidor.buffer, sizeof(servidor.buffer));

        char *resposta = NULL;
        if(strstr(servidor.buffer, "GET /script")) resposta = ler_arquivo("script.js","text/javascript");
        else if(strstr(servidor.buffer, "GET /style.css")) resposta = ler_arquivo("style.css","text/css");
        else if(strstr(servidor.buffer, "GET /")) resposta = ler_arquivo("index.html","text/html");

        if(resposta != NULL)
        {
            write(servidor.connection_fd, resposta, strlen(resposta));
            free(resposta);
        }else{
            write(servidor.connection_fd, erro404, strlen(erro404));
        }
        close(servidor.connection_fd);
    }
    printf("\033[36m[\033[1mSISTEMA\033[0m\033[36m]\033[0m: Fechando e liberando recursos...\n");

    close(websocket.socket_fd);
    close(servidor.socket_fd);
    close(servidor.connection_fd);
    
    if(websocket.connection_fd >= 0) close(websocket.connection_fd);
    if(flag != -1) close(flag);
    pthread_mutex_destroy(&block);

    pthread_join(id_monitor, NULL);
    pthread_join(id_websocket, NULL);

    printf("\033[36m[\033[1mSISTEMA\033[0m\033[36m]\033[0m: Bye :)\n");
    return 0;
}

Transport Tcp(const char *tipo, int port)
{
    Transport new;
    
    new.client_size = sizeof(new.client);                                                         
    new.socket_fd = socket(AF_INET, SOCK_STREAM, 0);                                
                                                                                 
    if(new.socket_fd > 0) printf("\033[1;32m[WS]\033[0m Socket aberto:\n");                                
    else exit(EXIT_FAILURE);                                                                             

    int optar = 1;
    if (setsockopt(new.socket_fd, SOL_SOCKET, SO_REUSEADDR, &optar, sizeof(optar)) < 0) {
        perror("Erro ao definir SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    new.myserve.sin_family = AF_INET;                                               
    new.myserve.sin_port = htons(port);                                             
    new.myserve.sin_addr.s_addr = INADDR_ANY;                                       
    if(!strcmp(tipo, "server"))inet_aton(LOCAL_HOST, &(new.myserve.sin_addr));                                 
                                                                                
    printf("tentando abrira porta %i\n",port);                                  
                                                                                
    if(bind(new.socket_fd, (struct sockaddr*)&new.myserve, sizeof(new.myserve)) != 0)       
    {                                                                           
        printf("houve um problema em abrir a porta\n");                         
        exit(EXIT_FAILURE);                                                               
    }                                                                           
    printf("tipo:%s porta %i aberta\n",tipo, port);                                       
    listen(new.socket_fd, 3);                                                       

    return new;
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

void *websocket_serve(void *arg)
{
    Transport *websocket = (Transport *)arg;

    printf("Iniciando Websocket\n");
    if(websocket == NULL) exit(EXIT_FAILURE);
    printf("iniciado com sucesso esperando um conexao na porta %d\n", ws_port);
    
    while(rodando)
    {
        int pronto = aguardar_fd(websocket->socket_fd , time_aguardar );
        if(pronto < 0){
            if(errno == EINTR) continue; 
            break;
        }
        if(pronto == 0) continue;
            websocket->connection_fd = accept(websocket->socket_fd, (struct sockaddr*)&websocket->client, &websocket->client_size);
            memset(websocket->buffer, 0, sizeof(websocket->buffer));
            size_t bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer) - 1);
        if(bytes_lidos <= 0)
        {
            printf("erro na conexao\n");
            close(websocket->connection_fd);
        }else{
            char *key_start = strstr(websocket->buffer,"Sec-WebSocket-Key: ");
            if(key_start != NULL)
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
                write(websocket->connection_fd, resposta, strlen(resposta));
                free(resposta);

                pthread_mutex_lock(&block);
                flag = websocket->connection_fd;
                pthread_mutex_unlock(&block);

                printf("\033[32m[\033[1mWS\033[0m\033[32m]\033[0m: Conexao estabelecida com sucesso\n\n");
                memset(websocket->buffer, 0,sizeof(websocket->buffer));
                int pronto_msg = aguardar_fd(websocket->connection_fd, time_aguardar);
                if(pronto_msg < 0 && errno != EINTR){
                    printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Conexao perdida com o navegador.\n");
                    pthread_mutex_lock(&block);
                    if(flag == websocket->connection_fd) flag = -1;
                    pthread_mutex_unlock(&block);
                    close(websocket->connection_fd);
                }else if(pronto_msg > 0){
                    size_t bytes_lidos;
                    if((bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer))) > 0)
                    {
                        if((websocket->buffer[0] & 0x0F) == 0x01)
                        {
                            int playload_len = websocket->buffer[1] & 0x7F;
                            int mask_index = 2;
                            int data_index = mask_index + 4;
                            unsigned char mask[4];
                            for(int i = 0; i < 4; i++) mask[i] = websocket->buffer[mask_index + i];

                            char mensagem[1024];
                            memset(mensagem, 0, sizeof(mensagem));

                            for(int i = 0; i < playload_len; i++) mensagem[i] = websocket->buffer[data_index + i] ^ mask[i % 4];
                            mensagem[playload_len] = '\0';
                            printf("%s\n", mensagem);
                        }
                    }else{
                        printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Conexao perdida com o navegador.\n");
                        pthread_mutex_lock(&block);
                        if(flag == websocket->connection_fd) flag = -1;
                        pthread_mutex_unlock(&block);
                        close(websocket->connection_fd);
                    }
                }
                }else{
                write(websocket->connection_fd, erro404, strlen(erro404));
                printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Cliente encerrou a conexao na porta %d.\n", ws_port);
                close(websocket->connection_fd);
                break;

            }
        }
    }
    printf("\033[31m[\033[1mWS\033[0m\033[31m]\033[0m: Cliente encerrou a conexao na porta %d.\n", ws_port);
    close(websocket->connection_fd);
    pthread_mutex_lock(&block);
    flag = -1;
    rodando = 0;
    pthread_mutex_unlock(&block);
    return NULL;
}

char *ler_arquivo(const char* path, const char *type)
{
    int size= 0;
    char cabecalho[234], *buffer, reshtml[717], cofirm = '\0'; 
    FILE *fd = fopen(path, "rb");
    if(fd == NULL && !strcmp(path, "index.html"))
    {
        printf("\narquivo index.html nao existe, deseja criar[Y/n]?");
        scanf(" %c", &cofirm);
        if(cofirm == 'n' || cofirm == 'N') return NULL;
        memset(reshtml, 0, sizeof(reshtml));
        sprintf( reshtml,"<!DOCTYPE html>\n  <html lang=\"pt-BR\">\n     <head>\n        <meta charset=\"UTF-8\">\n      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n        <link rel=\"stylesheet\" href=\"style.css\">\n      <title>Título da Página</title>\n   <head>\n    <body>\n        <script>\n             const ws = new WebSocket('ws:127.0.0.1:%i');\n             ws.onopen = () =>{\n                   console.log(\"Conectado ao WebSocket!\");\n         };\n             ws.onmessage = (event) => {\n               if( event.data === \"reload\"){\n          location.reload();\n        };\n    };\n            ws.onerror = (error) =>{\n          console.log(\"Erro no WebSocket\", error);\n        };\n      </script>\n     </body>\n</html>", ws_port);
        reshtml[716] = '\0';
        fd = fopen(path, "wb");
        
        if(fd == NULL) return NULL;
        fwrite(reshtml, 1,strlen(reshtml),fd);

        buffer = (char *)malloc(strlen(reshtml) + 1);
        strcpy(buffer, reshtml);
        fclose(fd);
        size = strlen(reshtml);
    }else if(fd == NULL){
        return NULL;
    }else{

        fseek(fd, 0, SEEK_END);                                    
        long tamanho_bytes = ftell(fd);                            
        rewind(fd);                                                
        buffer = (char *)malloc(tamanho_bytes + 1);               

        if(buffer == NULL)                                              
        {                                                               
            perror("erro alocar memoria\n");                              
            fclose(fd);                                            
            return NULL;                                                                                                                    
        }                                                               
        size_t bytes_lidos = fread(buffer, 1, tamanho_bytes ,fd);  
        if(bytes_lidos != tamanho_bytes)                                
        {                                                               
            perror("Erro ao ler arquivo\n");                              
            free(buffer);                                               
            fclose(fd);                                           
            return NULL;                                                
        }                                                               
        fclose(fd);                                                
        buffer[tamanho_bytes] = '\0'; 
        size = tamanho_bytes;
        if(!strcmp(type,"text/html")){
        char *novo_buffer = malloc(size + strlen(script_injection) + 1);
        
        // Tenta encontrar a tag </body> para injetar o script logo antes dela
        char *body_tag = strstr(buffer, "</body>");
        
        if (body_tag) {
            int pos = body_tag - buffer;
            strncpy(novo_buffer, buffer, pos);
            novo_buffer[pos] = '\0';
            strcat(novo_buffer, script_injection);
            strcat(novo_buffer, body_tag); // Adiciona o </body> e o resto do HTML
        } else {
            strcpy(novo_buffer, buffer);
            strcat(novo_buffer, script_injection);
        }
        
        free(buffer); 
        buffer = novo_buffer; 
        size = strlen(buffer); 
        }
    }
    if(size < 3) return NULL; 
    sprintf(cabecalho,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: close\r\n\r\n", type, size); 
    char *resposta = malloc(strlen(cabecalho) + strlen(buffer) + 1);                                          
    strcpy(resposta, cabecalho);                                                                             
    strcat(resposta,buffer);                                                                               
    free(buffer);                                                                                         
    return resposta; 
}

void *notificacao(void *arg)
{                                                                                                                                  
    char buffer[BUFFER_LEN]; 
    int fd = inotify_init1(IN_NONBLOCK);
    if(fd < 0){
        perror("Erro ao inicializar inotify");
        return NULL;

    }
    inotify_add_watch(fd, ".", IN_MODIFY | IN_CLOSE_WRITE);

    while(rodando)
    {
        int pronto = aguardar_fd(fd , time_aguardar );
        if(pronto < 0){
            if(errno == EINTR) continue; 
            break;
        }
        if(pronto == 0) continue;
        if(rodando == 0) break;
        int lenght = read(fd, buffer, BUFFER_LEN);
        if(lenght <  0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                usleep(10000);
                continue;
            }
            perror("Erro de leitura");
            break;
        }
  
        int i = 0;
        while(i < lenght)
        {
            if(rodando == 0) break;
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if(event->len)
            {
                if (strcmp(event->name, "index.html") == 0 ||         
                    strcmp(event->name, "style.css") == 0 ||          
                    strcmp(event->name, "script.js") == 0)        
                {
                        printf("Arquivo modificado: %s\n", event->name);  
                        if(flag != -1)                        
                        {
                            if(write(flag, ws_frame, 8) < 0)
                            {
                                printf("\033[31m[\033[1mWS\033[0m]\003[0m Conexao perdida com o navegador.\n");
                                pthread_mutex_lock(&block);
                                close(flag);              
                                flag = -1;                
                                pthread_mutex_unlock(&block);
                                break;
                            }else{
                                printf("\033[31m[\033[1mWS\033[0m]\003[0m Comando 'reload' enviado com sucesso!\n");
                        }
                    }
                }
            }
            i+= EVENT_SIZE + event->len;
        }
    } 
    if(flag != -1) close(flag);
    close(fd);
    pthread_exit(NULL); 
}

void captura_signal(int sinal){
    printf("\033[36m[\033[1mSISTEMA\033[0m\033[36m]\033[0m: Encerrando servidor suavemente...\n");
    pthread_mutex_lock(&block);
    rodando = 0;
    pthread_mutex_unlock(&block);
}

static int aguardar_fd(int fd, long timeout_usec)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    struct timeval timeout = {0, timeout_usec};
    return select(fd + 1, &readfds, NULL, NULL, &timeout);
}
