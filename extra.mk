CFLAGS = -g -Wall -std=c++14 -pthread
CC = g++
PROG_NAME=lizardUni

objects = lizardUni.o

.PHONY : all
all: $(PROG_NAME)

$(PROG_NAME): $(objects)
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(objects)

lizardUni.o: lizardUni.cpp
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c lizardUni.cpp

.PHONY : clean
clean: 
	rm $(PROG_NAME) $(objects)
