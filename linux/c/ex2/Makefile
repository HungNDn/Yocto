CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread -lrt

all: ex2_main

ex2_main: ex2_main.o user_data.o
	$(CC) -o ex2_main ex2_main.o user_data.o $(LDFLAGS)

ex2_main.o: ex2_main.c user_data.h
	$(CC) $(CFLAGS) -c ex2_main.c

user_data.o: user_data.c user_data.h
	$(CC) $(CFLAGS) -c user_data.c

clean:
	rm -f *.o ex2_main
