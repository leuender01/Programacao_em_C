gerar_strings_aleatorias.o: 
	gcc  -c libtesetes/gerar_strings_aleatorias.c

captura_signal.o: 
	gcc  -c ./capturar_signal.c

hash.o: 
	gcc -c ./hash/hash.c

Queue.o: 
	gcc  -c ./Queuen/Queue.c

Tcp.o: 
	gcc  -c ./Tcp/Tcp.c

Queuen_rumbro.o: 
	gcc  -c ./rumbro_negra/Queuen_rumbro.c

arvore_rumbro_negra.o: 
	gcc  -c ./rumbro_negra/arvore_rumbro_negra.c

websocket.o: 
	gcc -c websocket.c

websocket_client.o: 
	gcc  -c websocket_client.c

websocket_teste: captura_signal.o hash.o Queue.o bin/ Tcp.o arvore_rumbro_negra.o Queuen_rumbro.o gerar_strings_aleatorias.o websocket_client.o
	gcc  websocket_teste.c websocket.o websocket_client.o Tcp.o capturar_signal.o hash.o Queue.o arvore_rumbro_negra.o Queuen_rumbro.o  gerar_strings_aleatorias.o -o bin/websocket_teste -lcrypto
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/websocket_teste

bin/:
	mkdir bin
