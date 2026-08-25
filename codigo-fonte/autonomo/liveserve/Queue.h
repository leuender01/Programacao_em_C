#ifdef QUEUEN_H

struct Queuedata {
    int socket_fd;
    char *strig;
    struct Queuedata *prox;
};

typedef struct{
    struct Queuedata *tail;
    struct Queuedata *first;
    unsigned long int size;
} Queue;

int newQueue(Queue *p);
int empytQueue(Queue *p);
int Enqueue(Queue *p, const char *string, int socket_fd);
struct Queuedata Dequeue(Queue *p);
struct Queuedata peekQueue(Queue *p);
int freeQueuen(Queue *p);

#endif
