all: server client
client: client.c
	gcc -Wall -pedantic -std=c99 client.c -o client -lpthread
server: server.c
	gcc -Wall -pedantic -std=c99 server.c -o server -lpthread
.PHONY: clean
clean:
	rm -rf client
	rm -rf server
