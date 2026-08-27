Queue: Queue.c
	gcc -g -c Queue.c

Queue_teste: Queue bin/
	gcc -g Queue.o Queuen_teste.c -o bin/Queuen_teste
	@echo "execultado ..." 
	valgrind --leak-check=full bin/Queuen_teste
	gdb bin/Queuen_teste

bin/:
	mkdir bin
