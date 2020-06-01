all: client
 
board_functions.o: board_functions.c board_functions.h
	cc -c -Wall -g board_functions.c 

client.o: client.c board_functions.h
	cc -c -Wall -g client.c 

clnt: client.o board_functions.o
	cc -g -o client client.o board_functions.o

clean:
	rm *.o 
	rm client
