CC=gcc 	# the C compiler is gcc
CFLAGS=-g -Wall -pthread

all: server twitsay twithear twitspeak rapid

server: server.o 
	$(CC) $(CFLAGS) -o $@ server.o
					
twitsay: twitsay.o
	$(CC) $(CFLAGS) -o $@ twitsay.o
					
twithear: twithear.o
	$(CC) $(CFLAGS) -o $@ twithear.o

twitspeak: twitspeak.o
	$(CC) $(CFLAGS) -o $@ twitspeak.o

rapid: rapid.o
	$(CC) $(CFLAGS) -o $@ rapid.o


server.c: server.h

clean:
	rm -rf server twitsay twithear twitspeak rapid *.o