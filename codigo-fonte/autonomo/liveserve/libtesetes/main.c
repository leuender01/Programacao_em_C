#include "testes.h"


int b = 10, c= 2;

//TEST_CASE(teste_b)
//{
//    TEST_ASSERT(b == 2);
//    TESTE_PASS();
//}
//
//TEST_SUITE(teste_b)

TEST_CASE(tes_eq){
    EXPECTED_EQ("TESTANDO SE 0 E IGUAL A 0", 0, 0);
    TESTE_PASS();
}

TEST_CASE(tes_aq){
    EXPECTED_EQ("TESTANDO SE 1 E IGUAL A 1", 1, 1);
    TESTE_PASS();
}

TEST_CASE(tes_eae){
    EXPECTED_EQ("TESTANDO SE 2 E IGUAL A 2", 2, 2);
    TESTE_PASS();
}
TEST_CASE(tescond){
    EXPECT_COND(1 == 10);
    TESTE_PASS()
}


TEST_SUITE(tescond, tes_eq, tes_eae, tes_aq)
