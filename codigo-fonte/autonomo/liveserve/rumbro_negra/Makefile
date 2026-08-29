teste: ./libtesetes/gerar_strings_aleatorias.c
	gcc -g -c libtesetes/gerar_strings_aleatorias.c

Queue: Queuen_rumbro.c
	gcc -g -c Queuen_rumbro.c

Queue_teste: Queue bin/ teste
	gcc -g Queuen_rumbro.o gerar_strings_aleatorias.o Queuen_rumbro_teste.c -o bin/Queuen_rumbro_teste
	valgrind --leak-check=full bin/Queuen_rumbro_teste

Rumbro_negra: arvore_rumbro_negra.c
	gcc -g -c arvore_rumbro_negra.c

Rumbro_negra_teste: Rumbro_negra arvore_rumbro_negra_teste.c Queue teste
	gcc -g arvore_rumbro_negra.o Queuen_rumbro.o arvore_rumbro_negra_teste.c gerar_strings_aleatorias.o -o bin/arvore_rumbro_negra_teste
	valgrind --leak-check=full bin/arvore_rumbro_negra_teste

bin/:
	mkdir bin

clean:
	rm -f *.o 


