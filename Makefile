CFLAGS=-Wall -W -pedantic -std=c99 -g

all: hecto key

hecto: hecto.c

key: key.o common.o

clean:
	rm hecto key *.o
