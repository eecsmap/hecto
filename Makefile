CFLAGS=-Wall -W -pedantic -std=c99 -g

all: key

hecto: hecto.c

key: key.o common.o

clean:
	rm key *.o
