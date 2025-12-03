CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread -lrt

all: ex1_main

ex1_main: ex1_main.o user_data.o
	$(CC) -o ex1_main ex1_main.o user_data.o $(LDFLAGS)

ex1_main.o: ex1_main.c user_data.h
	$(CC) $(CFLAGS) -c ex1_main.c

user_data.o: user_data.c user_data.h
	$(CC) $(CFLAGS) -c user_data.c

clean:
	rm -f *.o ex1_main
