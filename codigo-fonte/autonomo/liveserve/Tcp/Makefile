Tcp.o: 
	gcc  -c Tcp.c

Queue.o: 
	gcc  -c Queuen/Queue.c

bin/:
	mkdir bin

Tcp_teste: Tcp.o bin/ Queue.o
	gcc Tcp.o tcp_teste.c Queue.o -o bin/tcp_teste
	valgrind --leak-check=full bin/tcp_teste
