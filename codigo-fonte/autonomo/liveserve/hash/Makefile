hash: hash.c
	gcc -g -c hash.c

hash_teste: ../libtesetes/ hash bin/
	gcc -g -c libtesetes/gerar_strings_aleatorias.c
	gcc -g gerar_strings_aleatorias.o hash_teste.c hash.o -o bin/hash_teste -lpthread
	valgrind --leak-check=full bin/hash_teste

bin/:
	mkdir bin
