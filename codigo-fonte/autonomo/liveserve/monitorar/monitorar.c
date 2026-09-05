#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <unistd.h>
#include <pthread.h>
#include "Queuen/Queue.h"
//#include <time.h>
//#include "libtesetes/testes.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN (1024 * (EVENT_SIZE + 16))

extern Queue output;
extern volatile int rodando;
extern pthread_mutex_t block;
static const int time_aguardar = 50000;

static int aguarda_fd(int fd)
{
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(fd, &readfs);
    struct timeval timeout = {0, time_aguardar};
    return select(fd + 1, &readfs, NULL, NULL, &timeout);
}

void *monitorar(void *arg)
{
    char buffer[BUFFER_LEN];
    int fd = inotify_init1(IN_NONBLOCK);
    if(fd < 0) return (void *)(intptr_t) -1;
    inotify_add_watch(fd, ".", IN_MODIFY | IN_CLOSE_WRITE);
    while(rodando)
    {
        int pronto = aguarda_fd(fd);
        if(pronto < 0 && errno == EINTR) continue;
        if(pronto < 0 || rodando == 0) break;
        if(pronto == 0) continue;
        int lenght = read(fd, buffer, BUFFER_LEN);
        if(lenght < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                usleep(10000);
                continue;
            }
            break;
        }
        int i = 0;
        while (i < lenght) 
        {
            if(rodando == 0) break;
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if(event->len)
            {
                if(
                    strstr(event->name, ".js") != NULL ||
                    strstr(event->name, ".css") != NULL ||
                    strstr(event->name, ".html") != NULL
                    )
                {
                    printf("%s\n", event->name);
                    pthread_mutex_lock(&block);
                    Enqueue(&output,"reload", 0);
                    pthread_mutex_unlock(&block);
                }

            }
            i += EVENT_SIZE + event->len;
        }
    }
    close(fd);
    return (void *)(intptr_t) 0;
}
