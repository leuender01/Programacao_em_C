#include "http.h"
#include <pthread.h>
#include <unistd.h>
#include "libtesetes/testes.h"

TEST_CASE(http_init)
{
    signal(SIGINT, captura_signal);
    pthread_t http_id;
    pthread_create(&http_id, NULL, http_server, NULL);
    void *result;
    pthread_join( http_id,&result);


    TESTE_PASS();
}

TEST_SUITE(http_init)
