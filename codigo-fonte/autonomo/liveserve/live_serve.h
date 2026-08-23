#ifndef LIVE_SERVE 
#define LIVE_SERVE


#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN (1024 * (EVENT_SIZE + 16))
#define LOCAL_HOST "0.0.0.0"

typedef struct{
    int socket_fd, connection_fd;  
    socklen_t client_size;
    struct sockaddr_in myserve, client;
    char buffer[1024];
} Transport;             

static int aguardar_fd(int fd, long timeout_usec);
Transport Tcp(const char *tipo, int port);
void calcular_chave_websocket(const char *key, char *output);
void *websocket_serve(void *arg);
char *ler_arquivo(const char* path, const char *type);
void *notificacao(void *arg);
void captura_signal(int sinal);

#endif
