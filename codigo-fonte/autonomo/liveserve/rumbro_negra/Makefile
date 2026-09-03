gerar_strings_aleatorias.o:
	gcc  -c libtesetes/gerar_strings_aleatorias.c
Queuen_rumbro.o: 
	gcc  -c Queuen_rumbro.c

Queue_teste: Queuen_rumbro.o bin/ gerar_strings_aleatorias.o
	gcc  Queuen_rumbro.o gerar_strings_aleatorias.o Queuen_rumbro_teste.c -o bin/Queuen_rumbro_teste -pthread
	valgrind --leak-check=full bin/Queuen_rumbro_teste

arvore_rumbro_negra.o: arvore_rumbro_negra.c 
	gcc  -c arvore_rumbro_negra.c 

Rumbro_negra_teste: arvore_rumbro_negra.o arvore_rumbro_negra_teste.c  gerar_strings_aleatorias.o
	gcc arvore_rumbro_negra.o Queuen_rumbro.o arvore_rumbro_negra_teste.c gerar_strings_aleatorias.o -o bin/arvore_rumbro_negra_teste -pthread
	valgrind --leak-check=full bin/arvore_rumbro_negra_teste

bin/:
	mkdir bin

clean:
	rm -f *.o 


