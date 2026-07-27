#ifndef ARRAY_DINAMICO
#define ARRAY_DINAMICO

typedef enum {
    INTEIRO,
    FLOAT,
    STRING,
    UINT16
}Types;

struct Node {
    void *data;
    Types type;
};

typedef struct {
    struct Node *no;
    int length;
    int max;
}ArrayS;

void freearray(ArrayS *array);
void list_type(ArrayS *array, Types type);
void *recuperar_dado(ArrayS *array, int index);
void add_data(ArrayS *array, Types type, void *data);

#endif
