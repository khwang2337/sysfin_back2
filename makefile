all: server client

server: server.o networking.o
	gcc -g -o server server.o networking.o

client: client.o networking.o
	gcc -g -o client client.o networking.o

server.o: server.c networking.h
	gcc -g -c server.c

client.o: client.c networking.h
	gcc -g -c client.c

networking.o: networking.c networking.h
	gcc -g -c networking.c

clean:
	rm client
	rm server
	rm *.o
	rm *~
