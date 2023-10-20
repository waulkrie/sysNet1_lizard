CFLAGS = -g -Wall -std=c++14 -pthread
CC = g++
PROG_NAME=lizard

objects = lizard.o

.PHONY : all
all: $(PROG_NAME)

$(PROG_NAME): $(objects)
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(objects)

lizard.o: lizard.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c lizard.cpp

.PHONY : clean
clean: 
	rm $(PROG_NAME) $(objects)
