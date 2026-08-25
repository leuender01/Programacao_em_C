#ifdef QUEUEN_H

struct node {
    int socket_fd;
    char *strig;
    struct node *prox;
};

typedef struct{
    struct node *tail;
    struct node *first;
    unsigned long int size;
} Queue;

int newQueue(Queue *p);
int empytQueue(Queue *p);
int Enqueue(Queue *p, const char *string, int socket_fd);
struct node Dequeue(Queue *p);
struct node peekQueue(Queue *p);
int freeQueuen(Queue *p);

#endif
