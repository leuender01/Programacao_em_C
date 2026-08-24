Tcp: ../Tcp.c
	gcc -g -c ../Tcp.c
	gcc -g ./Tcp.o ./tcp_teste.c -o ./tcp_teste

websocket: ../websocket.c captura_signal hash
	gcc -g -c ../websocket.c
	gcc -g ./websocket_teste.c ./websocket.o ./Tcp.o ./capturar_signal.o ./hash.o -o ./websocket_teste -lcrypto

captura_signal: ../capturar_signal.c
	gcc -g -c ../capturar_signal.c

hash: ../hash.c
	gcc -g -c ../hash.c

clean:
	rm -f *.o 


