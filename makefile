CC = g++
CFLAGS = -Wall -g
main: main.o
	$(CC) $(CFLAGS) -o main main.o
main.o:
	$(CC) $(CFLAGS) -c main.cpp
clean:
	rm -f *.o