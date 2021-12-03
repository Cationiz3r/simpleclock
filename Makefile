PROG = spclock
CC = g++
CPPFLAGS = -Wall -O2

all:
	$(CC) $(CPPFLAGS) spclock.c -o $(PROG)
clean:
	rm -f spclock
