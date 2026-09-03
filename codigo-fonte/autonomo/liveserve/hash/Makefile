hash: hash.c
	gcc  -c hash.c

gerar_strings_aleatorias.o:
	gcc  -c libtesetes/gerar_strings_aleatorias.c
	
hash_teste: ../libtesetes/ hash bin/
	gcc -g gerar_strings_aleatorias.o hash_teste.c hash.o -o bin/hash_teste -lpthread
	valgrind --leak-check=full bin/hash_teste

bin/:
	mkdir bin
