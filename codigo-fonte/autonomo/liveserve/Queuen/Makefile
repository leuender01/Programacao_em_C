Queue.o: 
	gcc  -c Queue.c

Queue_teste: Queue.o bin/
	gcc  Queue.o Queuen_teste.c -o bin/Queuen_teste
	@echo "execultado ..." 
	valgrind --leak-check=full bin/Queuen_teste

bin/:
	mkdir bin
