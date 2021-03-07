CFLAGS=-Wall -W -pedantic -std=c99 -g

all: hecto

hecto: hecto.c common.o

clean:
	rm hecto *.o
