Queue: Queuen_rumbro.c
	gcc -g -c Queuen_rumbro.c

Rumbro_negra: ../arvore_rumbro_negra.c
	gcc -g -c arvore_rumbro_negra.c

Rumbro_negra_teste: Rumbro_negra ./arvore_rumbro_negra_teste.c Queue
	gcc -g arvore_rumbro_negra.o Queuen_rumbro.o arvore_rumbro_negra_teste.c -o bin/arvore_rumbro_negra
	valgrind --leak-check=full bin/arvore_rumbro_negra

bin/:
	mkdir bin

clean:
	rm -f *.o 


