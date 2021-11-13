PROG = spclock
CC = g++
CPPFLAGS = -Wall -O2
OBJS = main.o spclock.o

all: $(OBJS)
	$(CC) -o $(PROG) $(OBJS)
main.o:
	$(CC) $(CPPFLAGS) -c main.cpp
spclock.o:
	$(CC) $(CPPFLAGS) -c spclock.cpp

clean:
	rm -f $(PROG) $(OBJS)
