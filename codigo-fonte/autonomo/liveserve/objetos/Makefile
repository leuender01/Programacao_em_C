captura_signal: ../capturar_signal.c
	gcc -g -c ../capturar_signal.c

hash: ../hash.c
	gcc -g -c ../hash.c

Queue: ../Queue.c
	gcc -g -c ../Queue.c

Rumbro_negra: ../arvore_rumbro_negra.c
	gcc -g -c ../arvore_rumbro_negra.c

Rumbro_negra_teste: Rumbro_negra ../arvore_rumbro_negra_teste.c
	gcc -g arvore_rumbro_negra.o ../arvore_rumbro_negra_teste.c -o bin/arvore_rumbro_negra
	valgrind --leak-check=full bin/arvore_rumbro_negra


Tcp: ../Tcp.c
	gcc -g -c ../Tcp.c

hash_teste: libtesetes/ hash bin/
	gcc -g -c libtesetes/gerar_strings_aleatorias.c
	gcc -g gerar_strings_aleatorias.o hash_teste.c hash.o -o bin/hash_teste
	valgrind --leak-check=full bin/hash_teste


Tcp_teste: Tcp bin/
	gcc -g ./Tcp.o ./tcp_teste.c -o bin/tcp_teste
	valgrind --leak-check=full bin/tcp_teste

websocket: ../websocket.c captura_signal hash Queue bin/
	gcc -g -c ../websocket.c
	gcc -g ./websocket_teste.c ./websocket.o ./Tcp.o ./capturar_signal.o ./hash.o ./Queue.o -o bin/websocket_teste -lcrypto
	valgrind --leak-check=full bin/websocket_teste

Queue_teste: Queue bin/
	gcc -g Queue.o Queuen_teste.c -o bin/Queuen_teste
	@echo "execultado ..." 
	valgrind --leak-check=full bin/Queuen_teste
	gdb bin/Queuen_teste

bin/:
	mkdir bin

clean:
	rm -f *.o 


