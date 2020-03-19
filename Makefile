client: client.c valid.c valid.h
	gcc -o client client.c valid.c valid.h

server: server.c valid.c valid.h
	gcc -o server server.c valid.c valid.h

clean:
	rm -f *.o client server
