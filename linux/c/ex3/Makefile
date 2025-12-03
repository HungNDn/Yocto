CC = gcc
CFLAGS = -Wall -Wextra -O2

all: ex3_main

ex3_main: ex3_main.o
	$(CC) -o ex3_main ex3_main.o

ex3_main.o: ex3_main.c
	$(CC) $(CFLAGS) -c ex3_main.c

clean:
	rm -f *.o ex3_main
