Tcp: Tcp.c
	gcc -g -c Tcp.c

Queue: Queuen/Queue.c
	gcc -g -c Queuen/Queue.c

bin/:
	mkdir bin

Tcp_teste: Tcp bin/ Queue
	gcc -g Tcp.o tcp_teste.c Queue.o -o bin/tcp_teste
	valgrind --leak-check=full bin/tcp_teste
