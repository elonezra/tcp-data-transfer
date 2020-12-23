.PHONY: sender measure clean all local

CC=gcc
CFLAGS=-I. -O3 -Wall -pedantic -g
DEPS = connection.h
PORT=12345

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

all: sender measure

sender: sender.o
		$(CC) -o $@ $^ $(CFLAGS)

measure: measure.o
		$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o sender measure

local: sender measure
	./measure $(PORT) &
	./sender localhost $(PORT)

