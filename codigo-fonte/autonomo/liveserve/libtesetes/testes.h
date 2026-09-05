#ifndef TESTES
#define TESTES
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

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


#define type_sumary(x) _Generic((x),         \
    unsigned char: "%c",                \
    char: "%c",                         \
    signed char: "%c",                  \
    short int: "%hd",                   \
    unsigned short int: "%hu",          \
    int: "%d",                          \
    unsigned int: "%u",                 \
    long int: "%ld",                    \
    unsigned long int: "%lu",           \
    long long int: "%lld",              \
    unsigned long long int: "%llu",     \
    float: "%f",                        \
    double: "%f",                       \
    long double: "%Lf",                 \
    char *: "%s",                       \
    void *: "%p",                       \
    default: "%p")


#define PRINT_FAIL(actual, expected) \
    do {                                                                          \
        printf("%s:%d: " COLOR_RED "Failure\n" COLOR_RESET , __FILE__, __LINE__); \
            printf(COLOR_RED "RESULTADO: " COLOR_RESET);                              \
            printf(type_fmt(actual), actual);                                         \
            printf(COLOR_GREEN "EXPECTATIVA: " COLOR_RESET);                          \
            printf(type_fmt(expected), expected);                                     \
        } while (0)


#define TEST_CASE(_NAME_) void * _NAME_(void *arg)
#define TESTE_PASS()                                \
        puts(COLOR_GREEN "PASSOU" COLOR_RESET);         \
        return (void *)(intptr_t) 0;

#define EXPECTED_EQ(_MESSAGE_, input, output)               \
    if(input != output) {                                   \
        puts(COLOR_RED "ERRO: " _MESSAGE_ COLOR_RESET);     \
        PRINT_FAIL(input, output);                          \
        return (void *)(intptr_t) -1;                                      \
    }

#define EXPECT_COND(_CONDITION_)                            \
    if(!(_CONDITION_)){                                     \
        PRINT_FAIL(_CONDITION_, 1);                         \
        void  *RETURN;                                      \
        return (void *)(intptr_t) -1;                                      \
    }
        

#define TEST_SUITE(...)                                     \
    int main(void) {                                        \
        void* (*const TESTS[])(void *arg) = { __VA_ARGS__ };       \
        int size = sizeof(TESTS) / sizeof(*TESTS);\
        pthread_t *arrayId = malloc(sizeof(pthread_t) * size);\
        for (uint32_t index = 0; index < size; ++index) {   \
            if(pthread_create(&arrayId[index], NULL, TESTS[index], NULL)) { \
                fprintf(stderr, COLOR_RED "Erro ao iniciar thread %d\n" COLOR_RESET, index); \
            }                                                                               \
        }                                                   \
        int passed_count = 0;                                                       \
        for (uint32_t index = 0; index < size; ++index) {                           \
            void *thread_result;                                                    \
            pthread_join(arrayId[index], &thread_result);                           \
            if((intptr_t)thread_result == 0) passed_count++;                         \
        }                                                                           \
        printf(COLOR_GREEN "CASES:PASSOU [%d/%d] \n" COLOR_RESET, size, passed_count);    \
        free(arrayId);                                                              \
        return (passed_count != size);                                              \
    }


#define TEST_SUITE_ISOLATE(...)                                     \
    int main(void) {                                        \
        void* (*const TESTS[])(void *arg) = { __VA_ARGS__ };       \
        int size = sizeof(TESTS) / sizeof(*TESTS);\
        pid_t *pids = malloc(sizeof(pid_t) * size);\
        for (int index = 0; index < size; ++index) {   \
            pid_t pid = fork();                          \
            if(pid < 0) { \
                fprintf(stderr, COLOR_RED "Erro ao executar fork para o teste %d\n" COLOR_RESET, index); \
            }else if(pid == 0){ \
                void *res = TESTS[index](NULL);\
                free(pids);\
                exit((intptr_t)res != 0);\
            } else { \
                pids[index] = pid;\
                }                                                   \
        }\
        int passed_count = 0;                                                       \
        for (uint32_t index = 0; index < size; ++index) {                           \
            int status = 0;                                                         \
            waitpid(pids[index], &status, 0);                                       \
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {                    \
                passed_count++;                                     \
            }                                                        \
        }                                                                           \
        printf(COLOR_GREEN "CASES:PASSOU [%d/%d] \n" COLOR_RESET, size, passed_count);    \
        free(pids);                                                              \
        return (passed_count != size);                                              \
        }

#define MESSAGE(_MESSAGE_) puts(COLOR_YELOW " " _MESSAGE_ COLOR_RESET);

#define SUMARY(resultado, base)                             \
    printf(COLOR_YELOW "RESULTADO: ");                      \
    printf(type_sumary(resultado), resultado);              \
    printf(" | ") ;                                         \
    printf(type_sumary(base), base);                        \
    printf(" \n" COLOR_RESET);

#define CLOCK_INIT clock_t inicio = clock();
#define CLOCK_FINISH clock_t fim = clock(); \
         double time_final = (double)(fim - inicio) / CLOCKS_PER_SEC ; \
         printf( COLOR_YELOW "tempo de execulcao: %lf\n" COLOR_RESET, time_final);

#define RESPONSE_PRINT(message) \
    puts("_____RESPOSTA______");\
    printf(type_fmt(message), message);\
    puts("-_________________");

char *gerarStringAletaria(void);
int numerosAletaorios(int seed);

#endif
