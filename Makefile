all: client server
 
board_functions.o: board_functions.c board_functions.h
	cc -std=c99 -c -Wall -g board_functions.c 

client.o: client.c board_functions.h
	cc -std=c99 -c -Wall -g client.c

server.o: server.c board_functions.h
	cc -std=c99 -c -Wall -g server.c

clnt: client.o board_functions.o
	cc -std=c99 -g -o client client.o board_functions.o

servr: server.o board_functions.o
	cc -std=c99 -g -o server server.o board_functions.o

clean:
	rm *.o 
	rm client
	rm server

