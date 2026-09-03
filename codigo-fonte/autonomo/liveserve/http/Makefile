capturar_signal.o:
	gcc -g -c ./websocket/capturar_signal.c

http.o:
	gcc -g -c http.c

bin/:
	mkdir bin

http_teste: http.o capturar_signal.o bin/
	gcc -g ./http_teste.c capturar_signal.o http.o  -o bin/http_teste -pthread
	rm *.o
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/http_teste
