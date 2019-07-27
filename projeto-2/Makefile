# variables
CC=gcc -o
RM=rm -f

all: server/server_udp server/server_tcp client/client

server/server_udp: server/server_udp.c
	$(CC) server/server_udp server/server_udp.c

server/server_tcp: server/server_tcp.c
	$(CC) server/server_tcp server/server_tcp.c

client/client: client/client.c
	$(CC) client/client client/client.c

clean:
	$(RM) server/server_udp 
	$(RM) server/server_tcp
	$(RM) client/client
