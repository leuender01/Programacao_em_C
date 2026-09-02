all: main

gerarStringAletaria: gerar_strings_aleatorias.c
	gcc -c ./gerar_strings_aleatorias.c


main: gerarStringAletaria
	gcc gerar_strings_aleatorias.o main.c -o main -pthread
	./main
