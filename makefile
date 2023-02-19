CC = gcc
CFLAGS = -g -Wall -Werror
HEADERS = -pthread -ljson-c

all: multipleServer client

multipleServer: multipleServer.c
	$(CC) $(CFLAGS) $(HEADERS) -o multipleServer multipleServer.c

client: client.c
	$(CC) $(CFLAGS) $(HEADERS) -o client client.c

clean:
	rm -f multipleServer client

fresh:
	make clean 
	make all

