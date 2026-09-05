capturar_signal.o:
	gcc -g -c websocket/capturar_signal.c

Queue.o:
	gcc -g -c Queuen/Queue.c

gerar_strings_aleatorias.o:
	gcc -g -c libtesetes/gerar_strings_aleatorias.c
monitorar.o:
	gcc -g -c monitorar.c
 
monitorar_teste: Queue.o gerar_strings_aleatorias.o capturar_signal.o bin/ monitorar.o
	gcc -g  monitorar_teste.c gerar_strings_aleatorias.o Queue.o capturar_signal.o monitorar.o -o bin/monitorar_teste
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/monitorar_teste
	rm *.o

bin/:
	mkdir bin

clean:
	rm *.o
