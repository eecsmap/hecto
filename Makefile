CFLAGS=-Wall -W -pedantic -std=c99 -g

all: key hecto

hecto: hecto.c common.o

key: key.o common.o

clean:
	rm key hecto *.o
