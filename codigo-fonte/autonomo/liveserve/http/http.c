#include "libtesetes/testes.h"
#include "http.h"
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

#ifndef LOCAL_HOST
#define LOCAL_HOST "0.0.0.0"
#endif
int ht_port = 8082;

static const char script_injection[] = "<script>const ws = new WebSocket(\'ws:127.0.0.1:8082\');ws.onopen=()=>{console.log(\"Conectado ao WebSocket!\");};ws.onmessage = (event) => {if( event.data === \"reload\"){location.reload();};};ws.onerror=(error)=>{console.log(\"Erro no WebSocket\", error);}; </script>";
static const char erro404[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
volatile int rodando = 1;
const int time_aguardar = 20000;

int aguardar_fd(int fd, long timeout_usec){
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    struct timeval timeout = {0, timeout_usec};
    return select(fd + 1, &readfds, NULL, NULL, &timeout);
}

int freelistaArquivos(struct listaArquivos *p)
{
    if(p == NULL) return -1;
    if(p->string == NULL) return -1;
    for(int i = 0; i < p->legth; i++){
        if(p->string[i] != NULL) free(p->string[i]);
    }
    free(p->string);
    p->string = NULL;
    p->legth = 0;
    return 0;
}

int arquivos_atuais(const char *extencao, struct listaArquivos *b)
{
    freelistaArquivos(b);
    DIR *d = opendir(".");
    if(d == NULL) return -1;
    struct dirent *dir;
    unsigned long contador = 0;
    char **arraystring = NULL;
    while ((dir = readdir(d)) != NULL) {
        char *string = NULL;
        if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0 && strstr(dir->d_name, extencao) != NULL ){
            string = strdup(dir->d_name);
            if(string == NULL) break;
            if(arraystring != NULL)
            {
                contador++;
                char **ptr = realloc(arraystring, sizeof(char *) * contador );
                if(ptr == NULL) break;
                ptr[contador - 1] = string;  
                arraystring = ptr;
                string = NULL;
                continue;
            }
            arraystring = malloc(sizeof(char *));
            arraystring[contador] = string;
//            printf("%s\n", dir->d_name);
            contador++;
            string = NULL;
        }
    }
    closedir(d);
    
    b->string =  arraystring;
    b->legth = contador;
    return 0;
}



/*
static char *ler_arquivo(const char* path, const char *type)
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
        sprintf( reshtml,"<!DOCTYPE html>\n  <html lang=\"pt-BR\">\n     <head>\n        <meta charset=\"UTF-8\">\n      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n        <link rel=\"stylesheet\" href=\"style.css\">\n      <title>Título da Página</title>\n   <head>\n    <body>\n  </body>\n</html>");
        reshtml[716] = '\0';
        fd = fopen(path, "wb");
        
        if(fd == NULL) return NULL;
        fwrite(reshtml, 1,strlen(reshtml),fd);
        buffer = (char *)malloc(strlen(reshtml) + strlen(script_injection) + 1);
        strcpy(buffer, reshtml);
        fclose(fd);
        size = strlen(reshtml);
    }else if(fd == NULL){
        return NULL;
    }else{
        fseek(fd, 0, SEEK_END);                                    
        long tamanho_bytes = ftell(fd);                            
        rewind(fd);                                                
        buffer = (char *)malloc(tamanho_bytes + strlen(script_injection) + 1);               
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
        char *body_tag = strstr(buffer, "</body>");
        if (body_tag) {
            int pos = body_tag - buffer;
            strcat(buffer, script_injection);
            strcat(buffer, body_tag);
        } else {
            strcpy(buffer, buffer);
            strcat(buffer, script_injection);
        }
        
        size = strlen(buffer); 
        }
    }
    if(size < 3) return NULL; 
    sprintf(
    cabecalho,
    "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: close\r\n\r\n",
    "text/html",
    size); 
    char *resposta = malloc(strlen(cabecalho) + strlen(buffer) + 1);                                          
    strcpy(resposta, cabecalho);                                                                             
    strcat(resposta,buffer);                                                                               
    free(buffer);                                                                                         
    return resposta; 
}
*/

char *file_html(char* path){
    FILE *fd = NULL;
    int size= 0;
    char cabecalho[234], *buffer, reshtml[717], cofirm = '\0'; 
    struct listaArquivos files = {NULL, 0};
    (path != NULL) ? arquivos_atuais(path, &files) : arquivos_atuais(".html", &files);
    if(files.legth == 0)
    {
        printf("\narquivo %s nao existe, deseja criar[Y/n]?", (path != NULL) ? path : "index.html" );
        scanf(" %c", &cofirm);
        if(cofirm == 'n' || cofirm == 'N') return NULL;
        memset(reshtml, 0, sizeof(reshtml));
        sprintf( reshtml,"<!DOCTYPE html>\n  <html lang=\"pt-BR\">\n     <head>\n        <meta charset=\"UTF-8\">\n      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n        <link rel=\"stylesheet\" href=\"style.css\">\n      <title>Título da Página</title>\n   <head>\n    <body>\n  </body>\n</html>");
        reshtml[716] = '\0';
        fd = (path != NULL ) ? fopen(path, "wb") : fopen("index.html", "wb");
        if(fd == NULL) return NULL;
        fwrite(reshtml, 1,strlen(reshtml),fd);

        buffer = (char *)malloc(sizeof(reshtml) +  strlen(script_injection) + 1);
        strcpy(buffer, reshtml);
        fclose(fd);
        size = strlen(reshtml);
    }else{
        if(files.legth == 1) fd = fopen(files.string[0], "rb") ;
        else if(path != NULL && files.legth > 1){
            for(int i = 0; i < files.legth; i++)
            {
                if(!strcmp(path, files.string[i]))
                {
                    fd = fopen(path, "rb");
                }
            }
        }
        if(fd == NULL)
        {
            freelistaArquivos(&files);
            return NULL;
        }
        fseek(fd, 0, SEEK_END);
        long  tamanho_bytes = ftell(fd);                            
        rewind(fd);                                                
        buffer = (char *)malloc(tamanho_bytes + strlen(script_injection) +  strlen("\n</body> \n</html>") + 1);               
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
            freelistaArquivos(&files);
            return NULL;                                                
        }                                                               
        fclose(fd);                                                
        buffer[tamanho_bytes] = '\0'; 
        size = tamanho_bytes;
    }
    const char *remove = "</body>";
    char *body_tag = strstr(buffer, remove);
    if (body_tag) {
        memmove(body_tag, body_tag + strlen(remove), strlen(body_tag + strlen(remove)) + 1);
        char *html_tag = strstr(buffer, "</html>");
        if(html_tag != NULL) memmove(html_tag, html_tag + strlen("</html>"), strlen(html_tag + strlen("</html>")) + 1);
        strcat(buffer, script_injection);
        strcat(buffer, "\n   </body>\n </html>");
    } else {
        strcpy(buffer, buffer);
        strcat(buffer, script_injection);
    }
    size = strlen(buffer); 
    if(size < 3) return NULL; 
    sprintf(cabecalho,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %i\r\nConnection: close\r\n\r\n", size); 
    char *resposta = malloc(strlen(cabecalho) + strlen(buffer) + 3);                                          
    strcpy(resposta, cabecalho);                                                                             
    strcat(resposta,buffer);                                                                               
    free(buffer);                                                                                         
    freelistaArquivos(&files);
    return resposta; 
}

char *file_JsStyle(const char *path){
    if(path == NULL ) return NULL;
    FILE *fd = NULL;
    int size= 0;
    char cabecalho[234], *buffer, reshtml[717], cofirm = '\0'; 
    struct listaArquivos files = {NULL, 0};
    arquivos_atuais(path, &files);

    if(files.legth == 0){
        freelistaArquivos(&files);
        return NULL;
    }
    
    if(files.legth == 1) fd = fopen(files.string[0], "rb") ;
    else if(path != NULL && files.legth > 1){
        for(int i = 0; i < files.legth; i++)
        {
            if(!strcmp(path, files.string[i]))
            {
                fd = fopen(path, "rb");
            }
        }
    }
    if(fd == NULL)
    {
        freelistaArquivos(&files);
        return NULL;
    }
    fseek(fd, 0, SEEK_END);
    long  tamanho_bytes = ftell(fd);                            
    rewind(fd);                                                
    buffer = (char *)malloc(tamanho_bytes  + 1);               
    if(buffer == NULL)                                              
    {                                                               
        perror("erro alocar memoria\n");                              
        freelistaArquivos(&files);
        fclose(fd);                                            
        return NULL;                                                                                                                    
    }                                                               
    size_t bytes_lidos = fread(buffer, 1, tamanho_bytes ,fd);  
    if(bytes_lidos != tamanho_bytes)                                
    {                                                               
        perror("Erro ao ler arquivo\n");                              
        free(buffer);                                               
        fclose(fd);                                           
        freelistaArquivos(&files);
        return NULL;                                                
    }                                                               
    fclose(fd);                                                
    buffer[tamanho_bytes] = '\0'; 
    size = tamanho_bytes;
    size = strlen(buffer); 
    if(size < 3) return NULL; 
    sprintf(cabecalho,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: text/%i\r\nConnection: close\r\n\r\n", (strstr(path, ".js") != NULL) ? "javascript" : "css", size); 
    char *resposta = malloc(strlen(cabecalho) + strlen(buffer) + 3);                                          
    strcpy(resposta, cabecalho);                                                                             
    strcat(resposta,buffer);                                                                               
    free(buffer);                                                                                         
    freelistaArquivos(&files);
    return resposta;
}

static void *conectionHttp(void *arg)
{
    CLOCK_INIT
//    FILE *log_fd = fopen("http.log" , "a");
    int *cliet = (int *)arg;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    read(*cliet, buffer, sizeof(buffer));
    char *resposta = NULL;
    if(strstr(buffer, "GET /") != NULL && strstr(buffer, ".js") != NULL) resposta = file_JsStyle("script.js");
    else if(strstr(buffer, "GET /") && strstr(buffer, ".css") != NULL) resposta = file_JsStyle("style.css");
    else if(strstr(buffer, "GET / ") != NULL) resposta = file_html("index.html");
    if(resposta != NULL)
    {
        write(*cliet, resposta, strlen(resposta));
        free(resposta);
    }else{
        write(*cliet, erro404, strlen(erro404));
    }
//    fprintf(log_fd,"%s\n",buffer);
//    fclose(log_fd);
    close(*cliet);
    free(cliet);
    CLOCK_FINISH
    return NULL;
}

static struct serverHTTP initServ(int port){
    struct serverHTTP new= {0};
    new.client_size = sizeof(struct sockaddr_in);
    new.fd = socket(AF_INET, SOCK_STREAM, 0);
    if(new.fd > 0) printf("\033[1;32mSocket aberto\033[0m\n");
    else return (struct serverHTTP){0};
    int optar = 1;
    if(setsockopt(new.fd, SOL_SOCKET, SO_REUSEADDR, &optar, sizeof(optar)) < 0) return (struct serverHTTP){0};
    new.myserver.sin_family = AF_INET;
    new.myserver.sin_port = htons(port);
    new.myserver.sin_addr.s_addr = INADDR_ANY;
    inet_aton(LOCAL_HOST, &(new.myserver.sin_addr));
    if(bind(new.fd, (struct sockaddr*)&new.myserver, sizeof(new.myserver)) != 0)
    {
        return (struct serverHTTP){0};
    }
    listen(new.fd, 20);
    return new;
}

void *http_server(void *arg)
{
    struct serverHTTP server =  initServ(ht_port);
    printf("\033[1;32mEndereço do server \033[0m < http://localhost:8082 >\n");
    while (rodando) {
        int esperar = aguardar_fd(server.fd, time_aguardar);   
        if(esperar < 0){
            if(errno == EINTR) continue;
            break;
        }
        if(esperar == 0) continue;
        int *client = malloc(sizeof(int));
        if(client == NULL) continue;
        *client = accept(server.fd, (struct sockaddr  *)&server.client, &server.client_size);
        if(rodando == 0)
        {
            close(server.fd);
            if(client != NULL) free(client);
            break;
        }
        pthread_t cliet_id;
        pthread_create(&cliet_id,NULL, conectionHttp, (void *)client);
        pthread_detach(cliet_id);
        client = NULL;
    }
    return (void *)(intptr_t) 0;
}
