gerar_strings_aleatorias.o: 
	gcc -g  -c libtesetes/gerar_strings_aleatorias.c

captura_signal.o: 
	gcc -g -c ./capturar_signal.c

hash.o: 
	gcc -g -c ./hash/hash.c

Queue.o: 
	gcc -g -c ./Queuen/Queue.c


Queuen_rumbro.o: 
	gcc -g -c ./rumbro_negra/Queuen_rumbro.c

arvore_rumbro_negra.o: 
	gcc -g -c ./rumbro_negra/arvore_rumbro_negra.c

websocket.o: 
	gcc -g -c websocket.c

websocket_client.o: 
	gcc -g -c websocket_client.c

websocket_teste: captura_signal.o hash.o Queue.o bin/ arvore_rumbro_negra.o Queuen_rumbro.o gerar_strings_aleatorias.o websocket_client.o websocket.o
	gcc -g websocket_teste.c websocket.o websocket_client.o capturar_signal.o hash.o Queue.o arvore_rumbro_negra.o Queuen_rumbro.o  gerar_strings_aleatorias.o -o bin/websocket_teste -lcrypto
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/websocket_teste

bin/:
	mkdir bin

clean:
	rm *.o
