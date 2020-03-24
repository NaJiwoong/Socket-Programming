
all: client server

client: client.c valid.c valid.h
	@gcc -o client client.c valid.c valid.h -g

server: server.c valid.c valid.h caesar.c caesar.h
	@gcc -o server server.c valid.c valid.h caesar.c caesar.h -g

clean:
	@rm -f *.o client server

