#include "http.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libtesetes/testes.h"

extern char *file_html(char* path);
extern char *file_JsStyle(const char *path);

TEST_CASE(http_init)
{
    signal(SIGINT, captura_signal);
    pthread_t http_id;
    pthread_create(&http_id, NULL, http_server, NULL);
    void *result;
    pthread_join( http_id,&result);
    EXPECTED_EQ("ESPERASEE UMA SAIDA VALIDA", (intptr_t)result, 0);
    TESTE_PASS();
}

TEST_CASE(file_Html)
{
    char *resposta = file_html(NULL);
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    resposta = file_html("bola.html");
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    TESTE_PASS();
}

TEST_CASE(file_js)
{
//    CLOCK_INIT;
    char *resposta = file_JsStyle(NULL);
    EXPECTED_EQ("ESPEARA SE QUE SEJA NULO", resposta, NULL);
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    resposta = file_JsStyle("style.css");
    EXPECT_COND(resposta != NULL);
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    resposta = file_JsStyle("script.js");
    EXPECT_COND(resposta != NULL);
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
//    CLOCK_FINISH;
    TESTE_PASS();
}

TEST_SUITE(file_js, file_Html, http_init)
