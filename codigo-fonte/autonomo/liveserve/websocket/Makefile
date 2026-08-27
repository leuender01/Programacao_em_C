captura_signal: capturar_signal.c
	gcc -g -c capturar_signal.c

hash: hash/hash.c
	gcc -g -c hash/hash.c

Queue: Queuen/Queue.c
	gcc -g -c Queuen/Queue.c

Tcp: Tcp/Tcp.c
	gcc -g -c Tcp/Tcp.c

websocket: websocket.c captura_signal hash Queue bin/ Tcp
	gcc -g -c websocket.c
	gcc -g websocket_teste.c websocket.o Tcp/Tcp.o capturar_signal.o hash/hash.o Queuen/Queue.o -o bin/websocket_teste -lcrypto
	valgrind --leak-check=full bin/websocket_teste

bin/:
	mkdir bin
