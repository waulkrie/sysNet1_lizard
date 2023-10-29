CFLAGS = -g -Wall -std=c++14 -pthread
CC = g++
PROG_NAME = lizard

objects = lizard.o cat.o main.o

.PHONY: all
all: $(PROG_NAME)

$(PROG_NAME): $(objects)
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(objects)


main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp cat.h lizard.h


lizard.o: lizard.cpp
	$(CC) $(CFLAGS) -c lizard.cpp lizard.h


cat.o: cat.cpp
	$(CC) $(CFLAGS) -c cat.cpp cat.h


.PHONY: clean
clean:
	rm $(PROG_NAME) $(objects)
