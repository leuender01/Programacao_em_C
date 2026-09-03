capturar_signal.o:
	gcc  -c ./websocket/capturar_signal.c

http.o:
	gcc  -c http.c

bin/:
	mkdir bin

http_teste: http.o capturar_signal.o bin/
	gcc  ./http_teste.c capturar_signal.o http.o  -o bin/http_teste -pthread
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/http_teste
	rm *.o

clean:
	rm *.o
