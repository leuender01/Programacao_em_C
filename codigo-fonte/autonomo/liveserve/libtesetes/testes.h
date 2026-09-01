#ifndef TESTES
#define TESTES
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELOW "\033[1;33m"
#define COLOR_RESET "\033[0m"

#define type_fmt(x) _Generic((x),         \
    unsigned char: "%c\n",                \
    char: "%c\n",                         \
    signed char: "%c\n",                  \
    short int: "%hd\n",                   \
    unsigned short int: "%hu\n",          \
    int: "%d\n",                          \
    unsigned int: "%u\n",                 \
    long int: "%ld\n",                    \
    unsigned long int: "%lu\n",           \
    long long int: "%lld\n",              \
    unsigned long long int: "%llu\n",     \
    float: "%f\n",                        \
    double: "%f\n",                       \
    long double: "%Lf\n",                 \
    char *: "%s\n",                       \
    void *: "%p\n",                       \
    default: "%p\n")

#define PRINT_FAIL(actual, expected) \
    do { \
        printf("%s:%d: " COLOR_RED "Failure\n" COLOR_RESET , __FILE__, __LINE__); \
        printf(COLOR_RED "RESULTADO: " COLOR_RESET);                                                                  \
        printf(type_fmt(actual), actual);                                         \
        printf(COLOR_GREEN "EXPECTATIVA: " COLOR_RESET);                                                                  \
        printf(type_fmt(expected), expected);                                     \
    } while (0)


#define TEST_CASE(_NAME_) static int _NAME_(void)
#define TESTE_PASS() \
    puts(COLOR_GREEN "PASSOU" COLOR_RESET);\
    return 0;

#define EXPECTED_EQ(_MESSAGE_, input, output) \
    puts(COLOR_YELOW _MESSAGE_ COLOR_RESET);\
    if(input != output) { \
        PRINT_FAIL(input, output); \
        return -1;\
    }

#define EXPECT_COND(_CONDITION_)\
    puts(COLOR_YELOW "TESTANDO SE A CONDIÇÃO " #_CONDITION_ " E VERDADEIRA" COLOR_RESET);\
    if(!(_CONDITION_)){ \
        PRINT_FAIL(_CONDITION_, 1); \
        return -1;\
    }
        

#define TEST_SUITE(...)  \
    int main(void) {  \
        int result = 0; \
        int (*const TESTS[])(void) = { __VA_ARGS__ }; \
        int size = sizeof(TESTS) / sizeof(*TESTS);\
        for (uint32_t index = 0; index < size; ++index) { \
            if (TESTS[index]() != 0) { \
                result--; \
            } \
            result++;\
        } \
        printf(COLOR_GREEN "CASES:PASSOU [%d/%d] \n" COLOR_RESET, size, result); \
        return (result != size); \
    }
#define MESSAGE(_MESSAGE_) puts(COLOR_YELOW " " _MESSAGE_ COLOR_RESET);

char *gerarStringAletaria(void);
int numerosAletaorios(int seed);

#endif
