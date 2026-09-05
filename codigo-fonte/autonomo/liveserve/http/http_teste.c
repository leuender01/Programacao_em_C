#include "http.h"
#include <unistd.h>
#include "libtesetes/testes.h"

volatile int rodando = 1;
/*

TEST_CASE(resolve_http_get){
    MESSAGE("TESTANDO HEADERS");
    const char* teste_hearders[] = {
        "GET /style.css HTTP/1.1\n\r",
        "GET / HTTP/1.1\n\r",
        "GET /script.js HTTP/1.1\n\r"
    };
    const char* expect_respose[] = {
        "style.css",
        " ",
        "script.js"
    };
    char *resposta = NULL;
    for(int i = 0; i < 2; i++)
    {
        resposta = get_response((const char*)teste_hearders[i]);
        RESPONSE_PRINT(resposta);
        EXPECT_COND(resposta != NULL);
        EXPECTED_EQ("COMPARANDO MENSAGES", strcmp(resposta, expect_respose[i]), 0);
        if(resposta != NULL) free(resposta);
    }
    MESSAGE("TESTANDO CASOS DE ERRO")
    EXPECTED_EQ("ESPERASSE QUE RETORNE NULO",get_response(NULL) , NULL);
    EXPECTED_EQ("ESPERASSE QUE RETORNE NULO",get_response("POST /") , NULL);
    EXPECTED_EQ("ESPERASSE QUE RETORNE NULO",get_response("PATCH /") , NULL);
    EXPECTED_EQ("ESPERASSE QUE RETORNE NULO",get_response("/") , NULL);
    EXPECTED_EQ("ESPERASSE QUE RETORNE NULO",get_response(" / ") , NULL);
    EXPECTED_EQ("ESPERASSE QUE RETORNE NULO",get_response("BALADA") , NULL);
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
    CLOCK_INIT
    char *resposta = file_JsStyle(NULL);
    EXPECTED_EQ("ESPEARA SE QUE SEJA NULO", resposta, NULL);
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    resposta = file_JsStyle("style.css");
    EXPECT_COND(resposta != NULL)
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    resposta = file_JsStyle("script.js");
    EXPECT_COND(resposta != NULL)
    printf("%s\n", (resposta != NULL) ? resposta : "vazio");
    if(resposta != NULL) free(resposta);
    CLOCK_FINISH
    TESTE_PASS()
}
*/

TEST_CASE(http_init)
{
    signal(SIGINT, captura_signal);
    EXPECTED_EQ("ESPERASEE UMA SAIDA VALIDA", http_server(), 0);
    TESTE_PASS();
}
TEST_SUITE(http_init)
