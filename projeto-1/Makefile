# variables
CC=gcc
RM=rm -f

all: server client

server: server/server.c
	$(CC) -o server/server server/server.c

client: client/client.c
	$(CC) -o client/client client/client.c

clean:
	$(RM) server/server
	$(RM) client/client

