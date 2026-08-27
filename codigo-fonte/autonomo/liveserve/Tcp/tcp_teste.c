#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Tcp.h"

int main(void){
    Transport teste = Tcp("server", 8080);
    Transport websocket = Tcp("client", 8081);
    close(teste.connection_fd);
    close(teste.socket_fd);
    close(websocket.connection_fd);
    close(websocket.socket_fd);

    return 0;
}
